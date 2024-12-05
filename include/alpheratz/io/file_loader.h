#pragma once
#include <absl/strings/string_view.h>

#include <functional>

namespace alpheratz {
namespace io {
// load file and line process callback
void LoadFile(absl::string_view filename,
              std::function<void(const std::string &, uint32_t)> &callback,
              absl::string_view exclude);

}  // namespace io
}  // namespace alpheratz
