#include "ini.hpp"
#include <iostream>

int main() {
    std::string_view content = R"(#! file comment

# section comment
[section] # The same line comment
#! section lower comment
# key comment
arr_1 = [ [1, 2], ["str"], [] ] # The same line comment
#! key lower comment
" escape sequence \b\t\f\n\v\u1000 etc" = "\b\t\f\n\v\u1000" # The same line comment
#! key lower comment2

[section2]
else = "else"
#! else comment

# file comment part2)";
    auto table = hwshqtb::ini::parse(content).value();

    hwshqtb::ini::join_format fmt;
    // fmt.comment_column = 0;
    // fmt.indent = 0;
    std::cout << hwshqtb::ini::join(table, fmt) << std::endl;

    std::cout << "--------" << std::endl;
    fmt.comment_column = 80;
    fmt.indent = 0;
    fmt.array_element_newline = true;
    std::cout << hwshqtb::ini::join(table, fmt);
}