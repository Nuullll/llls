#include "Server.h"
#include "TestCommon.h"
#include "Transport.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include "gtest/gtest.h"

using namespace llvm::json;
using namespace llls::lsp;

class LSPServerTest : public LSPTransportTest {
  std::string OutBuf;
  llvm::raw_string_ostream Out;

protected:
  LSPServerTest() : Out(OutBuf) {}

  std::unique_ptr<Server> makeServer(std::string InData) {
    return std::make_unique<Server>(
        std::move(makeTransport(InData, JSONStreamStyle::Standard, Out)));
  }

  std::string &output() {
    Out.flush();
    return OutBuf;
  }
};

TEST_F(LSPServerTest, OnInitialize) {
  std::string Input = R"(Content-Length: 53

{"json-rpc": "2.0", "id": 42, "method": "initialize"})";
  auto S = makeServer(Input);
  S->start(/*AutoStop*/ 1);
  ASSERT_EQ(
      output(),
      "Content-Length: 119\r\n\r\n"
      R"({"id":42,"jsonrpc":"2.0","result":{"capabilities":{"hoverProvider":true},"serverInfo":{"name":"llls","version":"1.0"}}})");
}
