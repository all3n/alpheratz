#include <gtest/gtest.h>
#include <alpheratz/common/locale.h>

#include <array>
#include <iostream>

// use python gen tests example
// >>> list("你好".encode('gbk'))
// [196, 227, 186, 195]
// >>> list("你好".encode('utf-8'))
// [228, 189, 160, 229, 165, 189]

TEST(TestCoreLocale, TestGbkToUtf8) {
    std::vector<uint8_t> gbk_bytes = {196, 227, 186, 195};
    std::vector<uint8_t> utf8_bytes = {228, 189, 160, 229, 165, 189};

    std::string gbk_str(gbk_bytes.begin(), gbk_bytes.end());
    std::string utf_expect_str(utf8_bytes.begin(), utf8_bytes.end());

    std::string out_utf8;
    alpheratz::common::locale::ConvertGbk2Utf8(gbk_str, out_utf8);
    std::cout << "gbk:" << gbk_str << std::endl;
    std::cout << "out_utf8:" << out_utf8 << std::endl;
    std::cout << "utf_expect_str:" << utf_expect_str << std::endl;
    ASSERT_EQ(out_utf8, utf_expect_str);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
