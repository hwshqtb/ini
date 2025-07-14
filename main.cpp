#include "src/ini.hpp"
#include <iostream>

int main() {
    std::cout << hwshqtb::ini::parse(std::string("./aimed.ini")).value();
}