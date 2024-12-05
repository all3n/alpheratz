#pragma once
#include <sys/stat.h>
#include <unistd.h>

#include <string>

namespace alpheratz {
namespace io {
namespace file {
inline bool Exists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

}  // namespace file
}  // namespace io
}  // namespace alpheratz
