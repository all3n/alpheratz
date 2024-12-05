#include <alpheratz/compress/zstd.h>
#include <zstd.h>
#include <zstd_errors.h>

namespace alpheratz {
namespace compress {

absl::Status CompressZstd(std::vector<uint8_t> &in, std::vector<uint8_t> &out) {
    size_t out_bound = ZSTD_compressBound(in.size());
    out.resize(out_bound);
    size_t compressed_size =
        ZSTD_compress(out.data(), out_bound, in.data(), in.size(), ZSTD_defaultCLevel());
    auto error_code = ZSTD_isError(compressed_size);
    if (error_code) {
        return absl::InternalError(ZSTD_getErrorName(error_code));
    }
    out.resize(compressed_size);
    return absl::OkStatus();
}
absl::Status UnCompressZstd(std::vector<uint8_t> &in, std::vector<uint8_t> &out) {
    size_t de_compress_size = ZSTD_getFrameContentSize(in.data(), in.size());
    if (de_compress_size == 0) {
        return absl::InvalidArgumentError("input is empty");
    }
    if (de_compress_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        return absl::InvalidArgumentError("content size is unknown");
    }
    if (de_compress_size == ZSTD_CONTENTSIZE_ERROR) {
        return absl::InvalidArgumentError("content size is error");
    }
    out.resize(de_compress_size);
    size_t raw_uncompress_size = ZSTD_decompress(out.data(), out.size(), in.data(), in.size());
    auto error_code = ZSTD_isError(raw_uncompress_size);
    if (error_code) {
        return absl::InternalError(ZSTD_getErrorName(error_code));
    }
    out.resize(raw_uncompress_size);
    return absl::OkStatus();
}

}  // namespace compress

}  // namespace alpheratz
