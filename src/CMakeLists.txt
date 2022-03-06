add_subdirectory(LSP)

add_executable(LLVMLanguageServer
  main.cpp
)

target_link_libraries(LLVMLanguageServer
  PRIVATE
  LLVMSupport
  LSP
)
