# LLVM Language Server (WIP)

A language server conformant to the [Language Server Protocol](https://microsoft.github.io/language-server-protocol/), providing language features like go to definition, find all references, hover information, etc. for LLVM assembly language.

## Design

### LSP Protocol (WIP)

- Defines C++ structs reflecting LSP structs' semantics
- Implements JSON serialization/deserialization

### LSP Transport (WIP)

Mostly stolen from [clangd design: Entry point and JSON-RPC](https://clangd.llvm.org/design/code#entry-point-and-json-rpc).

- `JSONTransport` speaks JSON-RPC protocol over stdin

## Unittests (WIP)

- LSP json serialization/deserialization
  - [x] Messages
- JSON transport
  - [x] Mirroring input requests

## Build

```bash
mkdir build
cd build
cmake -G Ninja -DLLVM_DIR=$LLVM_BUILD_DIR/lib/cmake/llvm ..
ninja
```
