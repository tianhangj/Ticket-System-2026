#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "string"
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

#endif