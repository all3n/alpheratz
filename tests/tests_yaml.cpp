#include <alpheratz/conf/yaml_conf.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

TEST(TestYaml, TestYamlLoad) {
    fs::path test_data_base("tests/test_data");
    std::cout << fs::absolute(test_data_base) << std::endl;
    if (fs::exists(test_data_base)) {
        std::cout << fs::absolute(test_data_base) << " exists" << std::endl;
    }
    fs::path config_yaml = test_data_base / "config.yaml";
    YAML::Node config = YAML::LoadFile(config_yaml.string());
    const auto username = config["username"].as<std::string>();
    const auto password = config["password"].as<std::string>();
    std::cout << config["username"].as<std::string>() << std::endl;
    std::cout << config["password"].as<std::string>() << std::endl;
}

TEST(TestYaml, TestConfLoad) {
    alpheratz::conf::YamlConfig &config = alpheratz::conf::YamlConfig::Get();
    ASSERT_EQ(config.IsInitialized(), false);
    fs::path test_data_base("tests/test_data");
    fs::path config_yaml = fs::absolute(test_data_base / "config.yaml");

    auto status = config.Load(config_yaml.string());
    LOG(INFO) << " test load:" << status;

    // const absl::optional<uint32_t> &af = config.operator[]<uint32_t>("adf");
    auto af = config["username"].as<uint32_t>(0);
    LOG(INFO) << af;
    auto af2 = config["test2"].as<uint32_t>(123);
    LOG(INFO) << af2;

    absl::optional<std::string> sub_value = config.GetOptional<std::string>("group", "abc");
    LOG(INFO) << sub_value.value_or("");

    ASSERT_EQ(status.ok(), true);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
