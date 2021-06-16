#include "proxy/helper.hpp"

#include <regex>
#include <sstream>

namespace helper
{

std::pair<std::string, std::string> cut(const std::string& s, const char delimiter, int beg, int end)
{
    // buffer overflow if { beg, end } are not in [0; s.size()]
    end = (end == -1) ? s.size() : end;
    for (int i = beg; i < end; ++i) {
        if (s[i] == delimiter) {
            return std::make_pair(s.substr(0, i), s.substr(i, end));
        }
    }

    return std::make_pair("", "");
}

std::vector<std::string> extract_uri_from_manifest(const std::string& content)
{
    static const std::regex attribute_uri_pattern(R"(^#.*uri=["'](.*)["'])", std::regex_constants::icase);

    std::vector<std::string> uris;

    std::smatch sm;
    std::istringstream iss(content);
    for (std::string line; std::getline(iss, line); ) {
        trim(line);
        if (line.empty())
            continue;

        if (line[0] == '#') {
            if (std::regex_search(line, sm, attribute_uri_pattern)) {
                uris.emplace_back(sm[1]);
            }
        } else {
            uris.emplace_back(line);
        }
    }

    return uris;
}

} // namespace helper