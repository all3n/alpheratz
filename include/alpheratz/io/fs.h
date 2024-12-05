#pragma once
#include <string>
#include <vector>
namespace alpheratz {
namespace io {
namespace fs {

std::string GetHomeDirectory();
void ParsePath(std::string& path);
}  // namespace fs
}  // namespace io
}  // namespace alpheratz
