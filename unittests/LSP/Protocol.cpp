#include "Protocol.h"
#include "llvm/Support/JSON.h"
#include "gtest/gtest.h"

using namespace llvm::json;
using namespace llls::lsp;

#define FAIL_ON_ERROR(Expected)                                                \
  if (auto E = Expected.takeError()) {                                         \
    llvm::errs() << E;                                                         \
    FAIL();                                                                    \
  }

class LSPProtocolJSONTest : public testing::Test {
protected:
  Path::Root Root;
};

TEST_F(LSPProtocolJSONTest, Message) {
  Message M;
  auto V = parse(R"({"jsonrpc":"2.0"})");
  FAIL_ON_ERROR(V);
  ASSERT_TRUE(fromJSON(*V, M, Root));
  ASSERT_EQ(toJSON(M), *V);
}

TEST_F(LSPProtocolJSONTest, Message_Neg) {
  Message M;
  auto V = parse(R"({"jsonrpc":"!=2.0"})");
  FAIL_ON_ERROR(V);
  ASSERT_FALSE(fromJSON(*V, M, Root));
}

TEST_F(LSPProtocolJSONTest, RequestMessage) {
  RequestMessage M;
  auto V = parse(R"({
    "jsonrpc":"2.0",
    "id":42,
    "method":"TestMethod",
    "params":["ABC", 123]
  })");
  FAIL_ON_ERROR(V);
  ASSERT_TRUE(fromJSON(*V, M, Root));
  ASSERT_EQ(toJSON(M), *V);
}

TEST_F(LSPProtocolJSONTest, ResponseError) {
  ResponseError E;
  auto V = parse(R"({
    "code":-32700,
    "message":"something's wrong",
    "data":404
  })");
  FAIL_ON_ERROR(V);
  ASSERT_TRUE(fromJSON(*V, E, Root));
  ASSERT_EQ(toJSON(E), *V);
}

TEST_F(LSPProtocolJSONTest, ResponseMessage) {
  ResponseMessage M;
  auto V = parse(R"({
    "jsonrpc":"2.0",
    "id":42,
    "result":{"message":"fail"},
    "error":{
      "code":-32700,
      "message":"something's wrong",
      "data":404
    }
  })");
  FAIL_ON_ERROR(V);
  ASSERT_TRUE(fromJSON(*V, M, Root));
  ASSERT_EQ(toJSON(M), *V);
}