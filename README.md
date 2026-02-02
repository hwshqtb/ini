# hwshqtb's ini parser

[![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/License-BSL--1.0-blue.svg)](https://opensource.org/licenses/BSL-1.0)

a special ini parser that parse a ml compromise between original ini and toml

## highlights

- parse and change comment
- (partial) control join format
  - indent for key, array
  - comment's alignment
- contain orders between keys

## aimed ini format
```ini
#! file comment

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

# file comment part2
```
