# myLang

## Overview
A small C-like compiled language written in C for learing Compiler design as a hobby

## Status
Mid development. Not stable.

## Goals
- Simple Lexer + Parser
- ARM64 Code generation
- Minimal C-like syntax (integer declaration and assignment, if/else, while loop)

## Current progress
- Lexer is done
- A linked-block arena allocator implemented
- Parser is done
- Semantic Analyzer in progress

## Build
(to be added)

## Development Environment

- **OS:** Ubuntu 24.04 (x86_64)
- **Build System:** GNU Make

---

## Development History

This project was initially developed entirely on a **Xiaomi Poco X3 Pro** using **Termux** with `proot-distro` (Debian 12). All of the lexer and early parser work was written and tested on a phone using a virtual keyboard. 

The project is now maintained on a Lenovo IdeaPad slim 3 15IRH10 (83K100VDIN) laptop, but the ARM64 code generation target remains a key goal to honor its mobile origins.
