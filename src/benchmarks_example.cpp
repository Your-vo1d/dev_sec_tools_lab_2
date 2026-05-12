#include "benchmarks_example.h"

void PrintBenchmarkSectionHeader(const char *title)
{
    std::cout << "\n\t" << title << "\t\t\t\n";
}

void PrintBenchmarkModeHeader(const char *modeName)
{
    std::cout << "  [" << modeName << "]\n";
}

void RunAllocatorBadAllocBenchmarks()
{
    constexpr std::size_t allocationSize = 8u * 1024u * 1024u;
    constexpr std::size_t allocationLimit = 64u;
    constexpr std::size_t poolObjects = 32u;

    std::vector<char> staticMemoryPool(allocationSize * poolObjects);

    Allocator allocatorHeapBlocks(allocationSize);
    Allocator allocatorHeapPool(allocationSize, static_cast<UINT>(poolObjects));
    Allocator allocatorStaticPool(allocationSize, static_cast<UINT>(poolObjects), staticMemoryPool.data());

    PrintBenchmarkSectionHeader("Allocator bad_alloc benchmarks");

    BenchmarkAllocatorBadAllocMode(
        "Heap", allocationSize, allocationLimit,
        [](std::size_t size)
        { return static_cast<void *>(new char[size]); },
        [](void *ptr)
        { delete[] static_cast<char *>(ptr); });

    BenchmarkAllocatorBadAllocMode(
        "Heap Blocks", allocationSize, allocationLimit,
        [&](std::size_t size)
        { return allocatorHeapBlocks.Allocate(size); },
        [&](void *ptr)
        { allocatorHeapBlocks.Deallocate(ptr); });

    BenchmarkAllocatorBadAllocMode(
        "Heap Pool", allocationSize, allocationLimit,
        [&](std::size_t size)
        { return allocatorHeapPool.Allocate(size); },
        [&](void *ptr)
        { allocatorHeapPool.Deallocate(ptr); });

    BenchmarkAllocatorBadAllocMode(
        "Static Pool", allocationSize, allocationLimit,
        [&](std::size_t size)
        { return allocatorStaticPool.Allocate(size); },
        [&](void *ptr)
        { allocatorStaticPool.Deallocate(ptr); });
}

void RunAllocatorBenchmarks()
{
    std::cout << "Allocator benchmarks" << std::endl;

    BenchmarkType<BBV>("BBV");
    BenchmarkType<BoolInterval>("BoolInterval");
    BenchmarkType<BoolEquation>("BoolEquation");
    BenchmarkType<NodeBoolTree>("NodeBoolTree");
    BenchmarkType<MinDontCareBranchingStrategy>("MinDontCareBranchingStrategy");
    BenchmarkType<FirstFreeColumnBranchingStrategy>("FirstFreeColumnBranchingStrategy");

    RunAllocatorBadAllocBenchmarks();
}
