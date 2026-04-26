# Compiler-project

This repository contains a C++ compiler frontend for AC-Lang.

## Files

- `CP.cpp`: complete compiler frontend source code.
- `PROJECT_EXPLANATION.md`: detailed explanation for studying the project.

## AC-Lang example

```txt
solve {
  x = read();
  y = 5;
  if (x > y) {
    print(x);
  }
  loop (x == y) {
    x = x + 1;
    print(x);
  }
}
```

## How to compile

Use `g++`:

```bash
g++ -std=c++17 CP.cpp -o cp
```

## How to run

Run with an input file:

```bash
./cp program.ac
```

Or type/paste the program through standard input:

```bash
./cp
```

The program prints:

- `Accepted` when lexical analysis and parsing succeed.
- `Rejected` when the source code has a lexical or syntax error.

## Quick test

```bash
cat > program.ac <<'EOF'
solve {
  x = read();
  y = 10;
  if (x > y) {
    print(x);
  }
}
EOF

g++ -std=c++17 CP.cpp -o cp
./cp program.ac
```