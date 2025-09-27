#ifndef HWSHQTB__INI__IO_HPP
#define HWSHQTB__INI__IO_HPP

#include "global.hpp"
#include <ostream>
#include <fstream>

namespace hwshqtb {
    namespace ini {
        using parse_result = std::optional<table>;

        HWSHQTB__INLINE parse_result parse(std::string_view sv) {
            table t;
            if (const auto& [_, succeed] = parse(sv, t); succeed)
                return std::make_optional(std::move(t));
            return std::nullopt;
        }

        HWSHQTB__INLINE parse_result parse(const std::string& s) {
            return parse((std::string_view)s);
        }

        HWSHQTB__INLINE parse_result parse_file(std::ifstream& file) {
            std::istreambuf_iterator<char> begin(file), end;
            std::string str(begin, end);
            return parse((std::string_view)str);
        }

        HWSHQTB__INLINE parse_result parse_file(const std::string& path) {
            std::ifstream file(path);
            if (!file.is_open())
                return std::nullopt;
            return parse_file(file);
        }

        HWSHQTB__INLINE std::ostream& operator<<(std::ostream& os, const table& v) {
            return os << join(v);
        }
    }
}

#endif