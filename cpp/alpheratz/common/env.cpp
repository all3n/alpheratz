#include <alpheratz/common/env.h>

#include <cstdlib>
#include <string>
#include <string_view>

namespace alpheratz {
namespace common {
namespace env {

std::string_view GetEnv(const std::string &key, const std::string &def) {
    const char *var = getenv(key.c_str());
    if (var == nullptr) {
        return def;
    }
    return std::string_view(var);
}

}  // namespace env
}  // namespace common
}  // namespace alpheratz
