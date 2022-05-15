#include "Transport.h"
#include "TestCommon.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include "gtest/gtest.h"

using namespace llls::lsp;

class LSPTransportEchoTest : public LSPTransportTest {
  std::string MirrorBuf;
  llvm::raw_string_ostream Mirror;

protected:
  LSPTransportEchoTest() : Mirror(MirrorBuf) {}

  std::unique_ptr<JSONTransport> makeTransport(std::string InData,
                                               JSONStreamStyle Style) {
    return LSPTransportTest::makeTransport(InData, Style, llvm::outs(),
                                           &Mirror);
  }

  std::string inputMirror() { return Mirror.str(); }
};

TEST_F(LSPTransportEchoTest, StandardJSON) {
  std::string Input = R"(Content-Length: 53

{"json-rpc": "2.0", "id": 42, "method": "initialize"})";
  auto T = makeTransport(Input, JSONStreamStyle::Standard);
  llvm::json::Value Data = nullptr;
  T->readJSONMessage(Data);
  ASSERT_EQ(Input, inputMirror());
}

TEST_F(LSPTransportEchoTest, DelimitedJSON) {
  std::string Input = R"(---
{"json-rpc": "2.0", "id": 42}
---)";
  auto T = makeTransport(Input, JSONStreamStyle::Delimited);
  llvm::json::Value Data = nullptr;
  T->readJSONMessage(Data);
  ASSERT_EQ(Input, inputMirror());
}
