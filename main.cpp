#include "src/ini.hpp"
#include <iostream>

int main() {
    auto table = hwshqtb::ini::parse(std::string("./aimed.ini")).value();
    std::cout << hwshqtb::ini::join(table);
}