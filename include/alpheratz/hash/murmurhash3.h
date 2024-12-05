#pragma once
#include <string>
namespace alpheratz {
namespace hash {

void MurmurHash3X8632(const void *key, int len, uint32_t seed, void *out);
void MurmurHash3X86128(const void *key, int len, uint32_t seed, void *out);
int32_t MurmurHash32(const std::string &str);
int32_t MurmurHash32(const std::string &str, uint32_t seed);
int64_t MurmurHash64(const std::string &str);
int64_t MurmurHash64(const std::string &str, uint32_t seed);

}  // namespace hash
}  // namespace alpheratz
