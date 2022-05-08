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
    R = S.dispatch(*V);
  }

  void getResponseResult(Object &Result) {
    if (!R)
      FAIL();
    auto *Obj = R->getAsObject();
    if (!Obj)
      FAIL();
    Obj = Obj->getObject("result");
    if (!Obj)
      FAIL();
    Result = *Obj;
  }

  Server S;
  llvm::Optional<Value> R;
};

TEST_F(LSPServerTest, OnInitialize) {
  std::string Input =
      R"({"json-rpc": "2.0", "id": 42, "method": "initialize"})";
  ASSERT_NO_FATAL_FAILURE(dispatch(Input));
  Object Obj;
  getResponseResult(Obj);
  auto *Info = Obj.getObject("serverInfo");
  ASSERT_TRUE(Info);
  ASSERT_TRUE(Info->getString("name")->equals("llls"));
}
