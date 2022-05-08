add_library(LSP
  Server.cpp
  Transport.cpp
)

target_include_directories(LSP
  PRIVATE
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(LSP
  LLVMSupport
)
