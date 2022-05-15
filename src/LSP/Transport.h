#ifndef LLLS_LSP_TRANSPORT_H
#define LLLS_LSP_TRANSPORT_H

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
  llvm::raw_ostream &Out;
  llvm::raw_ostream &Mirror;
  const JSONStreamStyle Style;
  llvm::SmallString<128> Buffer;

public:
  JSONTransport(std::FILE *In, llvm::raw_ostream &Out,
                llvm::raw_ostream *Mirror = nullptr,
                JSONStreamStyle Style = Standard)
      : In(In), Out(Out), Mirror(Mirror ? *Mirror : llvm::nulls()),
        Style(Style) {}

  bool readJSONMessage(llvm::json::Value &Data) {
    return Style == Standard ? readStandardJSONMessage(Data)
                             : readDelimitedJSONMessage(Data);
  }

  void writeJSONMessage(const llvm::json::Value &Message);

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
