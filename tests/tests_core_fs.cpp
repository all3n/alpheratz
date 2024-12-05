#include <gtest/gtest.h>
#include <alpheratz/io/fs.h>

#include <iostream>
using namespace alpheratz::io::fs;
TEST(TestCoreCommonFs, TestHomeDirectory) {
    std::string home_dir = GetHomeDirectory();
    std::cout << home_dir << std::endl;
}
TEST(TestCoreCommonFs, TestParsePath) {
    std::string path = "~/xxx/test.path";
    ParsePath(path);
    std::string expected_path = GetHomeDirectory() + "/xxx/test.path";
    ASSERT_EQ(expected_path, path);
    std::cout << path << std::endl;
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
