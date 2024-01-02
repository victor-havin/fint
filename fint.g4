//==============================================================================
// fint.g4
//
// Formula interpreter grammar file for ANTLR4
// 1/1/2024
//==============================================================================
grammar fint;

parse
    : program EOF
    ;

program
    : (operation)* 
    ;

operation
    : variable ASSIGNMENT expression SEMICOLON
    ;

variable
    : VARIABLE
    ;

expression
    :  term ( (PLUS | MINUS) term )*
    ;

term
    : factor ( (MUL | DIV) factor )*
    ;

factor
    : satom (POWER satom)*
    ;

satom
    : PLUS atom
    | MINUS atom
    | atom
    ;

atom    
    : float
    | variable
    | function
    | LPAREN expression RPAREN
    ;

function
    : VARIABLE LPAREN expression (',' expression)* RPAREN 
    ;

float
    : DECIMAL
    | SCIENTIFIC
    ;

DECIMAL
    : NUM+ ('.' NUM +)? 
    ;

SCIENTIFIC
    : DECIMAL [E|e] (PLUS | MINUS)? NUM+
    ;

SEMICOLON 
    : ';'
    ;

VARIABLE
    : ALPHA ALPHANUM*
    ;

ALPHA
    : 'a' .. 'z'
    | 'A' .. 'Z'
    | '_'
    ;

NUM
    : '0' .. '9'
    ;

ALPHANUM
    : ALPHA
    | NUM
    ;

ASSIGNMENT
    : '='
    ;

PLUS
    : '+'
    ;

MINUS
    : '-'
    ;

MUL
    : '*'
    ;

DIV
    : '/'
    ;

POWER
    : '^'
    ;

LPAREN
    : '('
    ;

RPAREN
    : ')'
    ;

WS
    : [\r\n\t ]+ -> skip
    ;