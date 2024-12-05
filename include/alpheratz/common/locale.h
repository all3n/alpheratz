#pragma once
#include <string>

namespace alpheratz {
namespace common {
namespace locale {
int ConvertGbk2Utf8(const std::string& gbk_content, std::string& out);

}  // namespace locale
}  // namespace common
}  // namespace alpheratz
