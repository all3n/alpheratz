#include <absl/strings/match.h>
#include <alpheratz/io/file_loader.h>

#include <fstream>
namespace alpheratz {
namespace io {
// load file and line process callback
void LoadFile(absl::string_view filename,
              std::function<void(const std::string &, uint32_t)> &callback,
              absl::string_view exclude = "") {
    std::ifstream input_file(std::string(filename.data(), filename.length()));
    std::string line;
    uint32_t index = 0;
    while (!input_file.eof()) {
        std::getline(input_file, line);
        if (line.length() == 0) {
            continue;
        }
        if (!exclude.empty() && absl::StartsWith(line, exclude)) {
            continue;
        }
        callback(line, index);
        ++index;
    }
}

}  // namespace io
}  // namespace alpheratz
