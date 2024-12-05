#pragma once
#include <string>
#include <string_view>

namespace alpheratz {
namespace hash {
void Md5Hash(const unsigned char *key, unsigned int length, unsigned char *result);
void Md5Hash(std::string_view s, char *output);
void Md5Hash(std::string_view s, std::string &output);
}  // namespace hash
}  // namespace alpheratz
