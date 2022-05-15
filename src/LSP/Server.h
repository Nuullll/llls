#ifndef LLLS_LSP_SERVER_H
#define LLLS_LSP_SERVER_H

#include "Transport.h"
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
  using RequestHandlerTy = std::function<void(
      Server &S, const llvm::json::Object *, llvm::json::Value &)>;
  using NotificationHandlerTy =
      std::function<void(Server &S, const llvm::json::Object *)>;

public:
  Server(std::unique_ptr<JSONTransport> T)
      : Transport(std::move(T)), Info{"llls", "1.0"} {
    initializeDispatchTables();
  }
  const ServerInfo &getInfo() const { return Info; }
  void start(int AutoStop = 0);

private:
  void initializeDispatchTables();
  llvm::Optional<llvm::json::Value> dispatch(const llvm::json::Value &Message);

private:
  std::unique_ptr<JSONTransport> Transport;
  const ServerInfo Info;

  llvm::StringMap<RequestHandlerTy> RequestDispatchTable;
  llvm::StringMap<NotificationHandlerTy> NotificationDispatchTable;
};

} // namespace lsp
} // namespace llls

#endif // LLLS_LSP_SERVER_H
