#include <alpheratz/string/string_utils.h>

namespace alpheratz {
namespace string {

void Split(const std::string &line, const std::string &delimiter, std::vector<std::string> &out) {
    out.clear();
    size_t pos_start = 0;
    size_t pos_end;
    size_t delim_len = delimiter.length();
    std::string token;

    while ((pos_end = line.find(delimiter, pos_start)) != std::string::npos) {
        token = line.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        out.push_back(token);
    }
    out.push_back(line.substr(pos_start));
}

}  // namespace string
}  // namespace alpheratz
