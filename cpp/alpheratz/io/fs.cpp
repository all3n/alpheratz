#include <alpheratz/io/fs.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

namespace alpheratz {
namespace io {
namespace fs {

// get home directory only init once and save in static string
std::string GetHomeDirectory() {
    static std::string home_dir;
    static bool initialized = false;
    if (!initialized) {
        struct passwd* pw = getpwuid(getuid());
        home_dir = std::string(pw->pw_dir);
        initialized = true;
    }
    return home_dir;
}

/**
 * expand home directory ~
 */
void ParsePath(std::string& path) {
    if (path[0] == '~') {
        std::string home_dir = GetHomeDirectory();
        // insert home_dir replace ~
        path.replace(0, 1, home_dir);
    }
}

}  // namespace fs
}  // namespace io
}  // namespace alpheratz
