#include "Server.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/JSON.h"

#define DEBUG_TYPE "llls-lsp-server"

using namespace llvm;
using namespace llls::lsp;

namespace MessageHandler {

static void on_initialize(const json::Object *Message) { return; }

} // namespace MessageHandler

Server::Server() { initializeDispatchMap(); }

void Server::dispatch(const json::Value &Message) {
  LLVM_DEBUG(dbgs() << "Dispatching message:\n" << Message << '\n');
  auto *Obj = Message.getAsObject();
  assert(Obj && "JSON object received by LSPServer is invalid!");
  auto Method = Obj->getString("method");
  if (Method) {
    LLVM_DEBUG(dbgs() << "Dispatching to on_" << *Method << "()\n");
    auto It = DispatchMap.find(*Method);
    if (It == DispatchMap.end()) {
      errs() << "Method handler for '" << *Method
             << "' is not binded/implemented!";
      llvm_unreachable("Not found in dispatch map.");
    }
    It->second(Obj);
  }
}

void Server::initializeDispatchMap() {
#define BIND(method) DispatchMap[#method] = &MessageHandler::on_##method
  BIND(initialize);
#undef BIND
}