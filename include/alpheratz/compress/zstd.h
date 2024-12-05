#pragma once

#include <absl/status/status.h>
namespace alpheratz {
namespace compress {

absl::Status CompressZstd(std::vector<uint8_t> &in, std::vector<uint8_t> &out);
absl::Status UnCompressZstd(std::vector<uint8_t> &in, std::vector<uint8_t> &out);

}  // namespace compress
}  // namespace alpheratz
