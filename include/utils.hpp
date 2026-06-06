#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "string"
#include <iostream>
#include "string.hpp"
#include "STLite/vector.hpp"

// string tools
inline sjtu::vector<std::string> split(const std::string& str, char c) {
    sjtu::vector<std::string> result;
    size_t i = 0;
    while (i < str.size()) {
        result.push_back("");
        while (i < str.size() && str[i] != c) {
            result.back() += str[i];
            i++;
        }
        i++;
    }
    return result;
}

// time tools
// hh:mm -> timestamp
inline int daytime(std::string time) {
    int h, m;
    sscanf(time.c_str(), "%d:%d", &h, &m);
    return h * 60 + m;
}

inline int day(std::string date) {
    int month, day;
    sscanf(date.c_str(), "%d-%d", &month, &day);
    if (month == 6) {
        return day;
    } else if (month == 7) {
        return 30 + day;
    } else if (month == 8) {
        return 30 + 31 + day;
    } else if (month == 9) {
        return 30 + 31 + 31 + day;
    } else {
        return 100000;
    }
}

// timestamp -> xx-xx xx:xx
inline String<20> timestr(int time) {
    int day = time / (24 * 60);
    int hour = (time % (24 * 60)) / 60;
    int minute = time % 60;
    int month = 6;
    if (day > 30 + 31 + 31) {
        month = 9;
        day -= 30 + 31 + 31;
    } else if (day > 30 + 31) {
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