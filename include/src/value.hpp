#ifndef HWSHQTB__INI__VALUE_HPP
#define HWSHQTB__INI__VALUE_HPP

#include "global.hpp"
#include <variant>
#include <optional>

namespace hwshqtb {
    namespace ini {
        class value {
            using base_type = std::variant<integer, floating, boolean, string, array>;

        public:
            value():
                _v(nullptr) {}

            value(const value& v):
                _v(v._v == nullptr ? nullptr : new base_type(*v._v)) {}

            value(value&& v) noexcept:
                _v(std::exchange(v._v, nullptr)) {}

            template <typename T>
            value(T&& v):
                _v(nullptr) {
                reset(std::forward<T>(v));
            }

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

            value& operator=(value&& v) noexcept {
                delete _v;
                _v = std::exchange(v._v, nullptr);
                return *this;
            }

            template <typename T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, value>, int> = 0>
            value& operator=(T&& v) {
                reset(std::forward<T>(v));
                return *this;
            }

            template <typename T, std::enable_if_t<(detail::value_helper<T>::same_index < 5), int> = 0>
            auto& ref() {
                return std::get<T>(*_v);
            }

            template <typename T, std::enable_if_t<(detail::value_helper<T>::same_index < 5), int> = 0>
            const auto& ref() const {
                return std::get<T>(*_v);
            }

            template <typename T>
            auto get() const {
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
                            if (succeed) return std::make_optional(r);
                            return std::optional<T>{std::nullopt};
                        }
                        else {
                            return std::make_optional(static_cast<T>(v));
                        }
                    }
                }, *_v);
            }

            template <typename T>
            auto value_or(T&& v) const {
                return get<std::decay_t<T>>().value_or(v);
            }

            template <class F>
            auto visit(F&& f) const {
                return std::visit(std::forward<F>(f), *_v);
            }

            template <typename T>
            bool is_type() const {
                if (_v == nullptr) return false;
                return std::holds_alternative<T>(*_v);
            }

#define JUDGE_TYPE(T)        \
    bool is_##T() const {    \
        return is_type<T>(); \
    }

            JUDGE_TYPE(string)
            JUDGE_TYPE(integer)
            JUDGE_TYPE(floating)
            JUDGE_TYPE(boolean)
            JUDGE_TYPE(array)
#undef JUDGE_TYPE

#define TYPE(T)               \
    T& as_##T() {             \
        return ref<T>();      \
    }                         \
    const T& as_##T() const { \
        return ref<T>();      \
    }

            TYPE(string)
            TYPE(integer)
            TYPE(floating)
            TYPE(boolean)
            TYPE(array)
#undef TYPE

            template <typename T>
            void set(T&& v) {
                using RT = std::decay_t<T>;
                constexpr std::size_t same_index = detail::value_helper<RT>::same_index;
                constexpr std::size_t constructible_index = detail::value_helper<RT>::constructible_index;
                if constexpr (std::is_same_v<RT, value>) {
                    *this = std::forward<T>(v);
                    return;
                }
                else if constexpr (same_index == 3) {
                    if (is_integer() && parse((std::string_view)v, as_integer()).succeed) return;
                    if (is_floating() && parse((std::string_view)v, as_floating()).succeed) return;
                    if (is_boolean()) {
                        if (parse((std::string_view)v, as_boolean()).succeed) return;
                        floating f;
                        if (parse((std::string_view)v, f).succeed) {
                            as_boolean() = f != 0;
                            return;
                        }
                    }
                    if (is_array() && parse((std::string_view)v, as_array()).succeed) return;
                    if (_v == nullptr)
                        _v = new base_type(std::in_place_type_t<string>{}, std::forward<T>(v));
                    else
                        *_v = std::forward<T>(v);
                    return;
                }
                else if constexpr (constructible_index <= 2) {
                    if (_v == nullptr)
                        _v = new base_type(std::in_place_type_t<std::variant_alternative_t<constructible_index, base_type>>{}, v);
                    else if (_v->index() == 0)
                        _v->emplace<0>(v);
                    else if (_v->index() == 1)
                        _v->emplace<1>(v);
                    else
                        _v->emplace<2>(v);
                }
                else if constexpr (constructible_index == 3) {
                    if (_v == nullptr)
                        _v = new base_type(std::in_place_type_t<string>{}, v);
                    else
                        _v->emplace<3>(v);
                }
                else if constexpr (constructible_index == 4) {
                    if (!this->is_array()) {
                        if (_v == nullptr)
                            _v = new base_type(array{});
                        else
                            *_v = array{};
                    }
                    auto& arr = as_array();
                    if (arr.size() == 0) arr.emplace_back(*v.begin());
                    arr.resize(v.size(), arr.front());
                    for (std::size_t index = 0; index < v.size(); ++index) {
                        arr[index].set(*(std::begin(v) + index));
                    }
                }
                else {
                    static_assert(!std::is_same_v<T, T>, "Type T is not supported to set to value.");
                }
            }

            template <typename T>
            void reset(T&& v) {
                using RT = std::decay_t<T>;
                constexpr std::size_t constructible_index = detail::value_helper<RT>::constructible_index;
                if constexpr (std::is_same_v<RT, value>) {
                    *this = std::forward<T>(v);
                    return;
                }
                else if constexpr (constructible_index <= 3) {
                    if (_v == nullptr)
                        _v = new base_type(std::in_place_type_t<std::variant_alternative_t<constructible_index, base_type>>{}, v);
                    else
                        _v->emplace<constructible_index>(v);
                }
                else if constexpr (constructible_index == 4) {
                    if (_v == nullptr) _v = new base_type(array{});
                    auto& arr = as_array();
                    arr.clear();
                    for (const auto& item : v)
                        arr.emplace_back(item);
                }
                else {
                    static_assert(!std::is_same_v<T, T>, "Type T is not supported to reset to value.");
                }
            }

            void clear() {
                delete _v;
                _v = nullptr;
            }

        private:
            base_type* _v;
        };

        parse_status parse(std::string_view sv, value& v) {
            v.clear();

#define PARSE(T)                                                  \
    do {                                                          \
        T r;                                                      \
        if (const auto& [nsv, succeed] = parse(sv, r); succeed) { \
            v = r;                                                \
            return {nsv, true};                                   \
        }                                                         \
    }                                                             \
    while (0)
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
