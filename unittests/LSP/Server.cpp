#include "Server.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include "gtest/gtest.h"

using namespace llvm::json;
using namespace llls::lsp;

class LSPServerTest : public testing::Test {
protected:
  void dispatch(llvm::StringRef JSONStr) {
    auto V = parse(JSONStr);
    if (auto E = V.takeError()) {
      llvm::errs() << E;
      FAIL();
    }
    S.dispatch(*V);
  }

  Server S;
};

TEST_F(LSPServerTest, OnInitialize) {
  std::string Input =
      R"({"json-rpc": "2.0", "id": 42, "method": "initialize"})";
  ASSERT_NO_FATAL_FAILURE(dispatch(Input));
}
