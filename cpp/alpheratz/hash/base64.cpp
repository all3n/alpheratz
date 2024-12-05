#include <alpheratz/hash/base64.h>
#include <openssl/base64.h>

namespace alpheratz {
namespace hash {

void Base64Encode(std::string_view s, std::string &out) {
    size_t outlen = 0;
    EVP_EncodedLength(&outlen, s.size());
    out.resize(outlen - 1);
    EVP_EncodeBlock(reinterpret_cast<uint8_t *>(out.data()),
                    reinterpret_cast<const uint8_t *>(s.data()), s.size());
}
int Base64Decode(std::string_view in, uint8_t *out, size_t buf_len) {
    size_t outlen = 0;
    // EVP_DecodedLength(&outlen, in.size());
    EVP_DecodeBase64(out, &outlen, buf_len, reinterpret_cast<const uint8_t *>(in.data()),
                     in.size());
    return outlen;
}
}  // namespace hash
}  // namespace alpheratz
