#ifndef HWSHQTB__INI_HPP
#define HWSHQTB__INI_HPP


/*** Start of inlined file: global.hpp ***/
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

/*** Start of inlined file: ordered_map.hpp ***/
#ifndef HWSHQTB__CONTAINER__ORDERED_MAP_HPP
#define HWSHQTB__CONTAINER__ORDERED_MAP_HPP

/*
*   2025-07-07
*       first untested version
*/

// #include "../version.hpp"
#include <stdexcept>
#include <list>
#include <map>

namespace hwshqtb {
    namespace container {
        template <typename Key, typename T,
            class Container = std::list<std::pair<const Key, T>>,
            class Map = std::map<Key, typename Container::iterator>>
        class ordered_map {
        public:
            using key_type = Key;
            using mapped_type = T;
            using value_type = std::pair<const Key, T>;
            using size_type = typename Container::size_type;
            using difference_type = typename Container::difference_type;
            using allocator_type = typename Container::allocator_type;
            using reference = typename Container::reference;
            using const_reference = typename Container::const_reference;
            using pointer = typename Container::pointer;
            using const_pointer = typename Container::const_pointer;
            using iterator = typename Container::iterator;
            using const_iterator = typename Container::const_iterator;
            using reverse_iterator = typename Container::reverse_iterator;
            using const_reverse_iterator = typename Container::const_reverse_iterator;
            using container_type = Container;

            ordered_map() = default;
            ordered_map(const ordered_map& other): _container(other._container), _map(other._map) {}
            ordered_map(ordered_map&& other) noexcept: _container(std::move(other._container)), _map(std::move(other._map)) {}
            explicit ordered_map(const container_type& container): _container(container) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            explicit ordered_map(container_type&& container) noexcept : _container(std::move(container)) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            template <typename InputIt>
            ordered_map(InputIt first, InputIt last, const allocator_type& allocator = allocator_type())
                : _container(allocator) {
                for (auto it = first; it != last; ++it) {
                    insert(*it);
                }
            }
            ordered_map(std::initializer_list<value_type> ilist)
                : _container(ilist) {
                for (const auto& value : ilist) {
                    _map[value.first] = --_container.end();
                }
            }
            template <class Alloc>
            explicit ordered_map(const Alloc& allocator): _container(allocator), _map(allocator) {}
            template <class Alloc>
            ordered_map(const ordered_map& other, const Alloc& allocator)
                : _container(other._container, allocator), _map(other._map, allocator) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            template <class Alloc>
            ordered_map(ordered_map&& other, const Alloc& allocator) noexcept
                : _container(std::move(other._container), allocator), _map(std::move(other._map), allocator) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            template <class Alloc>
            ordered_map(const container_type& container, const Alloc& allocator): _container(container, allocator), _map(allocator) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            template <class Alloc>
            ordered_map(container_type&& container, const Alloc& allocator) noexcept
                : _container(std::move(container), allocator), _map(allocator) {
                for (auto it = _container.begin(); it != _container.end(); ++it) {
                    _map[it->first] = it;
                }
            }
            template <typename InputIt, class Alloc>
            ordered_map(InputIt first, InputIt last, const Alloc& allocator)
                : _container(allocator) {
                for (auto it = first; it != last; ++it) {
                    insert(*it);
                }
            }
            template <class Alloc>
            ordered_map(std::initializer_list<value_type> ilist, const Alloc& allocator)
                : _container(ilist, allocator), _map(allocator) {
                for (const auto& value : ilist) {
                    _map[value.first] = --_container.end();
                }
            }

            ~ordered_map() = default;

            ordered_map& operator=(const ordered_map& other) {
                if (this != &other) {
                    _container = other._container;
                    _map = other._map;
                }
                return *this;
            }
            ordered_map& operator=(ordered_map&& other) noexcept {
                if (this != &other) {
                    _container = std::move(other._container);
                    _map = std::move(other._map);
                }
                return *this;
            }
            ordered_map& operator=(std::initializer_list<value_type> ilist) {
                clear();
                insert(ilist);
                return *this;
            }

            allocator_type get_allocator() const noexcept {
                return _container.get_allocator();
            }

            mapped_type& operator[](const key_type& key) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace_back(key, mapped_type{});
                    _map[key] = --_container.end();
                    return (--_container.end())->second;
                }
                return it->second->second;
            }

            const mapped_type& operator[](const key_type& key)const {
                auto it = _map.find(key);
                return it->second->second;
            }

            mapped_type& at(const key_type& key) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    throw std::out_of_range("Key not found");
                }
                return it->second->second;
            }
            const mapped_type& at(const key_type& key) const {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    throw std::out_of_range("Key not found");
                }
                return it->second->second;
            }

            bool empty() const noexcept {
                return _container.empty();
            }

            size_type size() const noexcept {
                return _container.size();
            }

            size_type max_size() const noexcept {
                return _container.max_size();
            }

            void clear() {
                _container.clear();
                _map.clear();
            }

            void insert(const value_type& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.push_back(value);
                    _map[_container.back().first] = --_container.end();
                }
            }
            void insert(value_type&& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.push_back(std::move(value));
                    _map[_container.back().first] = --_container.end();
                }
            }
            template <typename P>
            typename std::enable_if<std::is_constructible<value_type, P>::value && std::is_same<typename std::decay<P>::type, value_type>::value>::type insert(P&& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.push_back(std::forward<P>(value));
                    _map[_container.back().first] = --_container.end();
                }
            }
            iterator insert(const_iterator hint, const value_type& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.insert(hint, value);
                    auto new_it = --_container.end();
                    _map[_container.back().first] = new_it;
                    return new_it;
                }
                return it->second;
            }
            iterator insert(const_iterator hint, value_type&& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.insert(hint, std::move(value));
                    auto new_it = --_container.end();
                    _map[_container.back().first] = new_it;
                    return new_it;
                }
                return it->second;
            }
            template <typename P>
            typename std::enable_if<std::is_constructible<value_type, P>::value&& std::is_same<typename std::decay<P>::type, value_type>::value, iterator>::type insert(const_iterator hint, P&& value) {
                auto it = _map.find(value.first);
                if (it == _map.end()) {
                    _container.insert(hint, std::forward<P>(value));
                    auto new_it = --_container.end();
                    _map[_container.back().first] = new_it;
                    return new_it;
                }
                return it->second;
            }
            template <typename InputIt>
            void insert(InputIt first, InputIt last) {
                for (auto it = first; it != last; ++it) {
                    insert(*it);
                }
            }
            void insert(std::initializer_list<value_type> ilist) {
                for (const auto& value : ilist) {
                    insert(value);
                }
            }

            template <typename M>
            std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& value) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace_back(key, std::forward<M>(value));
                    auto new_it = --_container.end();
                    _map[key] = new_it;
                    return {new_it, true};
                } else {
                    it->second->second = std::forward<M>(value);
                    return {it->second, false};
                }
            }
            template <typename M>
            std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& value) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace_back(std::move(key), std::forward<M>(value));
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return {new_it, true};
                } else {
                    it->second->second = std::forward<M>(value);
                    return {it->second, false};
                }
            }
            template <typename M>
            iterator insert_or_assign(const_iterator hint, const key_type& key, M&& value) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.insert(hint, std::make_pair(key, std::forward<M>(value)));
                    auto new_it = --_container.end();
                    _map[key] = new_it;
                    return new_it;
                } else {
                    it->second->second = std::forward<M>(value);
                    return it->second;
                }
            }
            template <typename M>
            iterator insert_or_assign(const_iterator hint, key_type&& key, M&& value) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.insert(hint, std::make_pair(std::move(key), std::forward<M>(value)));
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return new_it;
                } else {
                    it->second->second = std::forward<M>(value);
                    return it->second;
                }
            }

            template <typename... Args>
            std::pair<iterator, bool> emplace(Args&&... args) {
                auto it = _map.find(key_type(std::forward<Args>(args)...));
                if (it == _map.end()) {
                    _container.emplace_back(std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return {new_it, true};
                }
                return {it->second, false};
            }
            template <typename... Args>
            iterator emplace_hint(const_iterator hint, Args&&... args) {
                auto it = _map.find(key_type(std::forward<Args>(args)...));
                if (it == _map.end()) {
                    _container.emplace(hint, std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return new_it;
                }
                return it->second;
            }

            template <typename... Args>
            std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace_back(key, std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[key] = new_it;
                    return {new_it, true};
                }
                return {it->second, false};
            }
            template <typename... Args>
            std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace_back(std::move(key), std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return {new_it, true};
                }
                return {it->second, false};
            }
            template <typename... Args>
            iterator try_emplace(const_iterator hint, const key_type& key, Args&&... args) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace(hint, key, std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[key] = new_it;
                    return new_it;
                }
                return it->second;
            }
            template <typename... Args>
            iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    _container.emplace(hint, std::move(key), std::forward<Args>(args)...);
                    auto new_it = --_container.end();
                    _map[new_it->first] = new_it;
                    return new_it;
                }
                return it->second;
            }

            void erase(iterator pos) {
                if (pos != _container.end()) {
                    _map.erase(pos->first);
                    _container.erase(pos);
                }
            }
            void erase(const_iterator pos) {
                if (pos != _container.end()) {
                    _map.erase(pos->first);
                    _container.erase(pos);
                }
            }
            void erase(const_iterator first, const_iterator last) {
                for (auto it = first; it != last; ++it) {
                    _map.erase(it->first);
                }
                _container.erase(first, last);
            }
            void erase(const key_type& key) {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    _container.erase(it->second);
                    _map.erase(it);
                }
            }
            template <typename K>
            void erase(K&& key) {
                auto it = _map.find(std::forward<K>(key));
                if (it != _map.end()) {
                    _container.erase(it->second);
                    _map.erase(it);
                }
            }

            void swap(ordered_map& other) noexcept {
                std::swap(_container, other._container);
                std::swap(_map, other._map);
            }

            iterator begin() noexcept {
                return _container.begin();
            }
            const_iterator begin() const noexcept {
                return _container.begin();
            }
            const_iterator cbegin() const noexcept {
                return _container.cbegin();
            }

            iterator end() noexcept {
                return _container.end();
            }
            const_iterator end() const noexcept {
                return _container.end();
            }
            const_iterator cend() const noexcept {
                return _container.cend();
            }

            reverse_iterator rbegin() noexcept {
                return _container.rbegin();
            }
            const_reverse_iterator rbegin() const noexcept {
                return _container.rbegin();
            }
            const_reverse_iterator crbegin() const noexcept {
                return _container.crbegin();
            }
            reverse_iterator rend() noexcept {
                return _container.rend();
            }
            const_reverse_iterator rend() const noexcept {
                return _container.rend();
            }
            const_reverse_iterator crend() const noexcept {
                return _container.crend();
            }

            size_type count(const key_type& key) const {
                return _map.count(key);
            }
            template <typename K>
            size_type count(const K& key) const {
                return _map.count(key);
            }

            iterator find(const key_type& key) {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return it->second;
                }
                return _container.end();
            }
            const_iterator find(const key_type& key) const {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return it->second;
                }
                return _container.end();
            }
            template <typename K>
            iterator find(const K& key) {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return it->second;
                }
                return _container.end();
            }
            template <typename K>
            const_iterator find(const K& key) const {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return it->second;
                }
                return _container.end();
            }

            std::pair<iterator, iterator> equal_range(const key_type& key) {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return {it->second, it->second + 1};
                }
                return {_container.end(), _container.end()};
            }
            std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return {it->second, it->second + 1};
                }
                return {_container.end(), _container.end()};
            }
            template <typename K>
            std::pair<iterator, iterator> equal_range(const K& key) {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return {it->second, it->second + 1};
                }
                return {_container.end(), _container.end()};
            }
            template <typename K>
            std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
                auto it = _map.find(key);
                if (it != _map.end()) {
                    return {it->second, it->second + 1};
                }
                return {_container.end(), _container.end()};
            }

        private:
            Container _container;
            Map _map;

        };

        template <typename Key, typename T, class Container, class Map>
        bool operator==(const ordered_map<Key, T, Container, Map>& lhs, const ordered_map<Key, T, Container, Map>& rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }

        template <typename Key, typename T, class Container, class Map>
        bool operator!=(const ordered_map<Key, T, Container, Map>& lhs, const ordered_map<Key, T, Container, Map>& rhs) {
            return !(lhs == rhs);
        }
    }
}

#endif
/*** End of inlined file: ordered_map.hpp ***/


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

        };
        HWSHQTB__INLINE join_format global_format;

        struct parse_status {
            std::string_view nsv;
            bool succeed;
        };

        template <typename T>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, T& v) {
            static_assert(true, "T must be a valid ini type");
            return {sv, false};
        }

        template <typename T>
        HWSHQTB__INLINE std::string join(const T& v, const join_format& fmt = global_format) {
            static_assert(true, "T must be a valid ini type");
            return "";
        }

        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, std::string& v); // both key and string
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, integer& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, floating& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, boolean& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, comment_upper_part& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, comment_lower_part& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, value& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, array& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key_value_pair& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, key_section_pair& v);
        template <>
        HWSHQTB__INLINE parse_status parse(std::string_view sv, table& v);

        template <>
        HWSHQTB__INLINE std::string join(const std::string& v, const join_format& fmt); // both key and string
        template <>
        HWSHQTB__INLINE std::string join(const integer& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const floating& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const boolean& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const comment_upper_part& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const comment_lower_part& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const value& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const key& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const array& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const key_value_pair& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const key_section_pair& v, const join_format& fmt);
        template <>
        HWSHQTB__INLINE std::string join(const table& v, const join_format& fmt);

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

/*** End of inlined file: global.hpp ***/


/*** Start of inlined file: basic_type.hpp ***/
#ifndef HWSHQTB__INI__BASIC_TYPE_HPP
#define HWSHQTB__INI__BASIC_TYPE_HPP

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
                    if (c == '\\') {
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

/*** End of inlined file: basic_type.hpp ***/


/*** Start of inlined file: comment.hpp ***/
#ifndef HWSHQTB__INI__COMMENT_HPP
#define HWSHQTB__INI__COMMENT_HPP

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, comment_upper_part& v) {
            v.contents.clear();
            if (sv.empty())
                return {sv, true};
            remove_blank(sv);
            if (sv.substr(0, 2) == "#!")
                return {sv, false};
            while (true) {
                if (sv.substr(0, 1) != "#")
                    break;
                sv.remove_prefix(1);
                if (sv.substr(0, 1) == "!")
                    sv.remove_prefix(1);
                std::size_t end = sv.find_first_of('\n', 0);
                v.contents.emplace_back(sv.substr(0, end));
                if (end == std::string_view::npos)
                    sv.remove_prefix(sv.size());
                else sv.remove_prefix(end + 1);
                remove_blank(sv);
            }
            return {sv, true};
        }

        template <>
        std::string join(const comment_upper_part& v, const join_format& fmt) {
            std::string result;
            for (const auto& line : v.contents)
                result += "#" + line + "\n";
            return result;
        }

        template <>
        parse_status parse(std::string_view sv, comment_lower_part& v) {
            v.contents.clear();
            v.same_line = false;
            if (sv.empty())
                return {sv, true};
            remove_space(sv);
            if (sv.substr(0, 2) != "#!") {
                if (sv.substr(0, 1) == "#") {
                    sv.remove_prefix(1);
                    v.same_line = true;
                    std::size_t end = sv.find_first_of('\n', 0);
                    v.contents.emplace_back(sv.substr(0, end));
                    if (end == std::string_view::npos)
                        sv.remove_prefix(sv.size());
                    else sv.remove_prefix(end + 1);
                }
                remove_blank(sv);
            }
            while (sv.substr(0, 2) == "#!") {
                sv.remove_prefix(2);
                std::size_t end = sv.find_first_of('\n', 0);
                v.contents.emplace_back(sv.substr(0, end));
                if (end == std::string_view::npos)
                    sv.remove_prefix(sv.size());
                else sv.remove_prefix(end + 1);
                remove_blank(sv);
            }
            return {sv, true};
        }

        template <>
        std::string join(const comment_lower_part& v, const join_format& fmt) {
            if (v.contents.empty())
                return "\n";
            std::string result;
            std::size_t index = 0;
            if (v.same_line) {
                result = " #" + v.contents.front() + "\n";
                index = 1;
            }
            while (index < v.contents.size()) {
                result += "#!" + v.contents[index++] + "\n";
            }
            return result;
        }
    }
}

#endif
/*** End of inlined file: comment.hpp ***/


/*** Start of inlined file: value.hpp ***/
#ifndef HWSHQTB__INI__VALUE_HPP
#define HWSHQTB__INI__VALUE_HPP

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
/*** End of inlined file: value.hpp ***/


/*** Start of inlined file: array.hpp ***/
#ifndef HWSHQTB__INI__ARRAY_HPP
#define HWSHQTB__INI__ARRAY_HPP

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, array& v) {
            v.clear();
            if (sv.substr(0, 1) != "[") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            while (sv.size()) {
                value r;
                if (const auto& [nsv, succeed] = parse(sv, r); sv = nsv, !succeed)
                    return {sv, false};
                v.push_back(r);
                remove_space(sv);
                if (sv.substr(0, 1) == ",") {
                    sv.remove_prefix(1);
                    remove_space(sv);
                }
                else if (sv.substr(0, 1) == "]") {
                    sv.remove_prefix(1);
                    return {sv, true};
                }
                else return {sv, false};
            }
            return {sv, false};
        }

        template <>
        std::string join(const array& v, const join_format& fmt) {
            std::string result = "[ ";
            for (const auto& r : v)
                result += join(r, fmt) + ", ";
            if (v.size()) {
                result.pop_back();
                result.pop_back();
                result.push_back(' ');
            }
            result += "]";
            return result;
        }
    }
}

#endif
/*** End of inlined file: array.hpp ***/


/*** Start of inlined file: table.hpp ***/
#ifndef HWSHQTB__INI__TABLE_HPP
#define HWSHQTB__INI__TABLE_HPP


/*** Start of inlined file: key.hpp ***/
#ifndef HWSHQTB__INI__KEY_HPP
#define HWSHQTB__INI__KEY_HPP

namespace hwshqtb {
    namespace ini {
        HWSHQTB__INLINE key::key():
            name(""), is_quoted(false) {}

        HWSHQTB__INLINE key::key(const std::string& c) {
            parse((std::string_view)c, *this);
        }

        template <>
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

        template <>
        std::string join(const key& v, const join_format& fmt) {
            if (v.is_quoted)
                return join(v.name, fmt);
            return v.name;
        }
    }
}

#endif
/*** End of inlined file: key.hpp ***/

#include <iostream>

namespace hwshqtb {
    namespace ini {
        template <>
        parse_status parse(std::string_view sv, key_value_pair& v) {
            auto& [k, r] = v;
            auto& [rv, rc] = r;
            rv.clear();
            if (const auto& [nsv, succeed] = parse(sv, rc.upper); sv = nsv, !succeed)
                return {sv, false};
            remove_space(sv);
            if (sv.empty())
                return {sv, true};
            if (const auto& [nsv, succeed] = parse(sv, k); sv = nsv, !succeed)
                return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "=")
                return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rv); sv = nsv, !succeed)
                return {sv, succeed};
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rc.lower); sv = nsv, !succeed)
                return {sv, false};
            return {sv, true};
        }

        template <>
        std::string join(const key_value_pair& v, const join_format& fmt) {
            std::string lower = join(v.second.second.lower, fmt);
            return join(v.second.second.upper, fmt) +
                join(v.first, fmt) + (fmt.space_around_eq ? " = " : "=") + join(v.second.first, fmt)
                + (lower.substr(0, 2) == "#!" ? "\n" : "") + lower;
        }

        template <>
        parse_status parse(std::string_view sv, key_section_pair& v) {
            auto& [k, r] = v;
            auto& [map, rc] = r;
            map.clear();
            if (const auto& [nsv, succeed] = parse(sv, rc.upper); sv = nsv, !succeed)
                return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "[") return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, k); sv = nsv, !succeed)
                return {sv, false};
            remove_space(sv);
            if (sv.substr(0, 1) != "]")
                return {sv, false};
            sv.remove_prefix(1);
            remove_space(sv);
            if (const auto& [nsv, succeed] = parse(sv, rc.lower); sv = nsv, !succeed)
                return {sv, false};
            remove_blank(sv);
            while (sv.size()) {
                if (sv.substr(0, 1) == "[")
                    break;
                key_value_pair p;
                if (const auto& [nsv, succeed] = parse(sv, p); succeed) {
                    if (p.first.name == "" && !p.first.is_quoted)
                        break;
                    sv = nsv;
                    map.insert(std::exchange(p, key_value_pair{}));
                    remove_blank(sv);
                }
                else return {sv, false};
            }
            return {sv, true};
        }

        template <>
        std::string join(const key_section_pair& v, const join_format& fmt) {
            std::string result = join(v.second.c.upper, fmt) +
                '[' + join(v.first, fmt) + ']' + join(v.second.c.lower);
            for (const auto& kv : v.second.kvs)
                result += join(kv, fmt);
            result += "\n";
            return result;
        }

        template <>
        parse_status parse(std::string_view sv, table& v) {
            auto& [sections, c] = v;
            sections.clear();
            if (const auto& [nsv, succeed] = parse(sv, c.lower); sv = nsv, !succeed)
                return {sv, false};
            remove_blank(sv);
            while (true) {
                key_section_pair ks;
                if (const auto& [nsv, succeed] = parse(sv, ks); sv = nsv, !succeed) {
                    if (!sv.empty())
                        return {sv, false};
                    c.upper = ks.second.c.upper;
                    return {sv, true};
                }
                sections.insert(std::exchange(ks, key_section_pair{}));
            }
        }

        template <>
        std::string join(const table& v, const join_format& fmt) {
            std::string result = join(v.c.lower, fmt);
            if (result == "\n")
                result.pop_back();
            else result += "\n";
            for (const auto& kv : v.sections)
                result += join(kv, fmt);
            return result + join(v.c.upper, fmt);
        }
    }
}

#endif
/*** End of inlined file: table.hpp ***/


/*** Start of inlined file: io.hpp ***/
#ifndef HWSHQTB__INI__IO_HPP
#define HWSHQTB__INI__IO_HPP

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

        HWSHQTB__INLINE parse_result parse(std::ifstream& file) {
            std::istreambuf_iterator<char> begin(file), end;
            std::string str(begin, end);
            return parse((std::string_view)str);
        }

        HWSHQTB__INLINE parse_result parse(const std::string& path) {
            std::ifstream file(path);
            if (!file.is_open())
                return std::nullopt;
            return parse(file);
        }

        HWSHQTB__INLINE std::ostream& operator<<(std::ostream& os, const table& v) {
            return os << join(v);
        }
    }
}

#endif
/*** End of inlined file: io.hpp ***/

#endif
