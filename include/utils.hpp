#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "string"
#include "string.hpp"
#include "STLite/vector.hpp"

// string tools
sjtu::vector<std::string> split(const std::string& str, char c) {
    sjtu::vector<std::string> result;
    size_t start = 0, end = 0;
    while (end < str.size()) {
        while (end < str.size() && str[end] != c) {
            end++;
        }
        if (start < end) {
            result.push_back(str.substr(start, end - start));
        }
        end++;
        start = end;
    }
    return result;
}

// time tools
// hh:mm -> timestamp
int daytime(std::string time) {
    int h, m;
    sscanf(time.c_str(), "%d:%d", &h, &m);
    return h * 60 + m;
}

int day(std::string date) {
    int month, day;
    sscanf(date.c_str(), "%d-%d", &month, &day);
    if (month == 6) {
        return day;
    } else if (month == 7) {
        return 30 + day;
    } else if (month == 8) {
        return 30 + 31 + day;
    } else {
        assert(false);
    }
}

// timestamp -> xx-xx xx:xx
String<20> timestr(int time) {
    int day = time / (24 * 60);
    int hour = (time % (24 * 60)) / 60;
    int minute = time % 60;
    int month = 6;
    if (day > 30 + 31) {
        month = 8;
        day -= 30 + 31;
    } else if (day > 30) {
        month = 7;
        day -= 30;
    }
    String<20> result;
    sprintf(result.data, "%02d-%02d %02d:%02d", month, day, hour, minute);
    return result;
}
#endif