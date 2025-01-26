#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cuda_runtime.h>

typedef double d_t;

// 3D点结构体
struct d3_t {
    d_t x, y, z;
};

// 计算3D向量的欧几里得范数
__host__ __device__
inline d_t norm(d_t dx, d_t dy, d_t dz) {
    return sqrt(dx * dx + dy * dy + dz * dz);
}

// 预处理核函数：计算反射镜点到光源的距离
__global__
void precomputeMirDistances(const d_t* __restrict__ mir_x,
                            const d_t* __restrict__ mir_y,
                            const d_t* __restrict__ mir_z,
                            int64_t mirn,
                            d3_t src,
                            d_t* __restrict__ distances) {
    const int64_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < mirn) {
        const d_t dx = mir_x[idx] - src.x;
        const d_t dy = mir_y[idx] - src.y;
        const d_t dz = mir_z[idx] - src.z;
        distances[idx] = norm(dx, dy, dz);
    }
}

// 主核函数：计算传感器点的 Iv 值
__global__
void computeIvKernel(const d_t* __restrict__ mir_x,
                     const d_t* __restrict__ mir_y,
                     const d_t* __restrict__ mir_z,
                     const d_t* __restrict__ mir_dist,
                     int64_t mirn,
                     const d_t* __restrict__ sen_x,
                     const d_t* __restrict__ sen_y,
                     const d_t* __restrict__ sen_z,
                     int64_t senn,
                     d_t* __restrict__ data) {
    extern __shared__ d_t smem[];
    d_t* real_sums = smem;
    d_t* imag_sums = smem + blockDim.x;

    const int sen_idx = blockIdx.x;
    if (sen_idx >= senn) return;

    const d_t sv_x = sen_x[sen_idx];
    const d_t sv_y = sen_y[sen_idx];
    const d_t sv_z = sen_z[sen_idx];

    const int tid = threadIdx.x;
    const int nthreads = blockDim.x;

    d_t sum_real = 0.0, sum_imag = 0.0;
    const d_t factor = 2.0 * M_PI / 500e-6;

    for (int64_t mu = tid; mu < mirn; mu += nthreads) {
        const d_t dx = mir_x[mu] - sv_x;
        const d_t dy = mir_y[mu] - sv_y;
        const d_t dz = mir_z[mu] - sv_z;
        const d_t dist = mir_dist[mu] + norm(dx, dy, dz);
        const d_t angle = dist * factor;

        sum_real += cos(angle);
        sum_imag += sin(angle);
    }

    real_sums[tid] = sum_real;
    imag_sums[tid] = sum_imag;
    __syncthreads();

    // 树状归约
    for (int s = nthreads / 2; s > 0; s >>= 1) {
        if (tid < s) {
            real_sums[tid] += real_sums[tid + s];
            imag_sums[tid] += imag_sums[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        const d_t total_real = real_sums[0];
        const d_t total_imag = imag_sums[0];
        data[sen_idx] = sqrt(total_real * total_real + total_imag * total_imag);
    }
}

int main() {
    // 打开输入文件
    FILE* fi = fopen("in.data", "rb");
    if (!fi) {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    // 读取点光源坐标
    d3_t src;
    fread(&src, sizeof(d3_t), 1, fi);

    // 读取反射镜点数量和坐标
    int64_t mirn;
    fread(&mirn, sizeof(int64_t), 1, fi);
    d_t* mir_x = (d_t*)malloc(mirn * sizeof(d_t));
    d_t* mir_y = (d_t*)malloc(mirn * sizeof(d_t));
    d_t* mir_z = (d_t*)malloc(mirn * sizeof(d_t));
    for (int64_t i = 0; i < mirn; i++) {
        d3_t point;
        fread(&point, sizeof(d3_t), 1, fi);
        mir_x[i] = point.x;
        mir_y[i] = point.y;
        mir_z[i] = point.z;
    }

    // 读取传感器点数量和坐标
    int64_t senn;
    fread(&senn, sizeof(int64_t), 1, fi);
    d_t* sen_x = (d_t*)malloc(senn * sizeof(d_t));
    d_t* sen_y = (d_t*)malloc(senn * sizeof(d_t));
    d_t* sen_z = (d_t*)malloc(senn * sizeof(d_t));
    for (int64_t i = 0; i < senn; i++) {
        d3_t point;
        fread(&point, sizeof(d3_t), 1, fi);
        sen_x[i] = point.x;
        sen_y[i] = point.y;
        sen_z[i] = point.z;
    }

    fclose(fi);

    // 分配主机端结果存储
    d_t* data = (d_t*)malloc(senn * sizeof(d_t));

    // 分配设备端内存
    d_t *d_mir_x, *d_mir_y, *d_mir_z, *d_mir_dist;
    d_t *d_sen_x, *d_sen_y, *d_sen_z, *d_data;
    cudaMalloc(&d_mir_x, mirn * sizeof(d_t));
    cudaMalloc(&d_mir_y, mirn * sizeof(d_t));
    cudaMalloc(&d_mir_z, mirn * sizeof(d_t));
    cudaMalloc(&d_mir_dist, mirn * sizeof(d_t));
    cudaMalloc(&d_sen_x, senn * sizeof(d_t));
    cudaMalloc(&d_sen_y, senn * sizeof(d_t));
    cudaMalloc(&d_sen_z, senn * sizeof(d_t));
    cudaMalloc(&d_data, senn * sizeof(d_t));

    // 将数据拷贝到设备端
    cudaMemcpy(d_mir_x, mir_x, mirn * sizeof(d_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_mir_y, mir_y, mirn * sizeof(d_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_mir_z, mir_z, mirn * sizeof(d_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_sen_x, sen_x, senn * sizeof(d_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_sen_y, sen_y, senn * sizeof(d_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_sen_z, sen_z, senn * sizeof(d_t), cudaMemcpyHostToDevice);

    // 预处理反射镜点到光源的距离
    const int BLOCK_SIZE = 256;
    precomputeMirDistances<<<(mirn + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE>>>(
        d_mir_x, d_mir_y, d_mir_z, mirn, src, d_mir_dist
    );

    // 主计算核函数
    size_t shared_mem = 2 * BLOCK_SIZE * sizeof(d_t);
    computeIvKernel<<<senn, BLOCK_SIZE, shared_mem>>>(
        d_mir_x, d_mir_y, d_mir_z, d_mir_dist, mirn,
        d_sen_x, d_sen_y, d_sen_z, senn, d_data
    );

    // 将结果拷贝回主机端
    cudaMemcpy(data, d_data, senn * sizeof(d_t), cudaMemcpyDeviceToHost);

    // 将结果写入输出文件
    FILE* fo = fopen("out.data", "wb");
    if (!fo) {
        perror("Failed to open output file");
        free(mir_x); free(mir_y); free(mir_z);
        free(sen_x); free(sen_y); free(sen_z);
        free(data);
        cudaFree(d_mir_x); cudaFree(d_mir_y); cudaFree(d_mir_z); cudaFree(d_mir_dist);
        cudaFree(d_sen_x); cudaFree(d_sen_y); cudaFree(d_sen_z); cudaFree(d_data);
        return EXIT_FAILURE;
    }
    fwrite(data, sizeof(d_t), senn, fo);
    fclose(fo);

    // 释放主机内存
    free(mir_x); free(mir_y); free(mir_z);
    free(sen_x); free(sen_y); free(sen_z);
    free(data);

    // 释放设备内存
    cudaFree(d_mir_x); cudaFree(d_mir_y); cudaFree(d_mir_z); cudaFree(d_mir_dist);
    cudaFree(d_sen_x); cudaFree(d_sen_y); cudaFree(d_sen_z); cudaFree(d_data);

    return 0;
}