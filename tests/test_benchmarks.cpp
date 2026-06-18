#include <cstdio>
#include <cstring>
#include <iostream>
#include <new>
#include <vector>

#include "Allocator.h"
#include "DataTypes.h"
#include "benchmarks_example.h"

static int g_passed = 0;
static int g_failed = 0;

#define CHECK(cond) \
    do { \
        if (cond) { \
            std::cout << "  PASS: " #cond "\n"; \
            g_passed++; \
        } else { \
            std::cout << "  FAIL: " #cond "  [line " << __LINE__ << "]\n"; \
            g_failed++; \
        } \
    } while (0)


void test_measure_microseconds_returns_nonneg()
{
    std::cout << "\n[MeasureMicroseconds]\n";

    long long t = MeasureMicroseconds([]() {});
    CHECK(t >= 0);

    long long t2 = MeasureMicroseconds([]() {
        volatile int x = 0;
        for (int i = 0; i < 1000000; i++) x += i;
    });
    CHECK(t2 > 0);
    printf("  1M-loop: %lld us\n", t2);
}


void test_allocator_heap_blocks_stats()
{
    std::cout << "\n[Allocator -- heap-blocks stats]\n";

    const int N = 100;
    Allocator alloc(32);

    std::vector<void *> ptrs(N);
    for (int i = 0; i < N; i++)
        ptrs[i] = alloc.Allocate(32);

    CHECK(alloc.GetAllocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == (UINT)N);
    CHECK(alloc.GetDeallocations() == 0u);

    for (int i = 0; i < N; i++)
        alloc.Deallocate(ptrs[i]);

    CHECK(alloc.GetDeallocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == 0u);
}

void test_allocator_heap_pool_stats()
{
    std::cout << "\n[Allocator -- heap-pool stats]\n";

    const int N = 50;
    Allocator alloc(64, N);

    std::vector<void *> ptrs(N);
    for (int i = 0; i < N; i++)
        ptrs[i] = alloc.Allocate(64);

    CHECK(alloc.GetAllocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == (UINT)N);

    for (int i = 0; i < N; i++)
        alloc.Deallocate(ptrs[i]);

    CHECK(alloc.GetDeallocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == 0u);

    void *p = alloc.Allocate(64);
    CHECK(p != nullptr);
    CHECK(alloc.GetBlocksInUse() == 1u);
    alloc.Deallocate(p);
    CHECK(alloc.GetBlocksInUse() == 0u);
}

void test_allocator_static_pool_stats()
{
    std::cout << "\n[Allocator -- static-pool stats]\n";

    const int N = 20;
    const size_t blockSize = 16;
    std::vector<char> buf(blockSize * N);
    Allocator alloc(blockSize, N, buf.data()); 

    std::vector<void *> ptrs(N);
    for (int i = 0; i < N; i++)
        ptrs[i] = alloc.Allocate(blockSize);

    CHECK(alloc.GetAllocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == (UINT)N);

    for (int i = 0; i < N; i++)
        alloc.Deallocate(ptrs[i]);

    CHECK(alloc.GetDeallocations() == (UINT)N);
    CHECK(alloc.GetBlocksInUse() == 0u);
}

void test_benchmark_mode_heap_returns_positive_time()
{
    std::cout << "\n[BenchmarkAllocatorMode -- heap]\n";

    const int N = 10000;
    std::vector<void *> ptrs(N);

    long long t = BenchmarkAllocatorMode(
        "Heap", N, 32, ptrs,
        [](int size) { return static_cast<void *>(new char[size]); },
        [](void *p)  { delete[] static_cast<char *>(p); });

    CHECK(t > 0);
    printf("  total: %lld us\n", t);
}

void test_benchmark_mode_heap_pool_returns_positive_time()
{
    std::cout << "\n[BenchmarkAllocatorMode -- heap pool]\n";

    const int N = 10000;
    Allocator poolAlloc(32, N);
    std::vector<void *> ptrs(N);

    long long t = BenchmarkAllocatorMode(
        "HeapPool", N, 32, ptrs,
        [&](int size) { return poolAlloc.Allocate(size); },
        [&](void *p)  { poolAlloc.Deallocate(p); });

    CHECK(t > 0);
    printf("  total: %lld us\n", t);
}

void test_benchmark_mode_static_pool_returns_positive_time()
{
    std::cout << "\n[BenchmarkAllocatorMode -- static pool]\n";

    const int N = 10000;
    const size_t blockSize = 32;
    std::vector<char> buf(blockSize * N);
    Allocator staticAlloc(blockSize, N, buf.data());
    std::vector<void *> ptrs(N);

    long long t = BenchmarkAllocatorMode(
        "StaticPool", N, (int)blockSize, ptrs,
        [&](int size) { return staticAlloc.Allocate(size); },
        [&](void *p)  { staticAlloc.Deallocate(p); });

    CHECK(t > 0);
    printf("  total: %lld us\n", t);
}


void test_pool_speed_comparison()
{
    std::cout << "\n[Pool vs heap speed comparison (100k allocs)]\n";

    const int N = 100000;
    const size_t blockSize = 64;
    std::vector<char> staticBuf(blockSize * N);
    Allocator heapPool(blockSize, N);
    Allocator staticPool(blockSize, N, staticBuf.data());
    std::vector<void *> ptrs(N);

    long long t_heap = BenchmarkAllocatorMode(
        "Heap", N, (int)blockSize, ptrs,
        [](int s)   { return static_cast<void *>(new char[s]); },
        [](void *p) { delete[] static_cast<char *>(p); });

    long long t_hpool = BenchmarkAllocatorMode(
        "HeapPool", N, (int)blockSize, ptrs,
        [&](int s)  { return heapPool.Allocate(s); },
        [&](void *p){ heapPool.Deallocate(p); });

    long long t_spool = BenchmarkAllocatorMode(
        "StaticPool", N, (int)blockSize, ptrs,
        [&](int s)  { return staticPool.Allocate(s); },
        [&](void *p){ staticPool.Deallocate(p); });

    printf("  heap=%lld us  heap-pool=%lld us  static-pool=%lld us\n",
           t_heap, t_hpool, t_spool);

    CHECK(t_heap > 0);
    CHECK(t_hpool > 0);
    CHECK(t_spool > 0);
    CHECK(t_hpool < t_heap * 10);
    CHECK(t_spool < t_heap * 10);
}


void test_bad_alloc_benchmark_does_not_crash()
{
    std::cout << "\n[RunAllocatorBadAllocBenchmarks]\n";
    bool ok = false;
    try {
        RunAllocatorBadAllocBenchmarks();
        ok = true;
    } catch (...) {}
    CHECK(ok);
}

static void out_of_memory() { throw std::bad_alloc(); }

int main()
{
    std::set_new_handler(out_of_memory);

    test_measure_microseconds_returns_nonneg();

    test_allocator_heap_blocks_stats();
    test_allocator_heap_pool_stats();
    test_allocator_static_pool_stats();

    test_benchmark_mode_heap_returns_positive_time();
    test_benchmark_mode_heap_pool_returns_positive_time();
    test_benchmark_mode_static_pool_returns_positive_time();

    test_pool_speed_comparison();

    test_bad_alloc_benchmark_does_not_crash();

    std::cout << "Results: " << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed > 0 ? 1 : 0;
}
