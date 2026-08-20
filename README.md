# myLang

## Overview
A small C-like compiled language written in C for learing Compiler design as a hobby

## Status
Early development(front end). Not stable.

## Goals
- Simple Lexer + Parser
- ARM64 Code generation
- Minimal C-like syntax

## Current progress
- Lexer is done.
- A linked-block arena allocator implemented
- Parser in progress (Statement parser + block parser imlemented)

## Build
(to be added)

## Development Environment

- **OS:** Debian 12 (x86_64)
- **Build System:** GNU Make

---

## Development History

This project was initially developed entirely on a **Xiaomi Poco X3 Pro** using **Termux** with `proot-distro` (Debian 12). All of the lexer and early parser work was written and tested on a phone using a virtual keyboard. 

The project is now maintained on a Lenovo IdeaPad slim 3 15IRH10 (83K100VDIN) laptop, but the ARM64 code generation target remains a key goal to honor its mobile origins.
