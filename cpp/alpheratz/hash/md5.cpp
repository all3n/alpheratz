#include <alpheratz/hash/md5.h>
#include <openssl/md5.h>
namespace alpheratz {
namespace hash {

// input: key[length]
// output: result, must sure result length > 16
void Md5(const unsigned char *key, unsigned int length, unsigned char *result) {
    MD5(key, length, result);
}
void Md5(std::string_view s, char *output) {
    unsigned char md5_hash[MD5_DIGEST_LENGTH];
    alpheratz::hash::Md5(reinterpret_cast<const unsigned char *>(s.data()), s.length(), md5_hash);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", md5_hash[i]);
    }
}
}  // namespace hash
}  // namespace alpheratz
