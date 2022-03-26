set(LLVM_LINK_COMPONENTS support)

add_svlang_library(svlangLex
    Preprocessor.cpp
    Lexer.cpp
    
    LINK_LIBS
    clang
)