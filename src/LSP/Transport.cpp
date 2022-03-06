#include "Transport.h"
#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "llls-lsp-transport"

// Tries to read a line up to and including \n.
static bool readLine(std::FILE *In, llvm::SmallVectorImpl<char> &Out,
                     llvm::raw_ostream &Mirror) {
  static constexpr int BufSize = 128;
  size_t Size = 0;
  Out.clear();
  while (!feof(In) && !ferror(In)) {
    Out.resize_for_overwrite(Size + BufSize);
    std::fgets(&Out[Size], BufSize, In);
    Mirror << llvm::StringRef(&Out[Size]);
    LLVM_DEBUG(llvm::dbgs() << "Readline: " << llvm::StringRef(&Out[Size]));
    size_t Read = std::strlen(&Out[Size]);
    if (Read > 0 && Out[Size + Read - 1] == '\n') {
      Out.resize(Size + Read);
      return true;
    }
    Size += Read;
  }
  return false;
}

static bool readNBytes(std::FILE *In, unsigned N,
                       llvm::SmallVectorImpl<char> &Out,
                       llvm::raw_ostream &Mirror) {
  Out.clear();
  Out.resize_for_overwrite(N);
  std::fread(&Out[0], 1, N, In);
  Mirror << Out;
  return true;
}

namespace llls {
namespace lsp {

void JSONTransport::run() {
  llvm::json::Value Data = nullptr;
  bool OK = Style == Standard ? readStandardJSONMessage(Data)
                              : readDelimitedJSONMessage(Data);
}

bool JSONTransport::readStandardJSONMessage(llvm::json::Value &Data) {
  // Parse header.
  if (!readLine(In, Buffer, Mirror))
    return false;
  unsigned ContentLength = 0;
  llvm::StringRef S(Buffer);
  if (!S.consume_front("Content-Length: ") ||
      S.consumeInteger(10, ContentLength))
    return false;
  // Skip the empty line.
  if (!readLine(In, Buffer, Mirror))
    return false;
  // Read JSON data bytes.
  if (!readNBytes(In, ContentLength, Buffer, Mirror))
    return false;
  auto J = llvm::json::parse(Buffer);
  if (auto E = J.takeError()) {
    LLVM_DEBUG(llvm::dbgs() << E);
    llvm::consumeError(std::move(E));
    return false;
  }
  Data = *J;
  return true;
}

bool JSONTransport::readDelimitedJSONMessage(llvm::json::Value &Data) {
  static const llvm::StringRef Delimiter = "---";
  // Parse delimiter.
  if (!readLine(In, Buffer, Mirror) || !Buffer.startswith(Delimiter))
    return false;
  llvm::SmallString<128> JSONStr;
  while (readLine(In, Buffer, Mirror) && !Buffer.startswith(Delimiter))
    JSONStr.append(std::string(Buffer));
  auto J = llvm::json::parse(JSONStr);
  if (auto E = J.takeError()) {
    LLVM_DEBUG(llvm::dbgs() << E);
    llvm::consumeError(std::move(E));
    return false;
  }
  Data = *J;
  return true;
}

} // namespace lsp
} // namespace llls
