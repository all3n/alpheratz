#pragma once
#include <string>
#include <string_view>

namespace alpheratz {
namespace hash {
void Md5(const unsigned char *key, unsigned int length, unsigned char *result);
void Md5(std::string_view s, char *output);
}  // namespace hash
}  // namespace alpheratz
