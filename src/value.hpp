#ifndef HWSHQTB__TINI__VALUE_HPP
#define HWSHQTB__TINI__VALUE_HPP

#include "global.hpp"
#include <variant>
#include <optional>

namespace hwshqtb {
    namespace ini {
        class value {
            using base_type = std::variant<std::monostate, string, integer, floating, boolean, array>;

        public:
            value():
                _v(std::monostate{}) {}

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            value(T&& v):
                _v(std::forward<T>(v)) {}

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            value& operator=(T&& v) {
                _v = std::forward<T>(v);
                return *this;
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            auto& ref() {
                return std::get<T>(_v);
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            const auto& ref()const {
                return std::get<T>(_v);
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            auto get()const {
                return std::visit([](auto&& v) {
                    using R = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<R, std::monostate>)
                        return std::optional<T>{std::nullopt};
                    else if constexpr (std::is_same_v<T, string>)
                        return std::make_optional(join((std::string_view)v, global_format));
                    else if constexpr (std::is_same_v<T, array>) {
                        if constexpr (std::is_same_v<R, array>)
                            return std::make_optional(v);
                        else
                            return std::optional<T>{std::nullopt};
                    }
                    else {
                        if constexpr (std::is_same_v<R, array>)
                            return std::optional<T>{std::nullopt};
                        else if constexpr (std::is_same_v<R, string>) {
                            T r;
                            const auto& [_, succeed] = parse((std::string_view)v, r);
                            if (succeed)
                                return std::make_optional(r);
                            return std::optional<T>{std::nullopt};
                        }
                        else {
                            return std::make_optional(static_cast<T>(v));
                        }
                    }
                }, _v);
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            auto value_or(T&& v) {
                return get<std::decay_t<T>>().value_or(v);
            }

            template <class F>
            auto visit(F&& f)const {
                return std::visit(std::forward<F>(f), _v);
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            bool is_type()const {
                return std::holds_alternative<T>(_v);
            }

            template <typename T, std::enable_if_t<!std::is_same_v<T, std::monostate>, int> = 0>
            void set(T&& v) {
                _v = std::forward<T>(v);
            }

            void clear() {
                _v = std::monostate{};
            }

        private:
            base_type _v;

        };

        template <>
        parse_status parse(std::string_view sv, value& v) {
            v.clear();

#define PARSE(T) \
do { \
    T r; \
    if (const auto& [nsv, succeed] = parse(sv, r); succeed) { \
        v = r; \
        return {nsv, true}; \
    } \
} while (0)
            PARSE(array);
            PARSE(string);
            PARSE(boolean);
            PARSE(integer);
            PARSE(floating);
            return {sv, false};
        }

        template <>
        std::string join(const value& v, const join_format& fmt) {
            return v.visit([&](auto&& r) {
                return join(r, fmt);
            });
        }
    }
}

#endif