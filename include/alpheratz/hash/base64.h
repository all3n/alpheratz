#pragma once
#include <string>
#include <string_view>
#include <absl/status/status.h>

namespace alpheratz {
namespace hash {

// 1 success or 0 failure
int EncodeNeedSize(size_t in_size);
void Base64Encode(std::string_view s, std::string &out);

int DecodeNeedSize(size_t in_size);
void Base64Decode(std::string_view s, std::string &out);

}  // namespace hash
}  // namespace alpheratz
