#include "Transport.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include "gtest/gtest.h"

using namespace llls::lsp;

class LSPTransportEchoTest : public testing::Test {
  std::string InBuf, MirrorBuf;
  llvm::raw_string_ostream Mirror;
  std::unique_ptr<FILE, int (*)(FILE *)> In;

protected:
  LSPTransportEchoTest() : Mirror(MirrorBuf), In(nullptr, nullptr) {}

  std::unique_ptr<JSONTransport> transport(std::string InData,
                                           JSONStreamStyle Style) {
    InBuf = std::move(InData);
    In = {fmemopen(&InBuf[0], InBuf.size(), "r"), &fclose};
    return std::make_unique<JSONTransport>(In.get(), &Mirror, Style);
  }

  std::string input() const { return InBuf; }
  std::string inputMirror() { return Mirror.str(); }
};

TEST_F(LSPTransportEchoTest, StandardJSON) {
  std::string Input = R"(Content-Length: 53

{"json-rpc": "2.0", "id": 42, "method": "initialize"})";
  auto T = transport(Input, JSONStreamStyle::Standard);
  T->run(1);
  ASSERT_EQ(Input, inputMirror());
}

TEST_F(LSPTransportEchoTest, DelimitedJSON) {
  std::string Input = R"(---
{"json-rpc": "2.0", "id": 42}
---)";
  auto T = transport(Input, JSONStreamStyle::Delimited);
  T->run(1);
  ASSERT_EQ(Input, inputMirror());
}
