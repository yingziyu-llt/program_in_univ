#include <iostream>
#include <cuda_runtime.h>
#include <random>
#include <ctime>

constexpr int block_size = 256;
constexpr int unroll_factor = 4;  // 每个线程处理4个char

__global__ void histogram(const unsigned int* source, int* hist, int dataSize) {
    // 每个线程处理unroll_factor个char
    int idx = (blockIdx.x * blockDim.x + threadIdx.x) * unroll_factor;

    // 局部直方图，减少全局内存原子操作
    __shared__ int local_hist[256];
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        local_hist[i] = 0;
    }
    __syncthreads();

    // 处理数据
    if (idx < dataSize) {
        unsigned int value_u32 = source[idx / unroll_factor];  // 读取一个int（4个char）
        for (int i = 0; i < unroll_factor; i++) {
            unsigned char charValue = (value_u32 >> (8 * i)) & 0xFF;  // 提取每个char
            atomicAdd(&local_hist[charValue], 1);  // 更新局部直方图
        }
    }
    __syncthreads();

    // 将局部直方图合并到全局直方图
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&hist[i], local_hist[i]);
    }
}

int main() {
    const int dataSize = 100000000;  // 数据大小
    char* source = new char[dataSize];
    int* hist = new int[256];

    // 初始化数据
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < dataSize; i++) {
        source[i] = static_cast<char>(dis(gen));
    }

    // 初始化直方图
    for (int i = 0; i < 256; i++) {
        hist[i] = 0;
    }

    // 设备端内存分配
    unsigned int* d_source;
    int* d_hist;
    cudaMalloc((void**)&d_source, (dataSize / unroll_factor) * sizeof(unsigned int));
    cudaMalloc((void**)&d_hist, 256 * sizeof(int));

    // 将数据从主机拷贝到设备
    cudaMemcpy(d_source, reinterpret_cast<unsigned int*>(source), (dataSize / unroll_factor) * sizeof(unsigned int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_hist, hist, 256 * sizeof(int), cudaMemcpyHostToDevice);

    // 启动核函数
    int grid_size = (dataSize / unroll_factor + block_size - 1) / block_size;
    clock_t time_start = clock();
    histogram<<<grid_size, block_size>>>(d_source, d_hist, dataSize);
    cudaDeviceSynchronize();
    clock_t time_end = clock();

    // 将直方图从设备拷贝回主机
    cudaMemcpy(hist, d_hist, 256 * sizeof(int), cudaMemcpyDeviceToHost);

    // 打印耗时
    std::cout << "Time: " << (time_end - time_start) * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;

    // 释放设备内存
    cudaFree(d_source);
    cudaFree(d_hist);

    // 释放主机内存
    delete[] source;
    delete[] hist;

    return 0;
}