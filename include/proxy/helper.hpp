#pragma once

// #include <tuple>
#include <utility>
#include <string>

namespace helper
{
    inline std::pair<std::string, std::string> cut(std::string s, const char delimiter, const int beg = 0, int end = -1)
    {
        end = (end == -1) ? s.size() : end;
        for (int i = beg; i < end; ++i) {
            if (s[i] == delimiter) {
                return std::make_pair(s.substr(0, i), s.substr(i, end));
            }
        }

        return std::make_pair("", "");
    }

    inline std::string get_extension(const std::string& s)
    {
        return s.substr(s.find_last_of('.'));
    }
}