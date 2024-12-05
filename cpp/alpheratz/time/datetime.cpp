#include <alpheratz/time/datetime.h>

#include <cstring>
#include <sstream>
namespace alpheratz {
namespace time {

std::string DateTime::ToString(const std::string &format) const {
    std::ostringstream oss;
    struct tm t;
    t.tm_year = year_ - 1900;
    t.tm_mon = month_ - 1;
    t.tm_mday = day_;
    t.tm_hour = hour_;
    t.tm_min = minute_;
    t.tm_sec = second_;
    char buf[100];
    if (strftime(buf, sizeof(buf), format.c_str(), &t) == 0) {
        return "";
    }
    return buf;
}

DateTime DateTime::Parse(const std::string &str, const std::string &format) {
    DateTime dt;
    struct tm t = {};
    std::memset(&t, 0, sizeof(t));
    if (strptime(str.c_str(), format.c_str(), &t) == nullptr) {
        return dt;
    }
    dt.year_ = t.tm_year + 1900;
    dt.month_ = t.tm_mon + 1;
    dt.day_ = t.tm_mday;
    dt.hour_ = t.tm_hour;
    dt.minute_ = t.tm_min;
    dt.second_ = t.tm_sec;
    return dt;
}
time_t DateTime::GetTimestamp() const {
    struct tm t;
    t.tm_year = year_ - 1900;
    t.tm_mon = month_ - 1;
    t.tm_mday = day_;
    t.tm_hour = hour_;
    t.tm_min = minute_;
    t.tm_sec = second_;
    t.tm_isdst = -1;
    return mktime(&t);
}
}  // namespace time
}  // namespace alpheratz
