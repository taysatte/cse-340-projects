# CSE 340 Projects

Coursework for CSE 340 (Programming Languages) at Arizona State University.

## Projects

| Project | What it does | Submit to Gradescope |
|---------|--------------|----------------------|
| [project-1](project-1/) | Parser and semantic analyzer for a small language | `parser.cc`, `parser.h` |
| [project-2](project-2/) | Grammar analysis (Nullable, FIRST/FOLLOW, transforms) | `project2.cc` only |

Each project has its own README with a full overview, build/test commands, and submission details.

## Quick start

```bash
cd project-1 && make && make test
cd project-2 && make && make test-all
```

## Repository layout

```
cse-340-projects/
├── project-1/
│   ├── docs/            # Spec and implementation guide
│   ├── provided_code/ # Implement parser.cc / parser.h
│   ├── provided_tests/
│   └── README.md
├── project-2/
│   ├── docs/            # Spec and implementation suggestions
│   ├── src/             # Implement project2.cc
│   ├── tests/
│   └── README.md
└── README.md
```

## Gradescope notes

- Submit **individual code files** — no zip archives.
- Provided files (`lexer`, `inputbuf`) are on Gradescope; do not submit them.
- Compile with **C++11** on **Ubuntu 22.04 / GCC** before submitting.
- Activate the submission you want graded (unlimited resubmits).
