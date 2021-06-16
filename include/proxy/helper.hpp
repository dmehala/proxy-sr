#pragma once

#include <utility>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace helper
{
    const char* const g_ws = " \n\r\f\v";

    std::pair<std::string, std::string> cut(const std::string& s, const char delimiter, int beg = 0, int end = -1);

    std::vector<std::string> extract_uri_from_manifest(const std::string& content);

    inline std::string get_extension(const std::string& s)
    {
        auto dot_pos = s.find_last_of('.');
        if (dot_pos == std::string::npos)
            return "";

        return s.substr(dot_pos);
    }

    // In-place trimming (be careful)
    inline void rtrim(std::string& s, const char* const ws = g_ws)
    {
        s.erase(s.find_last_not_of(ws) + 1);
    }

    inline void ltrim(std::string& s, const char* const ws = g_ws)
    {
        s.erase(0, s.find_first_not_of(ws));
    }

    inline void trim(std::string& s, const char* const ws = g_ws)
    {
        ltrim(s, ws);
        rtrim(s, ws);
    }


    inline std::string resolve_uri(const fs::path& manifest_uri, const std::string& uri)
    {
        fs::path base(manifest_uri);
        base.remove_filename();
        return (base / uri).lexically_normal().string();
    }

    inline bool contains(const std::string& s1, const std::string& s2)
    {
        return s1.find(s2) != std::string::npos;
    }
}