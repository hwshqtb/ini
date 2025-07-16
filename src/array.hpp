#ifndef HWSHQTB__INI__ARRAY_HPP
#define HWSHQTB__INI__ARRAY_HPP

#include "global.hpp"
#include "value.hpp"

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, array& v) {
            v.clear();
            if (sv.substr(0, 1) != "[") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            while (sv.size()) {
                value r;
                if (const auto& [nsv, succeed] = parse(sv, r); sv = nsv, !succeed)
                    return {sv, false};
                v.push_back(r);
                remove_space(sv);
                if (sv.substr(0, 1) == ",") {
                    sv.remove_prefix(1);
                    remove_space(sv);
                }
                else if (sv.substr(0, 1) == "]") {
                    sv.remove_prefix(1);
                    return {sv, true};
                }
                else return {sv, false};
            }
            return {sv, false};
        }

        template <>
        std::string join(const array& v, const join_format& fmt) {
            std::string result = "[ ";
            for (const auto& r : v)
                result += join(r, fmt) + ", ";
            if (v.size()) {
                result.pop_back();
                result.pop_back();
                result.push_back(' ');
            }
            result += "]";
            return result;
        }
    }
}

#endif