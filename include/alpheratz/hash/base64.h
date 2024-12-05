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
int Base64Decode(std::string_view in, uint8_t *out, size_t buf_len) ;

}  // namespace hash
}  // namespace alpheratz
