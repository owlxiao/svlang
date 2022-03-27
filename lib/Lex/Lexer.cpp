
#include <iostream>

#include <llvm/Support/Compiler.h>

#include "Basic/CharInfo.h"
#include "Lex/Lexer.h"

namespace svlang {

Lexer::Lexer(llvm::SourceMgr &SM) {
  unsigned int MainFileID = SM.getMainFileID();
  BufferStart = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferPtr = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferEnd = SM.getMemoryBuffer(MainFileID)->getBufferEnd();
}

//
// Helper functions

void Lexer::SkipWhiteSpace(Token &Result, const char *CurPtr) {
  unsigned char Char = *CurPtr;

  while (true) {
    // Skip Horizontal Whitespace:
    // ' ', '\t', '\f', 'v'
    while (clang::isHorizontalWhitespace(Char)) {
      Char = *++CurPtr;
    }

    // Otherwise we see:
    // '\n', '\r'
    if (!clang::isVerticalWhitespace(Char)) {
      break;
    }

    Char = *++CurPtr;
  }

  BufferPtr = CurPtr;
}

// End - Helper functions
//

inline char Lexer::advanceChar(const char *Ptr, unsigned int &Size) {
  ++Size;
  return *Ptr;
}

inline char Lexer::advance(const char *&Ptr) {
  unsigned int Size = 0;
  char C = advanceChar(Ptr, Size);
  Ptr += Size;
  return C;
}

const char *Lexer::ConsumeChar(const char *Ptr, unsigned int Size) {
  return Ptr + Size;
}

void Lexer::FormToken(Token &Reuslt, const char *TokEnd, tok::TokenKind Kind) {
  unsigned int TokLen = TokEnd - BufferPtr;
  Reuslt.setLength(TokLen);
  Reuslt.setKind(Kind);
  BufferPtr = TokEnd;
}

bool Lexer::Lex(Token &Result) {
LexNextToken:
  // New Token
  Result.startToken();
  Result.setPtrData(nullptr);

  // Cache BufferPtr
  const char *CurPtr = BufferPtr;

  // Clean horizontal whitespace
  if (clang::isHorizontalWhitespace(*CurPtr)) {
    do {
      ++CurPtr;
    } while (clang::isHorizontalWhitespace(*CurPtr));

    BufferPtr = CurPtr;
  }

  char Char = advance(CurPtr);
  tok::TokenKind Kind;

  switch (Char) {
  case 0:
    if (CurPtr - 1 == BufferEnd) {
      FormToken(Result, CurPtr, tok::_EOF);
    }

    return true;

  //
  // handle trivial token
  case '\r':
    if (*CurPtr == '\n') {
      (void)advance(CurPtr);
    }
    LLVM_FALLTHROUGH;
  case '\n':
    SkipWhiteSpace(Result, CurPtr);
    // Try again
    goto LexNextToken;

  case ' ':
  case '\t':
  case '\f':
  case '\v':
    SkipWhiteSpace(Result, CurPtr);
    CurPtr = BufferPtr;

    goto LexNextToken;
    // End - handle trivial token
    //

  default:
    if (clang::isASCII(Char)) {
      Kind = tok::_UNKNOWN;
      std::cout << Char;
      break;
    }
  }

  FormToken(Result, CurPtr, Kind);

  return true;
}

} // namespace svlang
