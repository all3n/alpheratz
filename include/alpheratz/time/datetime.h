#pragma once
#include <ctime>
#include <string>
namespace alpheratz {
namespace time {

#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define DATETIME_FORMAT2 "%Y%m%d%H%M%S"
#define DATE_FORMAT "%Y-%m-%d"

class DateTime {
   public:
    DateTime() {
        time_t t = std::time(nullptr);
        struct tm *now = localtime(&t);
        year_ = now->tm_year + 1900;
        month_ = now->tm_mon + 1;
        day_ = now->tm_mday;
        hour_ = now->tm_hour;
        minute_ = now->tm_min;
        second_ = now->tm_sec;
    }

    DateTime(int year, int month, int day, int hour, int minute, int second)
        : year_(year), month_(month), day_(day), hour_(hour), minute_(minute), second_(second) {}

    int GetYear() const { return year_; }

    int GetMonth() const { return month_; }

    int GetDay() const { return day_; }

    int GetHour() const { return hour_; }

    int GetMinute() const { return minute_; }

    int GetSecond() const { return second_; }

    time_t GetTimestamp() const;

    void SetYear(int year) { this->year_ = year; }

    void SetMonth(int month) { this->month_ = month; }

    void SetDay(int day) { this->day_ = day; }

    void SetHour(int hour) { this->hour_ = hour; }

    void SetMinute(int minute) { this->minute_ = minute; }

    void SetSecond(int second) { this->second_ = second; }

    std::string ToString(const std::string &format = DATETIME_FORMAT) const;

    static DateTime Parse(const std::string &str, const std::string &format = DATETIME_FORMAT);

   private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    int second_;
};

}  // namespace time
}  // namespace alpheratz
