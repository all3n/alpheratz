#pragma once
// @author all3n
// this file used for get environment value from system
#include <string>
#include <string_view>
namespace alpheratz {
namespace common {
namespace env {
std::string_view GetEnv(const std::string &key, const std::string &def);

}  // namespace env
}  // namespace common
}  // namespace alpheratz
