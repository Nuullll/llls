#ifndef LLLS_LSP_SERVER_H
#define LLLS_LSP_SERVER_H

#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/JSON.h"
#include <functional>

namespace llls {
namespace lsp {

struct ServerInfo {
  std::string Name;
  std::string Version;

  auto toJSON() const {
    return llvm::json::Object{{"name", Name}, {"version", Version}};
  }
};

class Server {
  using RequestHandlerTy =
      std::function<void(Server &S, const llvm::json::Object *, llvm::json::Value &)>;
  using NotificationHandlerTy = std::function<void(Server &S, const llvm::json::Object *)>;

public:
  Server() : Info{"llls", "1.0"} {
    initializeDispatchTables();
  }
  llvm::Optional<llvm::json::Value> dispatch(const llvm::json::Value &Message);
  const ServerInfo &getInfo() const { return Info; }

private:
  void initializeDispatchTables();

private:
  llvm::StringMap<RequestHandlerTy> RequestDispatchTable;
  llvm::StringMap<NotificationHandlerTy> NotificationDispatchTable;
  const ServerInfo Info;
};

} // namespace lsp
} // namespace llls

#endif // LLLS_LSP_SERVER_H
