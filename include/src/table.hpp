#ifndef HWSHQTB__INI__TABLE_HPP
#define HWSHQTB__INI__TABLE_HPP

#include "global.hpp"
#include "value.hpp"
#include "key.hpp"
#include <iostream>

namespace hwshqtb {
    namespace ini {
        parse_status parse(std::string_view sv, key_value_pair& v) {
            auto& [k, r] = v;
            auto& [rv, rc] = r;
            rv.clear();
            if (const auto& [nsv, succeed] = parse(sv, rc.upper); sv = nsv, !succeed) return {sv, false};
            remove_space(sv);
            if (sv.empty()) return {sv, true};
            if (const auto& [nsv, succeed] = parse(sv, k); sv = nsv, !succeed) return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "=") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rv); sv = nsv, !succeed) return {sv, succeed};
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rc.lower); sv = nsv, !succeed) return {sv, false};
            return {sv, true};
        }

        std::string join(const key_value_pair& v, const join_format& fmt) {
            std::string lower = join(v.second.second.lower, fmt);
            if (fmt.indent != static_cast<std::size_t>(-1)) ++fmt.indent;
            std::string kv = join(v.second.second.upper, fmt) + join(v.first, fmt) + (fmt.space_around_eq ? " = " : "=") + join(v.second.first, fmt);
            if (fmt.comment_column != static_cast<std::size_t>(-1)) {
                if (lower.substr(0, 2) != "#!" && lower.substr(0, 1) == "#")
                    fmt.comment_column = kv.size() + 1;
                else
                    fmt.comment_column = -1;
            }
            if (fmt.indent != static_cast<std::size_t>(-1)) --fmt.indent;
            return kv + (lower.substr(0, 2) == "#!" ? "\n" : " ") + lower;
        }

        parse_status parse(std::string_view sv, key_section_pair& v) {
            auto& [k, r] = v;
            auto& [map, rc] = r;
            map.clear();
            if (const auto& [nsv, succeed] = parse(sv, rc.upper); sv = nsv, !succeed) return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "[") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, k); sv = nsv, !succeed) return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "]") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rc.lower); sv = nsv, !succeed) return {sv, false};
            remove_blank(sv);
            while (sv.size()) {
                if (sv.substr(0, 1) == "[") break;
                key_value_pair p;
                if (const auto& [nsv, succeed] = parse(sv, p); succeed) {
                    if (p.first.name == "" && !p.first.is_quoted) break;
                    sv = nsv;
                    map.insert(std::exchange(p, key_value_pair{}));
                    remove_blank(sv);
                }
                else
                    return {sv, false};
            }
            return {sv, true};
        }

        std::string join(const key_section_pair& v, const join_format& fmt) {
            std::string result = join(v.second.c.upper, fmt) + '[' + join(v.first, fmt) + ']' + join(v.second.c.lower);
            std::size_t old_comment_colummn = fmt.comment_column, comment_min_colummn = fmt.comment_column;
            std::vector<std::size_t> comment_pos;
            std::vector<std::size_t> kv_size;
            std::size_t current = result.size();
            for (const auto& kv : v.second.kvs) {
                result += join(kv, fmt);
                if (fmt.comment_column != static_cast<std::size_t>(-1)) {
                    std::size_t pos = current + fmt.comment_column;
                    comment_pos.push_back(pos);
                    std::size_t line_pos = result.find_last_of('\n', pos);
                    kv_size.push_back(fmt.string_length_calculator(((std::string_view)result).substr(line_pos + 1, pos - line_pos - 1)));
                    comment_min_colummn = std::max(comment_min_colummn, kv_size.back());
                }
                fmt.comment_column = old_comment_colummn;
                current = result.size();
            }
            if (old_comment_colummn != static_cast<std::size_t>(-1)) {
                for (std::size_t i = comment_pos.size(); i-- > 0;) {
                    result.insert(comment_pos[i], comment_min_colummn - kv_size[i], ' ');
                }
            }
            result += "\n";
            return result;
        }

        parse_status parse(std::string_view sv, table& v) {
            auto& [sections, c] = v;
            sections.clear();
            if (const auto& [nsv, succeed] = parse(sv, c.lower); sv = nsv, !succeed) return {sv, false};
            remove_blank(sv);
            while (true) {
                key_section_pair ks;
                if (const auto& [nsv, succeed] = parse(sv, ks); sv = nsv, !succeed) {
                    if (!sv.empty()) return {sv, false};
                    c.upper = ks.second.c.upper;
                    return {sv, true};
                }
                sections.insert(std::exchange(ks, key_section_pair{}));
            }
        }

        std::string join(const table& v, const join_format& fmt) {
            std::string result = join(v.c.lower, fmt);
            if (result == "\n")
                result.pop_back();
            else
                result += "\n";
            for (const auto& kv : v.sections)
                result += join(kv, fmt);
            return result + join(v.c.upper, fmt);
        }
    }
}

#endif
