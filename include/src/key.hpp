#ifndef HWSHQTB__INI__KEY_HPP
#define HWSHQTB__INI__KEY_HPP

#include "global.hpp"

namespace hwshqtb {
    namespace ini {
        HWSHQTB__INLINE key::key():
            name(""), is_quoted(false) {}

        HWSHQTB__INLINE key::key(const std::string& c) {
            parse((std::string_view)c, *this);
        }
        
        parse_status parse(std::string_view sv, key& v) {
            v = key{};
            if (sv.empty()) return {sv, false};
            if (sv.substr(0, 1) == "\"" || sv.substr(0, 1) == "\'") {
                const auto& r = parse(sv, v.name);
                if (r.succeed)
                    v.is_quoted = true;
                return r;
            }
            std::size_t end = sv.find_first_not_of("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-");
            if (end == 0)
                return {sv, false};
            v.name.assign(sv.substr(0, end));
            if (end == std::string_view::npos)
                sv.remove_prefix(sv.size());
            else sv.remove_prefix(end);
            return {sv, true};
        }

        std::string join(const key& v, const join_format& fmt) {
            if (v.is_quoted)
                return join(v.name, fmt);
            return v.name;
        }
    }
}

#endif