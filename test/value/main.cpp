#include "ini.hpp"
#include <vector>
#include <array>

int main() {
    using namespace hwshqtb::ini;

    hwshqtb::ini::value v(12345);
    v.set(123);
    std::cout << hwshqtb::ini::join(v, hwshqtb::ini::global_format) << std::endl;
    
    v.set(std::vector{std::array<int, 2>{1, 2}, std::array<int, 2>{3, 4}});
    std::cout << hwshqtb::ini::join(v, hwshqtb::ini::global_format) << std::endl;
    return 0;
}