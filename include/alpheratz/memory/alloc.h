#pragma once
#include <unordered_map>
namespace alpheratz {
namespace alloc {
constexpr int kBlockSize = 4 * 1024;  // 512k
struct StflyMemBlock {
    uint8_t *start;
    size_t size;
};
class MemoryManagerAllocator {
   public:
    MemoryManagerAllocator(MemoryManagerAllocator &) = delete;
    MemoryManagerAllocator(MemoryManagerAllocator &&) = delete;
    static MemoryManagerAllocator &GetInstance() {
        static MemoryManagerAllocator instance;
        return instance;
    }
    int GetBlockSize() const { return block_size_; };
    void SetBlockSize(int block_size) { block_size_ = block_size; }
    uint32_t GetBlock(size_t real_size) const {
        int n_blocks = real_size / block_size_;
        return n_blocks;
    }

   private:
    MemoryManagerAllocator() {}
    int block_size_{kBlockSize};
    std::unordered_map<uint32_t, StflyMemBlock> blocks_;
    int block_count_{0};
};
}  // namespace alloc
}  // namespace alpheratz
