#include <alpheratz/common/locale.h>
#include <iconv.h>

#include <iostream>
#include <ostream>
#include <string>

namespace alpheratz {
namespace common {
namespace locale {

// gbk 2 bytes
// utf-8 1-6 bytes, utf-8
// utf-8 max 3 times outbuf of gbk size
int ConvertGbk2Utf8(const std::string& gbk_content, std::string& out) {
    iconv_t cd = iconv_open("UTF-8", "GBK");
    if (cd == (iconv_t)-1) {
        std::cerr << "Error opening iconv!" << std::endl;
        return -1;
    }
    size_t in_size = gbk_content.size();
    size_t utf8_len = in_size * 4;
    size_t out_size = utf8_len;
    out.resize(utf8_len);
    char* inbuf = const_cast<char*>(gbk_content.c_str());
    char* outbuf = out.data();

    size_t ret = iconv(cd, &inbuf, &in_size, &outbuf, &out_size);
    if (ret == static_cast<size_t>(-1)) {
        std::cerr << "Error converting encoding!" << std::endl;
        return -1;
    }
    out.resize(utf8_len - out_size);
    iconv_close(cd);
    return 0;
}

}  // namespace locale
}  // namespace common
}  // namespace alpheratz
