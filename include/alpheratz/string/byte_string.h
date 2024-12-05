#pragma once
#include <absl/strings/string_view.h>

#include <string>
#include <vector>
namespace alpheratz {
namespace string {
int HexStringToBytes(const std::string &data, std::vector<uint8_t> &out);
std::string BytesToHexString(std::vector<uint8_t> &data, absl::string_view delim = "");
}  // namespace string
}  // namespace alpheratz
