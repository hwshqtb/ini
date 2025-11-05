#include "ini.hpp"
#include <iostream>
#include <vector>
#include <array>

int main() {
    using namespace hwshqtb::ini;

    hwshqtb::ini::value v;
    v.set(std::deque<double>{1.4, 2.3});
    std::cout << hwshqtb::ini::join(v, hwshqtb::ini::global_format) << std::endl;
    
    v.set(std::vector{std::array<int, 2>{1, 2}, std::array<int, 2>{1, 2}});
    std::cout << v.as_array().front().is_floating() << std::endl;
    std::cout << hwshqtb::ini::join(v, hwshqtb::ini::global_format) << std::endl;

    return 0;
}