#include <alpheratz/string/byte_string.h>

#include <iomanip>
#include <sstream>

namespace alpheratz {
namespace string {

int HexStringToBytes(const std::string &data, std::vector<uint8_t> &out) {
    if (data.size() % 2 != 0) {
        return -1;
    }
    out.resize(data.size() / 2);
    for (size_t i = 0; i < data.length(); i += 2) {
        unsigned int byte;
        std::istringstream(data.substr(i, 2)) >> std::hex >> byte;
        out[i >> 1] = static_cast<unsigned char>(byte);
    }
    return 0;
}

std::string BytesToHexString(std::vector<uint8_t> &data, absl::string_view delim) {
    std::ostringstream os;
    size_t len = data.size();
    for (size_t i = 0; i < len; ++i) {
        os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        if (i != len - 1 && !delim.empty()) {
            os << delim;
        }
    }
    return os.str();
}

}  // namespace string
}  // namespace alpheratz
