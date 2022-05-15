#include "Transport.h"
#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "llls-lsp-transport"

using namespace llvm;
using namespace llls::lsp;

// Tries to read a line up to and including \n.
static bool readLine(std::FILE *In, SmallVectorImpl<char> &Out,
                     raw_ostream &Mirror) {
  LLVM_DEBUG(dbgs() << "Reading line\n");
  static constexpr int BufSize = 128;
  size_t Size = 0;
  Out.clear();
  while (!feof(In) && !ferror(In)) {
    Out.resize_for_overwrite(Size + BufSize);
    std::fgets(&Out[Size], BufSize, In);
    Mirror << StringRef(&Out[Size]);
    LLVM_DEBUG(dbgs().indent(2) << "got: " << StringRef(&Out[Size]) << '\n');
    size_t Read = std::strlen(&Out[Size]);
    if (Read > 0 && Out[Size + Read - 1] == '\n') {
      Out.resize(Size + Read);
      return true;
    }
    Size += Read;
  }
  return false;
}

static bool readNBytes(std::FILE *In, unsigned N, SmallVectorImpl<char> &Out,
                       raw_ostream &Mirror) {
  LLVM_DEBUG(dbgs() << "Reading " << N << " bytes\n");
  Out.clear();
  Out.resize_for_overwrite(N);
  std::fread(&Out[0], 1, N, In);
  Mirror << Out;
  LLVM_DEBUG(dbgs().indent(2) << "got: " << StringRef(&Out[0]) << '\n');
  return true;
}

void JSONTransport::writeJSONMessage(const json::Value &Message) {
  Out << "Content-Length: ";
  std::string S;
  raw_string_ostream SS(S);
  SS << Message;
  Out << S.size() << "\r\n\r\n" << S;
  Out.flush();
  LLVM_DEBUG(dbgs() << "Wrote " << S.size() << " bytes JSON to stdout:\n"
                    << Message << '\n');
}

bool JSONTransport::readStandardJSONMessage(json::Value &Data) {
  // Parse header.
  if (!readLine(In, Buffer, Mirror))
    return false;
  unsigned ContentLength = 0;
  StringRef S(Buffer);
  if (!S.consume_front("Content-Length: ") ||
      S.consumeInteger(10, ContentLength))
    return false;
  // Skip the empty line.
  if (!readLine(In, Buffer, Mirror))
    return false;
  // Read JSON data bytes.
  if (!readNBytes(In, ContentLength, Buffer, Mirror))
    return false;
  auto J = json::parse(Buffer);
  if (auto E = J.takeError()) {
    LLVM_DEBUG(dbgs() << E);
    consumeError(std::move(E));
    return false;
  }
  Data = *J;
  return true;
}

bool JSONTransport::readDelimitedJSONMessage(json::Value &Data) {
  static const StringRef Delimiter = "---";
  // Parse delimiter.
  if (!readLine(In, Buffer, Mirror) || !Buffer.startswith(Delimiter))
    return false;
  SmallString<128> JSONStr;
  while (readLine(In, Buffer, Mirror) && !Buffer.startswith(Delimiter))
    JSONStr.append(std::string(Buffer));
  auto J = json::parse(JSONStr);
  if (auto E = J.takeError()) {
    LLVM_DEBUG(dbgs() << E);
    consumeError(std::move(E));
    return false;
  }
  Data = *J;
  return true;
}
