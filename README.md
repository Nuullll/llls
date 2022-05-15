# LLVM Language Server (WIP)

A language server conformant to the [Language Server Protocol](https://microsoft.github.io/language-server-protocol/), providing language features like go to definition, find all references, hover information, etc., for LLVM assembly language.

## Design

### LSP Server

The main server receiving and sending JSON messages over `JSONTransport`.

Supported LSP messages:

- [x] [Initialize Request](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#initialize)
- [x] [Initialized Notification](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#initialized)

### LSP Transport

Mostly stolen from [clangd design: Entry point and JSON-RPC](https://clangd.llvm.org/design/code#entry-point-and-json-rpc).

- `JSONTransport` speaks JSON-RPC protocol over stdin/stdout

## Build

```bash
mkdir build
cd build
cmake -G Ninja -DLLVM_DIR=$LLVM_BUILD_DIR/lib/cmake/llvm ..
ninja
```
