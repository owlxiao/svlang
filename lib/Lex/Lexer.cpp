#include <iostream>

#include "Lex/Lexer.h"

namespace svlang {

Lexer::Lexer(llvm::SourceMgr &SM) {
  unsigned int MainFileID = SM.getMainFileID();
  BufferStart = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferPtr = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferEnd = SM.getMemoryBuffer(MainFileID)->getBufferEnd();
}

bool Lexer::Lex(Token &Result) {
  Result.startToken();

  const char *CurPtr = BufferPtr;

  std::cout << *CurPtr;

  BufferPtr = CurPtr + 1;

  return true;
}

} // namespace svlang
