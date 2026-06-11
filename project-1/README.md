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

## Format

```bash
make format
```

Formats `parser.cc` and `parser.h` with clang-format (requires `brew install clang-format`).

## Debug

From the repo root in Cursor:

1. Install the **CodeLLDB** extension (`vadimcn.vscode-lldb`) if prompted. Microsoft's C/C++ debugger is not available on Cursor for Apple Silicon; CodeLLDB uses the native `lldb` debugger instead.
2. Set breakpoints in `parser.cc` (click in the gutter next to a line number).
3. Open **Run and Debug** (`Cmd+Shift+D`) and choose:
   - **Debug project-1** — runs with `Task_2/t1.txt` as input
   - **Debug project-1 (pick test file)** — choose a test file before launching
4. Press **F5** to build (with debug symbols) and start debugging.

Debug builds use `make debug` (`-g -O0`). Use `cerr` for debug prints so they don't interfere with `make test` output.

**Breakpoints not hitting?** The debug launch always rebuilds with `-g`. If a breakpoint shows as hollow/unverified, press F5 again so the pre-launch task recompiles. You can also run `make debug` manually in `project-1/` first.

## Source

- `provided_code/parser.cc` — main file to implement
- `provided_code/lexer.cc`, `inputbuf.cc` — provided, do not modify
