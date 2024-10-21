```bnf
<program> ::= <function>*

<function> ::= "def" <identifier> "(" <parameter_list>* ")" "{" <statement>* <return_statement>* "}"

<block> ::= "{" <statement>* "}"

<statement> ::= <variable_declaration> | <assignment> | <conditional> 
              | <loop> | <function_call> | <return_statement> | <io_statement>

<variable_declaration> ::= <identifier>

<assignment> ::= <identifier> "=" <expression>

<conditional> ::= "if" "(" <expression> ")" <block> 
                | "if" "(" <expression> ")" <block> "else" <block>
                | "if" "(" <expression> ")" <block> "else" "if" "(" <expression> ")" <block>

<loop> ::= "while" "(" <expression> ")" <block>
         | "for" "(" <identifier> "=" <expression> ":" <expression> (: <expression>)* ")" <block>

<function_call> ::= <identifier> "(" <argument_list>* ")"

<return_statement> ::= "return" <expression>?

<io_statement> ::= "print" "(" <expression> ")" 
                 | "read" "(" <expression> ")"

<expression> ::= <factor> (("+" | "-" | "*" | "/" | "**" | "<" | ">" | "<=" | ">=" | "==" | "!=") <factor>)*

<factor> ::= <number> | <identifier> | "(" <expression> ")" | <string>

<parameter_list> ::= <identifier> ("," <identifier>)*

<argument_list> ::= <expression> ("," <expression>)*

<identifier> ::= <letter> (<letter> | <digit> | "_" )*

<number> ::= <digit>+

<string> ::= "\"" <character>* "\""

<letter> ::= "a" | "b" | "c" | ... | "z" | "A" | "B" | ... | "Z"

<digit> ::= "0" | "1" | "2" | ... | "9"

<character> ::= <any valid character except quote>
```