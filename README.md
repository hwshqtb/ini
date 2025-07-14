# hwshqtb's ini parser

a special ini parser that parse a ml compromise between original ini and toml

## highlights

- parse and control comment
- (partial) control join format 
- contain orders\ between keys

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
