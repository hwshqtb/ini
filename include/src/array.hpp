#ifndef HWSHQTB__INI__ARRAY_HPP
#define HWSHQTB__INI__ARRAY_HPP

#include "global.hpp"
#include "value.hpp"

namespace hwshqtb {
    namespace ini {
        parse_status parse(std::string_view sv, array& v) {
            v.clear();
            if (sv.substr(0, 1) != "[") return {sv, false};
            sv.remove_prefix(1);
            remove_blank(sv);
            while (sv.size()) {
                if (sv.substr(0, 1) == "]") {
                    sv.remove_prefix(1);
                    return {sv, true};
                }
                value r;
                if (const auto& [nsv, succeed] = parse(sv, r); sv = nsv, !succeed) return {sv, false};
                v.push_back(r);
                remove_blank(sv);
                if (sv.substr(0, 1) == ",") {
                    sv.remove_prefix(1);
                    remove_blank(sv);
                }
                else if (sv.substr(0, 1) == "]") {
                    sv.remove_prefix(1);
                    return {sv, true};
                }
                else
                    return {sv, false};
            }
            return {sv, false};
        }

        std::string join(const array& v, const join_format& fmt) {
            std::string result = "[";
            result += fmt.array_element_newline ? "\n" : " ";
            bool old = std::exchange(fmt.kv_inline, true);
            for (const auto& r : v) {
                if (fmt.array_element_newline) {
                    if (fmt.indent != static_cast<std::size_t>(-1))
                        for (std::size_t i = 0; i < fmt.indent; ++i)
                            result += fmt.indent_string;
                }
                if (fmt.indent != static_cast<std::size_t>(-1))
                    ++fmt.indent;
                result += join(r, fmt) + ",";
                if (fmt.indent != static_cast<std::size_t>(-1))
                    --fmt.indent;
                result += fmt.array_element_newline ? "\n" : " ";
            }
            fmt.kv_inline = old;
            if (v.size()) {
                result.pop_back();
                result.pop_back();
            }
            if (fmt.array_element_newline) {
                if (v.size())
                    result += "\n";
                if (fmt.indent != static_cast<std::size_t>(-1))
                    for (std::size_t i = 0; i < fmt.indent - 1; ++i)
                        result += fmt.indent_string;
            }
            else
                result += " ";
            result += "]";
            return result;
        }
    }
}

#endif
