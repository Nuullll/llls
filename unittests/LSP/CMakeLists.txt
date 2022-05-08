add_unittest(LLLSUnitTests LSPTests
  Protocol.cpp
  Server.cpp
  Transport.cpp
)

target_link_libraries(LSPTests
  PRIVATE
  LSP
)

get_target_property(LSP_SOURCE_DIR LSP SOURCE_DIR)
target_include_directories(LSPTests
  PRIVATE
  ${LSP_SOURCE_DIR}
)
