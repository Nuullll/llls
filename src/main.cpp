#include "LSP/Transport.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Program.h"

using namespace llls::lsp;

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "llvm assembly language server\n");
  llvm::sys::ChangeStdinToBinary();
  return 0;
}
