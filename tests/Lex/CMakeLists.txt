set(LLVM_LINK_COMPONENTS
    Support
)

add_svlang_unittest(LexTests
    LexerTest.cpp
)

target_link_libraries(LexTests
    PRIVATE
    svlangLex
)
