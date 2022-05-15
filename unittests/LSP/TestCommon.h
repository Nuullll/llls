#include "Transport.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include "gtest/gtest.h"

class LSPTransportTest : public testing::Test {
  std::unique_ptr<FILE, int (*)(FILE *)> In;
  std::string InBuf;

protected:
  LSPTransportTest() : In(nullptr, nullptr) {}

  std::unique_ptr<llls::lsp::JSONTransport>
  makeTransport(std::string InData, llls::lsp::JSONStreamStyle Style,
                llvm::raw_ostream &Out, llvm::raw_ostream *Mirror = nullptr) {
    InBuf = std::move(InData);
    In = {fmemopen(&InBuf[0], InBuf.size(), "r"), &fclose};
    return std::make_unique<llls::lsp::JSONTransport>(In.get(), Out, Mirror,
                                                      Style);
  }

  std::string input() const { return InBuf; }
};
