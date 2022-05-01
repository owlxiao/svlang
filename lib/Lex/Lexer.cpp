#include <algorithm>
#include <iostream>
#include <string>

#include <llvm/Support/Compiler.h>

#include "Basic/CharInfo.h"
#include "Lex/Lexer.h"

namespace svlang {

Lexer::Lexer(llvm::SourceMgr &SM) {
  unsigned int MainFileID = SM.getMainFileID();
  BufferStart = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferPtr = SM.getMemoryBuffer(MainFileID)->getBufferStart();
  BufferEnd = SM.getMemoryBuffer(MainFileID)->getBufferEnd();
  lastToken.startToken();
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

void Lexer::SkipLineComment(const char *CurPtr) {
  char C;

  while (true) {
    C = *CurPtr;

    while (C != 0 &&                 // EOF
           C != '\n' && C != '\r') { // Newline
      C = *++CurPtr;
    }

    // Then we skip whitespace before newline
    const char *NextLine = CurPtr;
    if (C != 0) {
      // We found a newline, see if it is escaped
      const char *EscapePtr = CurPtr - 1;
      bool HasSpace = false;

      while (clang::isHorizontalWhitespace(*EscapePtr)) {
        --EscapePtr;
        HasSpace = true;
      }
    }

    CurPtr = NextLine;
    break;
  }

  BufferPtr = CurPtr;
}

void Lexer::SkipBlockComment(const char *CurPtr) {
  unsigned char C = getAndAdcanceChar(CurPtr);

  while (true) {
    while (CurPtr[0] != '/' && CurPtr[1] != '/' && CurPtr[2] != '/' &&
           CurPtr[3] != '/' && CurPtr + 4 < BufferEnd) {
      CurPtr += 4;
    }

    C = *CurPtr++;

    while (C != '/' && C != '\0') {
      C = *CurPtr++;
    }

    if (C == '/') {
      if (CurPtr[-2] == '*') { // Found the final */
        break;
      }
    }
  }
  BufferPtr = CurPtr;
}

// End - Helper functions
//

/*
 * Helper functions to lex a token of the specific type
 */

bool Lexer::isTimeUnit(const char *C) const {
  return (*C == 's') ||
         ((C[1] == 's') && ((C[0] == 'm') || (C[0] == 'u') || (C[0] == 'n') ||
                            (C[0] == 'p') || (C[0] == 'f')));
}

bool Lexer::lexNumericLiteral(Token &Result, const char *CurPtr) {
  unsigned Size;
  char preChar = 0;
  bool isRealNumber = false;
  bool isTimeLiteral = false;
  char C = getCharAndSize(CurPtr, Size);
  while (clang::isPreprocessingNumberBody(C)) {
    CurPtr = ConsumeChar(CurPtr, Size);
    preChar = C;
    C = getCharAndSize(CurPtr, Size);
  }
  // check for a sign under e
  // e.g. 1.30e-2
  if ((C == '-' || C == '+') && (preChar == 'E' || preChar == 'e')) {
    return lexNumericLiteral(Result, ConsumeChar(CurPtr, Size));
  }

  isRealNumber = std::find_if(BufferPtr, CurPtr, [](const char C) {
                   return (C == '.') || (C == 'e') || (C == 'E');
                 }) != CurPtr;
  isTimeLiteral = std::find_if(BufferPtr, CurPtr, [this](const char C) {
                    return isTimeUnit(&C);
                  }) != CurPtr;

  const char *TokStart = BufferPtr;
  FormToken(Result, CurPtr,
            isTimeLiteral  ? tok::_TIME_LITERAL
            : isRealNumber ? tok::_REAL_LITERAL
                           : tok::_INTEGER_LITERAL);
  Result.setLiteralData(TokStart);
  return true;
}

bool Lexer::lexBaseFormat(Token &Result, const char *CurPtr) {
  char C = getAndAdcanceChar(CurPtr);
  tok::TokenKind kind;

  switch (C) {
  case '0':
  case '1':
  case 'x':
  case 'X':
  case 'z':
  case 'Z':
  case '?':
    kind = tok::_UNBASED_UNSIZED_LITERAL;
    break;
  case 's':
  case 'S':
    return lexBaseFormat(Result, CurPtr);
  default:
    kind = tok::_INTEGER_BASE;
    break;
  }

  const char *TokStart = BufferPtr;
  FormToken(Result, CurPtr, kind);
  Result.setLiteralData(TokStart);
  return true;
}

inline char Lexer::advanceChar(const char *Ptr, unsigned int &Size) {
  ++Size;
  return *Ptr;
}

inline char Lexer::getAndAdcanceChar(const char *&Ptr) {
  unsigned Size = 0;
  char C = advanceChar(Ptr, Size);
  Ptr += Size;
  return C;
}

const char *Lexer::ConsumeChar(const char *Ptr, unsigned int Size) {
  return Ptr + Size;
}

inline char Lexer::getCharAndSize(const char *Ptr, unsigned &Size) {
  Size = 1;
  return *Ptr;
}

llvm::SMLoc Lexer::getSourceLocation(const char *Loc) const {
  return llvm::SMLoc::getFromPointer(Loc);
}

void Lexer::FormToken(Token &Result, const char *TokEnd, tok::TokenKind Kind) {
  unsigned int TokLen = TokEnd - BufferPtr;
  Result.setLength(TokLen);
  Result.setLocation(getSourceLocation(BufferPtr));
  Result.setKind(Kind);
  BufferPtr = TokEnd;
  lastToken = Result;
  // FormToken(lastToken, TokEnd, Kind);
}

std::string Lexer::getSpelling(const Token &Tok) {
  std::string Result;
  Result.resize(Tok.getLength());
  size_t length = 0;
  char *spelling = &*Result.begin();
  const char *TokStart = Tok.getLocation().getPointer();
  const char *TokEnd = TokStart + Tok.getLength();
  char *bufPtr = const_cast<char *>(TokStart);
  while (bufPtr < TokEnd) {
    spelling[length++] = *bufPtr;
    ++bufPtr;
  }
  Result.resize(length);
  return Result;
}

bool Lexer::Lex(Token &Result) {
LexNextToken:
  // New Token
  Result.startToken();
  Result.setLiteralData(nullptr);

  // Cache BufferPtr
  const char *CurPtr = BufferPtr;

  // Clean horizontal whitespace
  if (clang::isHorizontalWhitespace(*CurPtr)) {
    do {
      ++CurPtr;
    } while (clang::isHorizontalWhitespace(*CurPtr));

    BufferPtr = CurPtr;
  }

  char Char = getAndAdcanceChar(CurPtr);
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
      (void)getAndAdcanceChar(CurPtr);
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

  case '/':
    Char = getAndAdcanceChar(CurPtr);
    if (Char == '/') {
      SkipLineComment(CurPtr);
      goto LexNextToken;
    } else if (Char == '*') {
      SkipBlockComment(CurPtr);
      goto LexNextToken;
    }
    break;
  // clang-format off
  case '0':  case '1':
  case '2':  case '3':
  case '4':  case '5':
  case '6':  case '7':
  case '8':  case '9':
    // clang-format on
    return lexNumericLiteral(Result, CurPtr);

  case '\'':
    return lexBaseFormat(Result, CurPtr);
  // clang-format off
  case 'A':  case 'B':  case 'C':  case 'D':
  case 'E':  case 'F':  case 'G':  case 'H':
  case 'I':  case 'J':  case 'K':  case 'L':
  case 'M':  case 'N':  case 'O':  case 'P':
  case 'Q':  case 'R':  case 'S':  case 'T':
  case 'U':  case 'V':  case 'W':  case 'X':
  case 'Y':  case 'Z':  case 'a':  case 'b':
  case 'c':  case 'd':  case 'e':  case 'f':
  case 'g':  case 'h':  case 'i':  case 'j':
  case 'k':  case 'l':  case 'm':  case 'n':
  case 'o':  case 'p':  case 'q':  case 'r':
  case 's':  case 't':  case 'u':  case 'v':
  case 'w':  case 'x':  case 'y':  case 'z':
  case '_':
    // clang-format on
    if (lastToken.is(tok::_INTEGER_BASE)) {
      if (clang::isHexDigit(Char) || (Char == 'x') || (Char == 'X') ||
          (Char == 'z') || (Char == 'Z')) {
        return lexNumericLiteral(Result, CurPtr);
      }
    }

  case '?':
    if (lastToken.is(tok::_INTEGER_BASE)) {
      return lexNumericLiteral(Result, CurPtr);
    }
  default:
    if (clang::isASCII(Char)) {
      Kind = tok::_UNKNOWN;
      break;
    }
  }

  FormToken(Result, CurPtr, Kind);

  return true;
}

} // namespace svlang
