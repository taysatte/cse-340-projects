# CSE 340 Projects

Coursework for CSE 340 (Programming Languages) at Arizona State University. Each project lives in its own directory with build instructions, tests, and any provided starter code.

## Projects

| Project | Description |
|---------|-------------|
| [project-1](project-1/) | Parser and semantic analyzer for a small programming language |
| [project-2](project-2/) | Grammar analysis: terminals/non-terminals, Nullable, FIRST/FOLLOW, left factoring, left-recursion elimination |

## Getting Started

Each project is self-contained. `cd` into a project directory and follow its README:

```bash
cd project-1
make
make test
```

```bash
cd project-2
make
make test-all
```

## Repository Structure

```
cse-340-projects/
├── project-1/
│   ├── docs/            # Project spec and implementation guide
│   ├── provided_code/   # Starter code (implement parser.cc)
│   ├── provided_tests/  # Autograder-style test cases
│   ├── Makefile
│   └── README.md
├── project-2/
│   ├── docs/            # Project spec and implementation suggestions
│   ├── src/             # Starter code (implement project2.cc)
│   ├── tests/           # Autograder-style test cases
│   ├── scripts/         # test_p2.sh
│   ├── Makefile
│   └── README.md
└── README.md
```
