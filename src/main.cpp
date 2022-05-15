#include "LSP/Server.h"
#include "LSP/Transport.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Program.h"

using namespace llls::lsp;

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "LLVM assembly language server\n");
  llvm::sys::ChangeStdinToBinary();
  auto T = std::make_unique<JSONTransport>(stdin, llvm::outs(), &llvm::errs());
  Server S(std::move(T));
  S.start();
  return 0;
}
