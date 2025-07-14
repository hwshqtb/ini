#ifndef HWSHQTB__INI__GLOBEL_HPP
#define HWSHQTB__INI__GLOBEL_HPP

#include <cstdint>
#include <type_traits>
#include <string>
#include <string_view>
#include <deque>
#include "ordered_map.hpp"
#include <unordered_map>
#include <functional>
#include <charconv>

namespace hwshqtb {
    namespace ini {
        using string = std::string;

        using integer = std::int64_t;

        using floating = double;

        using boolean = bool;

        struct comment_upper_part {
            std::deque<std::string> contents;
        };

        struct comment_lower_part {
            std::deque<std::string> contents;
            bool same_line = true;
        };

        struct comment {
            comment_upper_part upper;
            comment_lower_part lower;
        };

        using key = std::pair<std::string, bool>; 
        
        struct key_hash {
            std::size_t operator()(const key& s)const noexcept {
                return std::hash<std::string>{}(s.first);
            }
        };

        struct key_equal_to {
            bool operator()(const key& lhs, const key& rhs) const {
                return lhs.first == rhs.first;
            }
        };

        class value;

        using array = std::deque<value*>;

        using node = std::pair<value*, comment>;

        using key_value_pair = std::pair<key, node>;

        struct section {
            container::ordered_map<key, node,
                std::list<key_value_pair>,
                std::unordered_map<key, typename std::list<key_value_pair>::iterator, key_hash, key_equal_to>> kvs;
            comment c;
        };

        using key_section_pair = std::pair<key, section>;

        struct table {
            container::ordered_map<key, section,
                std::list<key_section_pair>,
                std::unordered_map<key, typename std::list<key_section_pair>::iterator, key_hash, key_equal_to>> sections;
            comment c;
        };

        struct join_format {
            char quote = '\'';
            bool space_around_eq = true;

            std::function<std::string(integer)> integer_formatter = [](integer v) -> std::string {
                std::string result(100, '\0');
                const auto& [ptr, ec] = std::to_chars(result.data(), result.data() + result.size(), v, 10);
                if (ec == std::errc()) {
                    result.resize(ptr - result.data());
                    return result;
                }
                return "";
            };

            std::function<std::string(floating)> floating_formatter = [](floating v) -> std::string {
                std::string result(400, '\0');
                const auto& [ptr, ec] = std::to_chars(result.data(), result.data() + result.size(), v);
                if (ec == std::errc()) {
                    result.resize(ptr - result.data());
                    if (result.find_first_of('.') == std::string::npos)
                        result += ".0";
                    return result;
                }
                return "";
            };

        } global_format;

        struct parse_status {
            std::string_view nsv;
            bool succeed;
        };

        template <typename T>
        parse_status parse(std::string_view sv, T& v) {
            static_assert(true, "T must be a valid ini type");
            return {sv, false};
        }

        template <typename T>
        std::string join(const T& v, const join_format& fmt = global_format) {
            static_assert(true, "T must be a valid ini type");
            return "";
        }

        template <>
        parse_status parse(std::string_view sv, std::string& v); // both key and string
        template <>
        parse_status parse(std::string_view sv, integer& v);
        template <>
        parse_status parse(std::string_view sv, floating& v);
        template <>
        parse_status parse(std::string_view sv, boolean& v);
        template <>
        parse_status parse(std::string_view sv, comment_upper_part& v);
        template <>
        parse_status parse(std::string_view sv, comment_lower_part& v);
        template <>
        parse_status parse(std::string_view sv, value& v);
        template <>
        parse_status parse(std::string_view sv, key& v);
        template <>
        parse_status parse(std::string_view sv, array& v);
        template <>
        parse_status parse(std::string_view sv, key_value_pair& v);
        template <>
        parse_status parse(std::string_view sv, key_section_pair& v);
        template <>
        parse_status parse(std::string_view sv, table& v);

        template <>
        std::string join(const std::string& v, const join_format& fmt); // both key and string
        template <>
        std::string join(const integer& v, const join_format& fmt);
        template <>
        std::string join(const floating& v, const join_format& fmt);
        template <>
        std::string join(const boolean& v, const join_format& fmt);
        template <>
        std::string join(const comment_upper_part& v, const join_format& fmt);
        template <>
        std::string join(const comment_lower_part& v, const join_format& fmt);
        template <>
        std::string join(const value& v, const join_format& fmt);
        template <>
        std::string join(const key& v, const join_format& fmt);
        template <>
        std::string join(const array& v, const join_format& fmt);
        template <>
        std::string join(const key_value_pair& v, const join_format& fmt);
        template <>
        std::string join(const key_section_pair& v, const join_format& fmt);
        template <>
        std::string join(const table& v, const join_format& fmt);

        bool is_space(char c) {
            return c == 0x09 || c == 0x20;
        }

        bool is_blank(char c) {
            return c == 0x09 || c == 0x20 || c == 0x0A || c == 0x0D;
        }

        std::int8_t is_xdigit(char c) {
            if (c >= 0x30 && c <= 0x39) return c - 0x30;
            if (c >= 0x41 && c <= 0x46) return c - 0x31;
            if (c >= 0x61 && c <= 0x66) return c - 0x51;
            return -1;
        }

        void remove_space(std::string_view& sv) {
            while (!sv.empty() && is_space(sv.front()))
                sv.remove_prefix(1);
        }

        void remove_space_back(std::string& s) {
            while (!s.empty() && is_space(s.back()))
                s.pop_back();
        }

        void remove_blank(std::string_view& sv) {
            while (!sv.empty() && is_blank(sv.front()))
                sv.remove_prefix(1);
        }

    }
}

#endif
