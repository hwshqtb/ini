#ifndef HWSHQTB__INI__GLOBEL_HPP
#define HWSHQTB__INI__GLOBEL_HPP

#ifndef HWSHQTB__NOT_HEADER_ONLY
#define HWSHQTB__INLINE inline
#else
#define HWSHQTB__INLINE
#endif

#include <cstdint>
#include <type_traits>
#include <string>
#include <string_view>
#include <deque>
#include "ordered_map.hpp"
#include <unordered_map>
#include <functional>
#include <charconv>
#include <utility>
#include <limits>

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

        struct key {
            std::string name;
            bool is_quoted;

            key();

            key(const std::string& c);

            key(const char* str):
                key(std::string(str)) {}
        };
        
        struct key_hash {
            std::size_t operator()(const key& s)const noexcept {
                return std::hash<std::string>{}(s.name);
            }
        };

        struct key_equal_to {
            bool operator()(const key& lhs, const key& rhs) const {
                return lhs.name == rhs.name;
            }
        };

        class value;

        using array = std::deque<value>;

        using node = std::pair<value, comment>;

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
            char quote = '\"';
            bool space_around_eq = true;
            bool array_element_newline = true;
            mutable bool kv_inline = false;

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

        };
        HWSHQTB__INLINE join_format global_format;

        struct parse_status {
            std::string_view nsv;
            bool succeed;
        };

        template <typename T>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, T& v) {
            if constexpr (std::is_constructible_v<T, integer>) {
                integer r;
                if (auto ps = parse(sv, r); ps.succeed) {
                    if (std::numeric_limits<T>::lowest() <= r && r <= std::numeric_limits<T>::max()) {
                        v = static_cast<T>(r);
                        return {ps.nsv, true};
                    }
                }
                else
                    return {ps.nsv, false};
            }
            else if constexpr (std::is_constructible_v<T, floating>) {
                floating r;
                if (auto ps = parse(sv, r); ps.succeed) {
                    if (std::numeric_limits<T>::lowest() <= r && r <= std::numeric_limits<T>::max()) {
                        v = static_cast<T>(r);
                        return {ps.nsv, true};
                    }
                }
                else
                    return {ps.nsv, false};
            }
            else if constexpr (std::is_constructible_v<T, boolean>) {
                boolean r;
                if (auto ps = parse(sv, r); ps.succeed) {
                    v = static_cast<T>(r);
                    return {ps.nsv, true};
                }
                else
                    return {ps.nsv, false};
            }
            else
                static_assert(!std::is_same_v<T, T>, "T must be a valid ini type");
            return {sv, false};
        }

        template <typename T>
        HWSHQTB__INLINE std::string join(const T& v, const join_format& fmt = global_format) {
            if constexpr (std::is_convertible_v<T, integer>)
                return fmt.integer_formatter(static_cast<integer>(v));
            else if constexpr (std::is_convertible_v<T, floating>)
                return fmt.floating_formatter(static_cast<floating>(v));
            else if constexpr (std::is_convertible_v<T, boolean>)
                return v ? "true" : "false";
            else
                static_assert(!std::is_same_v<T, T>, "T must be a valid ini type");
            return "";
        }

        HWSHQTB__INLINE parse_status parse(std::string_view sv, std::string& v); // both key and string
        HWSHQTB__INLINE parse_status parse(std::string_view sv, integer& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, floating& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, boolean& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, comment_upper_part& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, comment_lower_part& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, value& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, array& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key_value_pair& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key_section_pair& v);
        HWSHQTB__INLINE parse_status parse(std::string_view sv, table& v);

        HWSHQTB__INLINE std::string join(const std::string& v, const join_format& fmt = global_format); // both key and string
        HWSHQTB__INLINE std::string join(const integer& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const floating& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const boolean& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const comment_upper_part& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const comment_lower_part& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const value& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const key& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const array& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const key_value_pair& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const key_section_pair& v, const join_format& fmt = global_format);
        HWSHQTB__INLINE std::string join(const table& v, const join_format& fmt = global_format);

        HWSHQTB__INLINE bool is_space(char c) {
            return c == 0x09 || c == 0x20;
        }

        HWSHQTB__INLINE bool is_blank(char c) {
            return c == 0x09 || c == 0x20 || c == 0x0A || c == 0x0D;
        }

        HWSHQTB__INLINE std::int8_t is_xdigit(char c) {
            if (c >= 0x30 && c <= 0x39) return c - 0x30;
            if (c >= 0x41 && c <= 0x46) return c - 0x31;
            if (c >= 0x61 && c <= 0x66) return c - 0x51;
            return -1;
        }

        HWSHQTB__INLINE void remove_space(std::string_view& sv) {
            while (!sv.empty() && is_space(sv.front()))
                sv.remove_prefix(1);
        }

        HWSHQTB__INLINE void remove_space_back(std::string& s) {
            while (!s.empty() && is_space(s.back()))
                s.pop_back();
        }

        HWSHQTB__INLINE void remove_blank(std::string_view& sv) {
            while (!sv.empty() && is_blank(sv.front()))
                sv.remove_prefix(1);
        }

    }
}

#endif
