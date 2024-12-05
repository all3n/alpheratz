#include <alpheratz/hash/base64.h>
#include <alpheratz/hash/md5.h>
#include <alpheratz/hash/murmurhash3.h>
#include <gtest/gtest.h>

#include <iostream>
TEST(TestMD5, TestMd5SUM) {
    std::string test = "admin";
    std::string out;
    alpheratz::hash::Md5Hash(test, out);
    std::cout << out << std::endl;
}
TEST(TestBase64, TestBase64Encode) {
    std::string out;
    alpheratz::hash::Base64Encode("admin", out);
    std::cout << out << std::endl;
}

TEST(TestBase64, TestBase64Decode) {
    std::string encode_str = "YWRtaW4=";
    std::string out;
    int decode_max_size = alpheratz::hash::DecodeNeedSize(encode_str.size());
    out.resize(decode_max_size);
    size_t out_len = alpheratz::hash::Base64Decode(
        encode_str, reinterpret_cast<uint8_t *>(out.data()), decode_max_size);
    std::cout << out << ":" << out_len << " max:" << decode_max_size << std::endl;
}
TEST(TestMurmurHash, TestMurmurHash) {
  // alpheratz::hash::MurmurHash64();
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
