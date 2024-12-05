#include <alpheratz/hash/md5.h>
#include <openssl/md5.h>
namespace alpheratz {
namespace hash {

// input: key[length]
// output: result, must sure result length > 16
void Md5Hash(const unsigned char *key, unsigned int length, unsigned char *result) {
    MD5(key, length, result);
}
void Md5Hash(std::string_view s, char *output) {
    unsigned char md5_hash[MD5_DIGEST_LENGTH];
    alpheratz::hash::Md5Hash(reinterpret_cast<const unsigned char *>(s.data()), s.length(),
                             md5_hash);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", md5_hash[i]);
    }
}
void Md5Hash(std::string_view s, std::string &output) {
    output.resize(32);
    alpheratz::hash::Md5Hash(s, output.data());
}

}  // namespace hash
}  // namespace alpheratz
