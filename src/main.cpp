#include "LSP/Transport.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Program.h"

using namespace llls::lsp;

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "LLVM assembly language server\n");
  llvm::sys::ChangeStdinToBinary();
  llvm::outs() << "llls started\n";
  auto T = std::make_unique<JSONTransport>(stdin, &llvm::outs());
  T->run();
  return 0;
}
