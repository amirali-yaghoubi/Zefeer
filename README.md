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

## Syntax
### Variables
variables are declared using `let`, only integer supported:
```zefeer
let x : int = 10;
int y : int = 2;
```
note: Declaration without initializing is not supported, if don't want to assign with anything, you must initialize with zero.

variables can be assigned/reassigned:
```zefeer
x = 65;
```

### Expressions
Zefeer supports arithmetic expressions:
```zefeer
let x : int = 10 - 3;
let y : int = (x - 3 * 2) / 2 + x;
let z : int = x * x - y;
```

Supported operands:
- `+` addition
- `-` subtraction
- `*` multiplication
- `/` division


### Conditions:
Use `if` and `else` for conditional execution:
```zefeer
if (x >= 30) {
    print(x);
} else {
    print(0);
}
```

Supported comparison operators:
- `==` equal
- `!=` not equal
- `<` less than
- `>` greater than
- `<=` less than or equal
- `>=` greater than or equal

### While loops:
Use `while` for loops:
```zefeer
let c : int = 5;

while (x > 0) {
    print(x);
    x =  x - 1;
}
```

### Printing
Values can printed using `print`:
```zefeer
let x : int = 6;
print(x);
```



## How to run (the straightforward and standard way)(Linux)
1. ```make main```
2. ```./build/zfc YOUR_SOURCE_CODE_NAME.zf```

## Running on non-ARM64 systems
Zefeer generates ARM64 assembly.
On x86_64 Linux, you can use QEMU:

### Installing the dependencies
```bash
sudo apt install gcc-aarch64-linux-gnu qemu-user
```

### Running
1.
```bash
aarch64-linux-gnu-gcc <ASSEMBLY_FILE_NAME>.s -o <OUTPUT_FILE_NAME>
```
2.
```bash
qemu-aarch64 ./<OUTPUT_FILE_NAME>
```

## Build
Ubuntu / Debian:
### Build all(main + tests)
```bash
make
```
Or
```bash
make all
```

### Build the compiler alone
```bash
make main
```

### Build all tests
```bash
make tests
```

#### Build each test individually
```bash
make lexer_test
```
```bash
make parser_test
```
```bash
make sa_test
```
```bash
make ir_test
```
```bash
make arm64_test
```

### Clean(remove) the build directory
```bash
make clean
```

### Clean + Re-build all
```bash
make re
```

## Run
### The Compiler(zfc)
```bash
./build/zfc <YOUR_SOURCE_CODE_NAME>.zf
```
Or
```bash
./build/zfc <YOUR_SOURCE_CODE_NAME>.zf -o <OUTPUT_FILE_NAME>.s
```
Or
```bash
./build/zfc -o <OUTPUT_FILE_NAME>.s <YOUR_SOURCE_CODE_NAME>.zf
```

### The tests
```bash
./build/lexer_test
```
```bash
./build/parser_test
```
```bash
./build/semantic_analyzer_test
```
```bash
./build/ir_test
```
```bash
./build/arm64_test
```

## Development Environment

- **OS:** Ubuntu 24.04 (x86_64)[WSL2]
- **Build System:** GNU Make

---

## Development History

This project was initially developed entirely on a **Xiaomi Poco X3 Pro** using **Termux** with `proot-distro` (Debian 12). All of the lexer and early parser work was written and tested on a phone using a virtual keyboard. 

The project is now maintained on a Lenovo IdeaPad slim 3 15IRH10 (83K100VDIN) laptop, but the ARM64 code generation target remains a key goal to honor its mobile origins.
