#include "ini.hpp"
#include <iostream>

int main() {
    std::string_view content = R"(#! file comment

# section comment
[section] # The same line comment
#! section lower comment
# key comment
arr_1 = [ [1, 2], ["str"] ] # The same line comment
#! key lower comment
" escape sequence \b\t\f\n\v\u1000 etc" = "\b\t\f\n\v\u1000"

[section2]
else = "else"
#! else comment

# file comment part2)";
    auto table = hwshqtb::ini::parse(content).value();
    std::cout << hwshqtb::ini::join(table) << std::endl;

    hwshqtb::ini::join_format fmt;
    fmt.array_element_newline = false;
    std::cout << "---- With array_element_newline false ----" << std::endl;
    std::cout << hwshqtb::ini::join(table, fmt);
}