# CSE 340 Project 2

Grammar analysis tool: reads a CFG from stdin and runs one of six tasks based on a command-line argument.

## Build

```bash
make
```

Produces `a.out`.

## Test

```bash
make test TASK=1    # run tests for task 1 (default TASK=1)
make test-all       # run all tasks 1–6
```

Run a single case manually:

```bash
./a.out 3 < tests/test01.txt
```

## Tasks

| Task | Description |
|------|-------------|
| 1 | Print terminals then non-terminals (grammar appearance order) |
| 2 | Nullable non-terminals |
| 3 | FIRST sets |
| 4 | FOLLOW sets |
| 5 | Left-factor grammar |
| 6 | Eliminate left recursion |

## Debug / Format

```bash
make debug
make format
```

## Source

- `src/project2.cc` — implement here
- `src/lexer.cc`, `src/inputbuf.cc` — provided, do not modify

See `docs/` for the project specification and implementation suggestions.
