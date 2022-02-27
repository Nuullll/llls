#ifndef LLLS_LSP_PROTOCOL_H
#define LLLS_LSP_PROTOCOL_H

#include "llvm/ADT/Optional.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/JSON.h"

#define DECLARE_JSON_INTERFACES(Class)                                         \
  Value toJSON(const Class &);                                                 \
  bool fromJSON(const Value &, Class &, Path);

namespace llls {
namespace lsp {

enum class ErrorCode {
  // Defined by JSON RPC.
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603,

  ServerNotInitialized = -32002,
  UnknownErrorCode = -32001,

  // Defined by the protocol.
  RequestCancelled = -32800,
  ContentModified = -32801,
};
// Models an LSP error as an llvm::Error.
class LSPError : public llvm::ErrorInfo<LSPError> {
public:
  std::string Message;
  ErrorCode Code;
  static char ID;

  LSPError(std::string Message, ErrorCode Code)
      : Message(std::move(Message)), Code(Code) {}

  void log(llvm::raw_ostream &OS) const override {
    OS << int(Code) << ": " << Message;
  }
  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }
};

// interface Message {
// 	jsonrpc: string;
// }
struct Message {
  std::string jsonrpc;
};

// interface RequestMessage extends Message {
//
// 	/**
// 	 * The request id.
// 	 */
// 	id: integer | string;
//
// 	/**
// 	 * The method to be invoked.
// 	 */
// 	method: string;
//
// 	/**
// 	 * The method's params.
// 	 */
// 	params?: array | object;
// }
struct RequestMessage : public Message {
  llvm::json::Value id = nullptr;
  std::string method;
  llvm::Optional<llvm::json::Value> params;
};

// interface ResponseError {
// 	/**
// 	 * A number indicating the error type that occurred.
// 	 */
// 	code: integer;
//
// 	/**
// 	 * A string providing a short description of the error.
// 	 */
// 	message: string;
//
// 	/**
// 	 * A primitive or structured value that contains additional
// 	 * information about the error. Can be omitted.
// 	 */
// 	data?: string | number | boolean | array | object | null;
// }
struct ResponseError {
  ErrorCode code;
  std::string message;
  llvm::Optional<llvm::json::Value> data;
};

// interface ResponseMessage extends Message {
// 	/**
// 	 * The request id.
// 	 */
// 	id: integer | string | null;
//
// 	/**
// 	 * The result of a request. This member is REQUIRED on success.
// 	 * This member MUST NOT exist if there was an error invoking the method.
// 	 */
// 	result?: string | number | boolean | object | null;
//
// 	/**
// 	 * The error object in case a request fails.
// 	 */
// 	error?: ResponseError;
// }
struct ResponseMessage : public Message {
  llvm::json::Value id = nullptr;
  llvm::Optional<llvm::json::Value> result;
  llvm::Optional<ResponseError> error;
};

// interface NotificationMessage extends Message {
// 	/**
// 	 * The method to be invoked.
// 	 */
// 	method: string;
//
// 	/**
// 	 * The notification's params.
// 	 */
// 	params?: array | object;
// }
struct NotificationMessage : public Message {
  std::string method;
  llvm::json::Value params;
};

} // namespace lsp
} // namespace llls

namespace llvm {
namespace json {
DECLARE_JSON_INTERFACES(llls::lsp::Message)
DECLARE_JSON_INTERFACES(llls::lsp::RequestMessage)
DECLARE_JSON_INTERFACES(llls::lsp::ResponseError)
DECLARE_JSON_INTERFACES(llls::lsp::ResponseMessage)
DECLARE_JSON_INTERFACES(llls::lsp::NotificationMessage)
} // namespace json
} // namespace llvm

#endif // LLLS_LSP_PROTOCOL_H
