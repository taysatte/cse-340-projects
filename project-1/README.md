# CSE 340 Project 1

A simple compiler for a small programming language. Parses input, checks for syntax/semantic errors, and runs programs when requested.

## Build

```bash
make
```

Produces `a.out`.

## Test

```bash
make test
```

Runs the provided test cases in `provided_tests/`.

## Clean

```bash
make clean
```

## Source

- `provided_code/parser.cc` — main file to implement
- `provided_code/lexer.cc`, `inputbuf.cc` — provided, do not modify
