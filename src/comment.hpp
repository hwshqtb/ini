#ifndef HWSHQTB__INI__COMMENT_HPP
#define HWSHQTB__INI__COMMENT_HPP

#include "global.hpp"

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, comment_upper_part& v) {
            v.contents.clear();
            if (sv.empty())
                return {sv, true};
            remove_blank(sv);
            if (sv.substr(0, 2) == "#!")
                return {sv, false};
            while (true) {
                if (sv.substr(0, 1) != "#")
                    break;
                sv.remove_prefix(1);
                if (sv.substr(0, 1) == "!")
                    sv.remove_prefix(1);
                std::size_t end = sv.find_first_of('\n', 0);
                v.contents.emplace_back(sv.substr(0, end));
                if (end == std::string_view::npos)
                    sv.remove_prefix(sv.size());
                else sv.remove_prefix(end + 1);
                remove_blank(sv);
            }
            return {sv, true};
        }

        template <>
        std::string join(const comment_upper_part& v, const join_format& fmt) {
            std::string result;
            for (const auto& line : v.contents)
                result += "#" + line + "\n";
            return result;
        }

        template <>
        parse_status parse(std::string_view sv, comment_lower_part& v) {
            v.contents.clear();
            v.same_line = false;
            if (sv.empty())
                return {sv, true};
            remove_space(sv);
            if (sv.substr(0, 2) != "#!") {
                if (sv.substr(0, 1) == "#") {
                    sv.remove_prefix(1);
                    v.same_line = true;
                    std::size_t end = sv.find_first_of('\n', 0);
                    v.contents.emplace_back(sv.substr(0, end));
                    if (end == std::string_view::npos)
                        sv.remove_prefix(sv.size());
                    else sv.remove_prefix(end + 1);
                }
                remove_blank(sv);
            }
            while (sv.substr(0, 2) == "#!") {
                sv.remove_prefix(2);
                std::size_t end = sv.find_first_of('\n', 0);
                v.contents.emplace_back(sv.substr(0, end));
                if (end == std::string_view::npos)
                    sv.remove_prefix(sv.size());
                else sv.remove_prefix(end + 1);
                remove_blank(sv);
            }
            return {sv, true};
        }

        template <>
        std::string join(const comment_lower_part& v, const join_format& fmt) {
            if (v.contents.empty())
                return "";
            std::string result;
            std::size_t index = 0;
            if (v.same_line) {
                result = " #" + v.contents.front() + "\n";
                index = 1;
            }
            while (index < v.contents.size()) {
                result += "#!" + v.contents[index++] + "\n";
            }
            if (v.contents.size())
                result.pop_back();
            return result;
        }
    }
}

#endif