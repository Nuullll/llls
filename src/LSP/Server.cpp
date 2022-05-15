#include "Server.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/JSON.h"

#define DEBUG_TYPE "llls-lsp-server"

using namespace llvm;
using namespace llls::lsp;

void Server::start(int AutoStop) {
  json::Value Data = nullptr;
  while (true) {
    bool OK = Transport->readJSONMessage(Data);
    auto Response = dispatch(Data);
    if (Response)
      Transport->writeJSONMessage(*Response);
    // Auto stop after receiving N messages (for test only)
    if (AutoStop > 0 && --AutoStop == 0)
      return;
  }
}

Optional<json::Value> Server::dispatch(const json::Value &Message) {
  LLVM_DEBUG(dbgs() << "Dispatching message:\n" << Message << '\n');
  auto *Obj = Message.getAsObject();
  assert(Obj && "JSON object received by LSPServer is invalid!");
  auto *ID = Obj->get("id");
  auto Method = Obj->getString("method");
  if (Method) {
    LLVM_DEBUG(dbgs() << "Dispatching to on_" << *Method << "()\n");

    auto ReqIt = RequestDispatchTable.find(*Method);
    if (ReqIt != RequestDispatchTable.end()) {
      json::Value Result = nullptr;
      ReqIt->second(*this, Obj, Result);
      return json::Value(
          json::Object({{"id", *ID}, {"result", Result}, {"jsonrpc", "2.0"}}));
    }

    auto NotIt = NotificationDispatchTable.find(*Method);
    if (NotIt != NotificationDispatchTable.end()) {
      NotIt->second(*this, Obj);
      return None;
    }

    errs() << "Method handler for '" << *Method
           << "' is not binded/implemented!\n";
  }

  return None;
}

namespace MessageHandler {

#define REQUEST_HANDLER(method)                                                \
  static void on_##method(Server &S, const json::Object *Message,              \
                          json::Value &Result)
#define NOTIFICATION_HANDLER(method)                                           \
  static void on_##method(Server &S, const json::Object *Message)

// Result: InitializeResult
// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#initializeResult
REQUEST_HANDLER(initialize) {
  const auto &Info = S.getInfo();
  Result =
      json::Object({{"capabilities", json::Object({{"hoverProvider", true}})},
                    {"serverInfo", Info.toJSON()}});
}

NOTIFICATION_HANDLER(initialized) { return; }

} // namespace MessageHandler

void Server::initializeDispatchTables() {
#define BIND(type, method)                                                     \
  this->type##DispatchTable[#method] = &MessageHandler::on_##method
#define BIND_REQUEST(method) BIND(Request, method)
#define BIND_NOTIFICATION(method) BIND(Notification, method)
  BIND_REQUEST(initialize);
  BIND_NOTIFICATION(initialized);
#undef BIND
#undef BIND_REQUEST
#undef BIND_NOTIFICATION
}
