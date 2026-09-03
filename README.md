# Zefeer

## Overview
Zefeer is a simple C-like compiled programming language. The compiler is written entirely in C, from scratch.

## Status
Status: v0.1.0 released — stable

## Features

- Integer variables
- `if` / `else`
- `while` loop
- Semantic analysis
- Intermediate representation
- ARM64 assembly generation (Linux)

## How to run (the straightforward and standard way)(Linux)
1. ```make main```
2. ```./build/zfc YOUR_SOURCE_CODE_NAME.zf```


## Build
Ubuntu / Debian:
### Build all(main + tests)
```
make
```
Or
```
make all
```

### Build the compiler alone
```
make main
```

### Build all tests
```
make tests
```

#### Build each test individually
```
make lexer_test
```
```
make parser_test
```
```
make sa_test
```
```
make ir_test
```
```
make arm64_test
```

### Clean(remove) the build directory
```
make clean
```

### Clean + Re-build all
```
make re
```

## Run
### The Compiler(zfc)
```
./build/zfc YOUR_SOURCE_CODE_NAME.zf
```
Or
```
./build/zfc YOUR_SOURCE_CODE_NAME.zf -o OUTPUT_FILE_NAME.s
```
Or
```
./build/zfc -o OUTPUT_FILE_NAME.s YOUR_SOURCE_CODE_NAME.zf
```

### The tests
```
./build/lexer_test
```
```
./build/parser_test
```
```
./build/semantic_analyzer_test
```
```
./build/ir_test
```
```
./build/arm64_test
```

## Development Environment

- **OS:** Ubuntu 24.04 (x86_64)[WSL2]
- **Build System:** GNU Make

---

## Development History

This project was initially developed entirely on a **Xiaomi Poco X3 Pro** using **Termux** with `proot-distro` (Debian 12). All of the lexer and early parser work was written and tested on a phone using a virtual keyboard. 

The project is now maintained on a Lenovo IdeaPad slim 3 15IRH10 (83K100VDIN) laptop, but the ARM64 code generation target remains a key goal to honor its mobile origins.
