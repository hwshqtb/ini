#ifndef HWSHQTB__INI__VALUE_HPP
#define HWSHQTB__INI__VALUE_HPP

#include "global.hpp"
#include <variant>
#include <optional>

namespace hwshqtb {
    namespace ini {
        class value {
            using base_type = std::variant<string, integer, floating, boolean, array>;

        public:
            value():
                _v(nullptr) {}
            value(const value& v):
                _v(v._v == nullptr ? nullptr : new base_type(*v._v)) {}
            value(value&& v):
                _v(std::exchange(v._v, nullptr)) {}
            template <typename T>
            value(T&& v):
                _v(new base_type(std::forward<T>(v))) {}

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
            value& operator=(value&& v) {
                delete _v;
                _v = std::exchange(v._v, nullptr);
                return *this;
            }
            template <typename T>
            value& operator=(T&& v) {
                if (_v == nullptr)
                    _v = new base_type(std::forward<T>(v));
                else
                    *_v = std::forward<T>(v);
                return *this;
            }

            template <typename T>
            auto& ref() {
                return std::get<T>(*_v);
            }

            template <typename T>
            const auto& ref()const {
                return std::get<T>(*_v);
            }

            template <typename T>
            auto get()const {
                return std::visit([](auto&& v) {
                    using R = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<R, std::monostate>)
                        return std::optional<T>{std::nullopt};
                    else if constexpr (std::is_same_v<T, string>)
                        return std::make_optional(join(v, global_format));
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
                }, *_v);
            }

            template <typename T>
            auto value_or(T&& v) {
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

            template <typename T>
            void set(T&& v) {
                *_v = std::forward<T>(v);
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

        template <>
        std::string join(const value& v, const join_format& fmt) {
            return v.visit([&](auto&& r) {
                return join(r, fmt);
            });
        }
    }
}

#endif