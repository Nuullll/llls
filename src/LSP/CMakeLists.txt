add_library(LSP
  Protocol.cpp
  Transport.cpp
)

target_include_directories(LSP
  PRIVATE
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(LSP
  LLVMSupport
)
