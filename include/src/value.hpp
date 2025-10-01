#ifndef HWSHQTB__INI__VALUE_HPP
#define HWSHQTB__INI__VALUE_HPP

#include "global.hpp"
#include <variant>
#include <optional>

namespace hwshqtb {
    namespace ini {
        namespace detail {
            template <typename T, typename Variant>
            struct value_helper;

            template <typename T>
            struct value_helper<T, std::variant<>> {
                static constexpr std::size_t same_index = 0;
                static constexpr bool constructible_is = false;
                static constexpr std::size_t constructible_index = 0;
                using constructible_type = void;
            };

            template <typename T, typename U, typename... Types>
            struct value_helper<T, std::variant<U, Types...>> {
                static constexpr std::size_t same_index = std::is_same_v<T, U> ? 0 : 1 + value_helper<T, std::variant<Types...>>::same_index;
                static constexpr bool constructible_is = std::is_constructible_v<U, T>;
                static constexpr std::size_t constructible_index = constructible_is ? 0 : 1 + value_helper<T, std::variant<Types...>>::constructible_index;
                using constructible_type = std::conditional_t<constructible_is, U, typename value_helper<T, std::variant<Types...>>::constructible_type>;
            };
        }

        class value {
            using base_type = std::variant<integer, floating, boolean, string, array>;

        public:
            value():
                _v(nullptr) {}
            value(const value& v):
                _v(v._v == nullptr ? nullptr : new base_type(*v._v)) {}
            value(value&& v)noexcept:
                _v(std::exchange(v._v, nullptr)) {}
            template <typename T, std::enable_if_t<(detail::value_helper<std::decay_t<T>, base_type>::constructible_index < 5), int> = 0>
            value(T&& v) :
                _v(new base_type(std::in_place_type_t<typename detail::value_helper<std::decay_t<T>, base_type>::constructible_type>{}, std::forward<T>(v))) {}

            ~value() {
                delete _v;
            }

            value& operator=(const value& v) {
                if (v._v == nullptr)
                    clear();
                else if (_v == nullptr)
                    _v = new base_type(*v._v);
                else
                    *_v = *v._v;
                return *this;
            }
            value& operator=(value&& v)noexcept {
                delete _v;
                _v = std::exchange(v._v, nullptr);
                return *this;
            }
            template <typename T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, value>, int> = 0>
            value& operator=(T&& v) {
                if (_v == nullptr)
                    _v = new base_type(std::forward<T>(v));
                else
                    *_v = std::forward<T>(v);
                return *this;
            }

            template <typename T, std::enable_if_t<(detail::value_helper<std::decay_t<T>, base_type>::same_index < 5), int> = 0>
            auto& ref() {
                return std::get<T>(*_v);
            }

            template <typename T, std::enable_if_t<(detail::value_helper<std::decay_t<T>, base_type>::same_index < 5), int> = 0>
            const auto& ref()const {
                return std::get<T>(*_v);
            }

            template <typename T>
            auto get()const {
                return std::visit([](auto&& v) {
                    using R = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, R>)
                        return std::make_optional(v);
                    else if constexpr (std::is_same_v<T, string>)
                        return std::make_optional(join(v, global_format));
                    else if constexpr (std::is_same_v<T, array>) {
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
                }, *_v);
            }

            template <typename T>
            auto value_or(T&& v)const {
                return get<std::decay_t<T>>().value_or(v);
            }

            template <class F>
            auto visit(F&& f)const {
                return std::visit(std::forward<F>(f), *_v);
            }

            template <typename T>
            bool is_type()const {
                return std::holds_alternative<T>(*_v);
            }

#define JUDGE_TYPE(T) \
bool is_##T()const { \
    return is_type<T>(); \
}

            JUDGE_TYPE(string)
                JUDGE_TYPE(integer)
                JUDGE_TYPE(floating)
                JUDGE_TYPE(boolean)
                JUDGE_TYPE(array)
#undef JUDGE_TYPE

#define TYPE(T) \
T& as_##T() { \
    return ref<T>(); \
}\
const T& as_##T()const {\
    return ref<T>(); \
}

                TYPE(string)
                TYPE(integer)
                TYPE(floating)
                TYPE(boolean)
                TYPE(array)
#undef TYPE

                template <typename T, std::enable_if_t<(detail::value_helper<std::decay_t<T>, base_type>::constructible_index < 5), int> = 0>
            void set(T&& value) {
                if (_v == nullptr)
                    _v = new base_type(std::in_place_type_t<typename detail::value_helper<std::decay_t<T>, base_type>::constructible_type>{}, std::forward<T>(value));
                else {
                    using RT = std::decay_t<T>;
                    bool check = std::visit([&](auto&& v) -> bool {
                        using R = std::decay_t<decltype(v)>;
                        if constexpr (std::is_same_v<RT, string>) {
                            return parse(value, v).succeed;
                        }
                        else if constexpr (std::is_same_v<RT, array>) {
                            return false;
                        }
                        else {
                            if constexpr (std::is_same_v<R, array>)
                                return false;
                            else if constexpr (std::is_same_v<R, string>) {
                                v = join(value);
                                return true;
                            }
                            else {
                                v = static_cast<RT>(value);
                                return true;
                            }
                        }
                    }, *_v);
                    if (!check)
                        _v->emplace<detail::value_helper<std::decay_t<T>, base_type>::constructible_index>(value);
                }
            }

            template <typename Iter>
            void set(Iter l, Iter r) {
                set(array{});
                auto& arr = as_array();
                while (l != r)
                    arr.emplace_back(*l++);
            }

            void clear() {
                delete _v;
                _v = nullptr;
            }

        private:
            base_type* _v;

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
#undef PARSE
            return {sv, false};
        }

        std::string join(const value& v, const join_format& fmt) {
            return v.visit([&](auto&& r) {
                return join(r, fmt);
            });
        }
    }
}

#endif