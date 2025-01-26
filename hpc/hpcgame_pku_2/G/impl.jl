import Pkg
Pkg.activate(temp=true)
Pkg.add("DataStructures")

using Base.Threads
using DataStructures

function topk(a::Vector{T}, k::Int) where T <: Real
    n = length(a)
    k = min(k, n)  # 处理 k > n 的情况
    num_chunks = min(8, n)  # 动态调整分块数量
    chunk_size = n ÷ num_chunks

    # 每个块的前 k 个元素
    chunk_results = [Vector{Tuple{T, Int}}() for _ in 1:num_chunks]

    # 并行化分块处理
    @threads for i in 1:num_chunks
        start = (i - 1) * chunk_size + 1
        end_ = i == num_chunks ? n : i * chunk_size
        pq = BinaryMinHeap{Tuple{T, Int}}()  # 最小堆

        # 初始化堆
        for j in start:min(start + k - 1, end_)
            push!(pq, (a[j], j))
        end

        # 维护堆大小为 k
        for j in start + k:end_
            if a[j] > first(pq)[1]  # 如果当前值大于堆顶的最小值
                pop!(pq)
                push!(pq, (a[j], j))
            end
        end

        # 提取当前块的前 k 个元素
        while !isempty(pq)
            push!(chunk_results[i], pop!(pq))
        end
    end

    # 合并所有块的结果
    finalPq = BinaryMinHeap{Tuple{T, Int}}()
    for chunk in chunk_results
        for item in chunk
            push!(finalPq, item)
            if length(finalPq) > k
                pop!(finalPq)
            end
        end
    end

    # 提取前 k 个元素（按值降序排列）
    res = Vector{Int}(undef, k)
    for i in k:-1:1
        res[i] = last(pop!(finalPq))
    end

    return res
end