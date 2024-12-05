#pragma once
#include <absl/base/options.h>
#include <absl/status/status.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>
#include <alpheratz/common/macro.h>
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>

namespace alpheratz {
namespace conf {
class YamlConfig {
   public:
    ALPHERATZ_DISALLOW_COPY_AND_ASSIGN(YamlConfig);

    static YamlConfig& Get() {
        static YamlConfig config_instance;
        return config_instance;
    }
    absl::Status Load(absl::string_view filename);
    bool IsInitialized() const { return initialized_; }

    YAML::Node operator[](const std::string& key) const { return root_[key]; }

    template <typename T>
    const absl::optional<T>& operator[](const std::string& key) const {
        if (!root_[key]) {
            return absl::nullopt;
        }
        return root_[key].as<T>();
    }

    template <typename T>
    T Get(const char* key, const char* sub_key) {
        auto key_group = root_[key];
        if (key_group) {
            return key_group[sub_key].as<T>();
        }
        return T();
    }
    template <typename T>
    absl::optional<T> GetOptional(const char* key, const char* sub_key) {
        auto key_group = root_[key];
        if (key_group) {
            if (key_group[sub_key]) {
                return key_group[sub_key].as<T>();
            }
        }
        return absl::nullopt;
    }

   private:
    YamlConfig() {}
    YAML::Node root_;
    bool initialized_{false};
};

}  // namespace conf
}  // namespace alpheratz
