#ifndef LLLS_LSP_SERVER_H
#define LLLS_LSP_SERVER_H

#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/JSON.h"
#include <functional>

namespace llls {
namespace lsp {

class Server {
  using RequestHandlerTy =
      std::function<void(const llvm::json::Object *)>;
  using NotificationHandlerTy =
      std::function<void(const llvm::json::Object *)>;

public:
  Server();
  void dispatch(const llvm::json::Value &Message);

private:
  void initializeDispatchMap();

private:
  llvm::StringMap<RequestHandlerTy> DispatchMap;
};

} // namespace lsp
} // namespace llls

#endif // LLLS_LSP_SERVER_H
