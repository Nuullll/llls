#include "Protocol.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/JSON.h"

using namespace llls::lsp;

namespace llvm {
namespace json {

// A thin wrapper for json::Value so that ObjectMapper can handle json::Value.
bool fromJSON(const Value &E, Value &Out, Path P) {
  Out = E;
  return true;
}

// A thin wrapper for Optional<json::Value> so that ObjectMapper can handle
// Optional<json::Value>.
bool fromJSON(const Value &E, Optional<Value> &Out, Path P) {
  if (E.getAsNull()) {
    Out = None;
    return true;
  }
  Value Result = nullptr;
  if (!fromJSON(E, Result, P))
    return false;
  Out = std::move(Result);
  return true;
}

Value toJSON(const Message &M) { return Object{{"jsonrpc", M.jsonrpc}}; }
bool fromJSON(const Value &Params, Message &M, Path P) {
  ObjectMapper O(Params, P);
  return O && O.map("jsonrpc", M.jsonrpc) && M.jsonrpc == "2.0";
}

Value toJSON(const RequestMessage &M) {
  auto V = toJSON(static_cast<const Message &>(M));
  V.getAsObject()->try_emplace("id", M.id);
  V.getAsObject()->try_emplace("method", M.method);
  if (M.params)
    V.getAsObject()->try_emplace("params", *M.params);
  return V;
}
bool fromJSON(const Value &Params, RequestMessage &M, Path P) {
  if (!fromJSON(Params, static_cast<Message &>(M), P))
    return false;
  ObjectMapper O(Params, P);
  return O && O.map("id", M.id) && O.map("method", M.method) &&
         O.map("params", M.params);
}

Value toJSON(const ResponseError &E) {
  auto O = Object{{"code", static_cast<int>(E.code)}, {"message", E.message}};
  if (E.data)
    O.try_emplace("data", *E.data);
  return O;
}
bool fromJSON(const Value &Params, ResponseError &E, Path P) {
  ObjectMapper O(Params, P);
  int Code = 0;
  if (!O || !O.map("code", Code))
    return false;
  E.code = static_cast<ErrorCode>(Code);
  return O.map("message", E.message) && O.map("data", E.data);
}

Value toJSON(const ResponseMessage &M) {
  auto V = toJSON(static_cast<const Message &>(M));
  V.getAsObject()->try_emplace("id", M.id);
  if (M.result)
    V.getAsObject()->try_emplace("result", M.result);
  if (M.error)
    V.getAsObject()->try_emplace("error", toJSON(*M.error));
  return V;
}
bool fromJSON(const Value &Params, ResponseMessage &M, Path P) {
  if (!fromJSON(Params, static_cast<Message &>(M), P))
    return false;
  ObjectMapper O(Params, P);
  return O && O.map("id", M.id) && O.map("result", M.result) &&
         O.map("error", M.error);
}

} // namespace json
} // namespace llvm
