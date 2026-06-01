#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>

// N is the maximum size of the string, not including the '\0' character
template <size_t N>
struct String {
    char data[N+1];
    String() {
        data[0] = '\0';
    }
    String(const char* str) {
        size_t i = 0;
        while (i < N && str[i] != '\0') {
            data[i] = str[i];
            i++;
        }
        data[i] = '\0';
    }
    String(const std::string& other) {
        assert(other.size() <= N);
        for (size_t i = 0; i < other.size(); ++i) {
            data[i] = other[i];
        }
        data[other.size()] = '\0';
    }
    std::string str() const {
        return std::string(data);
    }
    bool operator == (const String& other) const {
        for (size_t i = 0; i < N; i++) {
            if (data[i] != other.data[i]) {
                return false;
            }
            if (data[i] == '\0') {
                break;
            }
        }
        return true;
    }
    bool operator < (const String& other) const {
        return std::strcmp(data, other.data) < 0;
    }
};

template <size_t N>
std::ostream& operator << (std::ostream& os, const String<N>& str) {
    os << str.data;
    return os;
}

#endif