# AC-Lang CLI Testing

Use this skill when testing the AC-Lang compiler frontend in `A-Qassem/Compiler-project`.

## Devin Secrets Needed

None.

## Setup

1. Work from the repository root.
2. Build the CLI with:

```bash
g++ -std=c++17 -Wall -Wextra CP.cpp -o cp
```

## Runtime Testing

Test valid file input:

```bash
cat > valid.ac <<'EOF'
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
EOF
./cp valid.ac
```

Expected output: exactly `Accepted` and exit code `0`.

Test parser rejection:

```bash
cat > missing_semicolon.ac <<'EOF'
solve {
  x = read()
}
EOF
./cp missing_semicolon.ac
```

Expected output includes `expected ; but found }`, includes `Rejected`, and exits `1`.

Test lexer rejection for non-positive numbers:

```bash
cat > zero.ac <<'EOF'
solve {
  x = 0;
}
EOF
./cp zero.ac
```

Expected output includes `number must be positive`, includes `Rejected`, and exits `1`.

Test invalid-character handling with a timeout so lexer hangs are caught:

```bash
cat > invalid_char.ac <<'EOF'
solve {
  @
}
EOF
timeout 3 ./cp invalid_char.ac
```

Expected output includes `invalid character @`, includes `Rejected`, exits `1`, and does not time out.

Test stdin input:

```bash
printf 'solve {\n  print(7);\n}\n' | ./cp
```

Expected output: exactly `Accepted` and exit code `0`.

## Notes

This is a CLI-only project, so screen recordings are usually not useful. Prefer attaching command output in the test report.