#ifndef HWSHQTB__INI__BASIC_TYPE_HPP
#define HWSHQTB__INI__BASIC_TYPE_HPP

#include "global.hpp"
#include <limits>

namespace hwshqtb {
    namespace ini {
        namespace impl {
            template <typename T>
            HWSHQTB__INLINE parse_status parse_fixed_size_integer(std::string_view sv, T& v, std::uint8_t size) {
                v = 0;
                while (size--) {
                    if (sv.empty()) return {sv, false};
                    char c = sv.front();
                    sv.remove_prefix(1);
                    if (is_space(c)) break;
                    T digit = is_xdigit(c);
                    if (digit == -1 || digit >= 15)
                        return {sv, false};
                    v = v * 10 + digit;
                }
                return {sv, true};
            }

            HWSHQTB__INLINE std::string ucs2_to_utf8(std::uint64_t codepoint) {
                std::string result;
                if (codepoint <= 0x7F)
                    result += static_cast<char>(codepoint);
                else if (codepoint <= 0x7FF) {
                    result += static_cast<char>(0xC0 | (codepoint >> 6));
                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                else if (codepoint <= 0xFFFF) {
                    result += static_cast<char>(0xE0 | (codepoint >> 12));
                    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                return result;
            }

            HWSHQTB__INLINE std::string ucs4_to_utf8(std::uint64_t codepoint) {
                std::string result;
                if (codepoint <= 0x7F)
                    result += static_cast<char>(codepoint);
                else if (codepoint <= 0x7FF) {
                    result += static_cast<char>(0xC0 | (codepoint >> 6));
                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                else if (codepoint <= 0xFFFF) {
                    result += static_cast<char>(0xE0 | (codepoint >> 12));
                    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                else if (codepoint <= 0x10FFFF) {
                    result += static_cast<char>(0xF0 | (codepoint >> 18));
                    result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                return result;
            }
        }
    }
}

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, std::string& v) {
            v.clear();
            if (sv.empty()) return {sv, false};
            if (sv.front() == '\"' || sv.front() == '\'') {
                char quote = sv.front();
                sv.remove_prefix(1);
                while (sv.size()) {
                    char c = sv.front();
                    sv.remove_prefix(1);
                    if (c == '\\' && quote == '\"') {
                        if (sv.empty()) return {sv.data(), false};
                        c = sv.front();
                        sv.remove_prefix(1);
                        switch (c) {
                            case '0': v += '\0'; break;
                            case 'a': v += '\a'; break;
                            case 'b': v += '\b'; break;
                            case 't': v += '\t'; break;
                            case 'v': v += '\v'; break;
                            case 'n': v += '\n'; break;
                            case 'f': v += '\f'; break;
                            case 'r': v += '\r'; break;
                            case '\"': v += '"'; break;
                            case '\'': v += '\''; break;
                            case '\\': v += '\\'; break;
                            case 'u': {
                                if (sv.size() < 4) return {sv.data(), false};
                                std::uint64_t codepoint;
                                if (const auto& [nsv, succeed] = impl::parse_fixed_size_integer(sv, codepoint, 4); sv = nsv, !succeed)
                                    return {sv, false};
                                v += impl::ucs2_to_utf8(codepoint);
                                break;
                            }
                            case 'U': {
                                if (sv.size() < 8) return {sv.data(), false};
                                std::uint64_t codepoint;
                                if (const auto& [nsv, succeed] = impl::parse_fixed_size_integer(sv, codepoint, 8); sv = nsv, !succeed)
                                    return {sv, false};
                                v += impl::ucs4_to_utf8(codepoint);
                                break;
                            }
                            default: return {sv, false};
                        }
                    }
                    else if (c == quote)
                        return {sv, true};
                    else v += c;
                }
            }
            return {sv.data(), false};
        }

        template <>
        std::string join(const std::string& v, const join_format& fmt) {
            std::string result;
            result.reserve(v.size() + 2);
            result += fmt.quote;
            for (const char c : v) {
                if (c == '\0') result += "\\0";
                else if (c == '\a') result += "\\a";
                else if (c == '\b') result += "\\b";
                else if (c == '\t') result += "\\t";
                else if (c == '\v') result += "\\v";
                else if (c == '\n') result += "\\n";
                else if (c == '\f') result += "\\f";
                else if (c == '\r') result += "\\r";
                else if (c == '\"' || c == '\'' || c == '\\')
                    result += '\\', result += c;
                else
                    result += c;
            }
            result += fmt.quote;
            return result;
        }

        template <>
        parse_status parse(std::string_view sv, integer& v) {
            v = 0;
            if (sv.empty()) return {sv, false};
            if (const auto& [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), v, 10); ec == std::errc()) {
                sv.remove_prefix(ptr - sv.data());
                if (sv.substr(0, 1) == ".")
                    return {sv, false};
                return {sv, true};
            }
            return {sv, false};
        }

        template <>
        std::string join(const integer& v, const join_format& fmt) {
            return fmt.integer_formatter(v);
        }

        template <>
        parse_status parse(std::string_view sv, floating& v) {
            v = 0.0;
            if (sv.empty()) return {sv, false};
            if (const auto& [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), v); ec == std::errc()) {
                sv.remove_prefix(ptr - sv.data());
                return {sv, true};
            }
            return {sv, false};
        }

        template <>
        std::string join(const floating& v, const join_format& fmt) {
            return fmt.floating_formatter(v);
        }

        template <>
        parse_status parse(std::string_view sv, boolean& v) {
            v = false;
            if (sv.empty()) return {sv, false};
            if (sv.substr(0, 4) == "true") {
                v = true;
                sv.remove_prefix(4);
                return {sv, true};
            }
            else if (sv.substr(0, 5) == "false") {
                v = false;
                sv.remove_prefix(5);
                return {sv, true};
            }
            return {sv.data(), false};
        }

        template <>
        std::string join(const boolean& v, const join_format& fmt) {
            return v ? "true" : "false";
        }
    }
}

#endif
