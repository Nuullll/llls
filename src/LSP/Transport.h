#ifndef LLLS_LSP_TRANSPORT_H
#define LLLS_LSP_TRANSPORT_H

#include "Server.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

namespace llls {
namespace lsp {

enum JSONStreamStyle { Standard, Delimited };

class JSONTransport {
  std::FILE *In;
  llvm::raw_ostream &Mirror;
  const JSONStreamStyle Style;
  llvm::SmallString<128> Buffer;
  Server LSPServer;

public:
  JSONTransport(std::FILE *In, llvm::raw_ostream *Mirror = nullptr,
                JSONStreamStyle Style = Standard)
      : In(In), Mirror(Mirror ? *Mirror : llvm::nulls()), Style(Style) {}

  void run(int AutoStopThreshold = 0);

private:
  // Typical standard message (defined by LSP):
  // Content-Length: xxx
  //
  // {"jsonrpc": "2.0", "id": 42}
  bool readStandardJSONMessage(llvm::json::Value &Data);

  // Typical delimited message (for tests):
  // ---
  // {
  //    "jsonrpc": "2.0",
  //    "id": 42
  // }
  // ---
  bool readDelimitedJSONMessage(llvm::json::Value &Data);

  void mirrorInput(const llvm::SmallVectorImpl<char> &Line) {
    Mirror << Line << '\n';
    Mirror.flush();
  }
};

} // namespace lsp
} // namespace llls

#endif // LLLS_LSP_TRANSPORT_H
