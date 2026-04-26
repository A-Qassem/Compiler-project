# AC-Lang Compiler Frontend Explanation

## Project goal

The project implements a compiler frontend for AC-Lang in one C++ file, `CP.cpp`.

A compiler frontend usually performs two main phases:

1. Lexical analysis: reads raw characters and produces tokens.
2. Syntax analysis: reads tokens and checks whether they match the grammar.

This project does not generate machine code or intermediate code. It accepts or rejects AC-Lang programs according to the required specification.

## AC-Lang summary

AC-Lang supports:

- Keywords: `solve`, `read`, `print`, `loop`, `if`
- Identifiers: a letter followed by zero or more letters or digits
- Numbers: positive integers
- Operators: `=`, `+`, `-`, `>`, `==`
- Symbols: `{`, `}`, `(`, `)`, `;`

Every valid program starts with `solve`, then a block surrounded by braces:

```txt
solve {
  statements
}
```

## Grammar used

```txt
Program  -> 'solve' '{' StmtList '}'
StmtList -> Stmt StmtList | empty
Stmt     -> Assign | Loop | If | Print
Assign   -> ID '=' Expr ';'
Expr     -> 'read' '(' ')' | Term | Term '+' Term | Term '-' Term
Term     -> ID | NUM
Loop     -> 'loop' '(' Cond ')' '{' StmtList '}'
If       -> 'if' '(' Cond ')' '{' StmtList '}'
Cond     -> Term OP Term
Print    -> 'print' '(' Term ')' ';'
```

## File structure

`CP.cpp` contains:

- The token structure.
- Global source-code storage.
- The DFA lexical analyzer.
- The recursive descent parser.
- The `main` function.

## Token representation

Each token stores:

- `t`: token type.
- `v`: token value.
- `l`: source line number.
- `c`: source column number.

Examples:

| Source text | Token type | Token value |
|---|---|---|
| `solve` | `solve` | `solve` |
| `abc1` | `ID` | `abc1` |
| `123` | `NUM` | `123` |
| `==` | `OP` | `==` |
| `{` | `{` | `{` |

Line and column are used for error messages.

## Lexical analyzer

The lexical analyzer is implemented in the `lex()` function.

It reads the program character by character and uses DFA states:

| State | Meaning |
|---|---|
| `0` | Start state |
| `1` | Reading an identifier or keyword |
| `2` | Reading a number |
| `3` | Reading `=` or `==` |

### State 0

State `0` decides what kind of token starts at the current character.

- A letter moves to state `1`.
- A digit moves to state `2`.
- `=` moves to state `3`.
- `+`, `-`, and `>` immediately become operator tokens.
- `{`, `}`, `(`, `)`, and `;` immediately become symbol tokens.
- Whitespace is skipped.
- Any other character is rejected.

### State 1

State `1` reads identifiers and keywords.

It keeps accepting letters and digits. When the next character is not a letter or digit, the collected text is checked:

- If it is one of the AC-Lang keywords, its token type is the keyword itself.
- Otherwise, it becomes an `ID` token.

Examples:

```txt
solve -> keyword token
x1    -> ID token
```

### State 2

State `2` reads numbers.

It keeps accepting digits. If a letter appears after digits, the token is invalid because identifiers cannot start with digits.

Examples:

```txt
123  -> valid NUM
12ab -> invalid number
```

The code also rejects zero because the specification says numbers are positive integers.

### State 3

State `3` handles `=` and `==`.

- If the next character is another `=`, the token is `==` with type `OP`.
- Otherwise, the token is assignment `=`.

This is necessary because `=` and `==` share the same first character.

## Parser

The parser is implemented using recursive descent.

Recursive descent means each grammar rule is represented by a function. The parser reads tokens from left to right and calls functions that match the grammar.

The parser uses:

- `idx`: current token index.
- `cur()`: returns the current token.
- `is(t)`: checks whether the current token type is `t`.
- `eat(t)`: consumes the current token if it matches `t`; otherwise reports an error.

## Parser functions

### Program

Grammar:

```txt
Program -> 'solve' '{' StmtList '}'
```

Function:

```txt
prog()
```

It expects:

1. `solve`
2. `{`
3. statement list
4. `}`
5. end of file

### Statement list

Grammar:

```txt
StmtList -> Stmt StmtList | empty
```

Function:

```txt
list()
```

The implementation uses a loop. While the current token can start a statement, it parses another statement.

Statement-start tokens are:

- `ID`
- `loop`
- `if`
- `print`

If none of these appears, the statement list is considered empty.

### Statement

Grammar:

```txt
Stmt -> Assign | Loop | If | Print
```

Function:

```txt
stmt()
```

The parser chooses the correct statement type from the current token:

- `ID` means assignment.
- `loop` means loop statement.
- `if` means if statement.
- `print` means print statement.

### Assignment

Grammar:

```txt
Assign -> ID '=' Expr ';'
```

Function:

```txt
asn()
```

Example:

```txt
x = read();
y = x + 1;
```

### Expression

Grammar:

```txt
Expr -> 'read' '(' ')' | Term | Term '+' Term | Term '-' Term
```

Function:

```txt
expr()
```

The parser supports:

- `read()`
- one term
- one addition
- one subtraction

Examples:

```txt
read()
x
5
x + 1
y - 2
```

Long expressions such as `x + y + z` are not part of the given grammar and are rejected.

### Term

Grammar:

```txt
Term -> ID | NUM
```

Function:

```txt
term()
```

A term is either an identifier or a number.

### Condition

Grammar:

```txt
Cond -> Term OP Term
```

Function:

```txt
cond()
```

The parser accepts condition operators:

- `>`
- `==`

Examples:

```txt
x > 5
x == y
```

### Loop

Grammar:

```txt
Loop -> 'loop' '(' Cond ')' '{' StmtList '}'
```

Function:

```txt
lop()
```

Example:

```txt
loop (x > 1) {
  print(x);
}
```

### If

Grammar:

```txt
If -> 'if' '(' Cond ')' '{' StmtList '}'
```

Function:

```txt
iff()
```

Example:

```txt
if (x == y) {
  print(x);
}
```

### Print

Grammar:

```txt
Print -> 'print' '(' Term ')' ';'
```

Function:

```txt
prt()
```

Example:

```txt
print(x);
print(10);
```

## Program input

The program can read AC-Lang source code in two ways:

1. From a file path passed as the first command-line argument.
2. From standard input if no file path is given.

Examples:

```bash
./cp program.ac
```

or:

```bash
./cp
```

## Output

If the lexical analyzer and parser both succeed:

```txt
Accepted
```

If a lexical or syntax error occurs:

```txt
Rejected
```

The program also prints an error location and reason.

## Example accepted program

```txt
solve {
  x = read();
  y = 10;
  if (x > y) {
    print(x);
  }
  loop (x == y) {
    y = y + 1;
    print(y);
  }
}
```

## Example rejected program

```txt
solve {
  x = read()
}
```

This is rejected because the assignment statement is missing a semicolon after `read()`.

## Why this matches the requirements

- The lexical analyzer is written manually with DFA-style states and character-by-character transitions.
- The parser is written manually using recursive descent.
- No Lex, Yacc, parser generator, or regex library is used.
- The complete implementation is in one C++ source file.
- The code can be compiled directly with `g++`.
