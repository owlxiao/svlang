#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include <llvm/ADT/SmallString.h>
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

bool Lexer::lexStringLiteral(Token &Result, const char *CurPtr) {
  // TODO: Fix limit to the length of a string literal
  auto String = std::make_unique<llvm::SmallString<128>>();
  size_t char2digits;
  char C;

  while (true) {
    C = getAndAdcanceChar(CurPtr);
    // Handle Specifying special characters
    if (C == '"') {
      break;
    }

    if (C == '\\') {
      C = getAndAdcanceChar(CurPtr);

      switch (C) {
      // clang-format off
      case 'n' : String->push_back ('\n'); break; /* Newline character  */
      case 't' : String->push_back('\t'); break; /* Tab character      */
      case '\\': String->push_back('\\'); break; /* \ character        */
      case '"' : String->push_back('"' ); break; /* " character        */
      case 'v' : String->push_back('\v'); break; /* vertical character */
      case 'f' : String->push_back('\f'); break; /* form feed          */
      case 'a' : String->push_back('\a'); break; /* bell               */

      // handle \ddd
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        // clang-format on
        char2digits = getDigitValue(C);
        if (isOctalNumber(*CurPtr)) {
          C = getAndAdcanceChar(CurPtr);
          char2digits = (char2digits * 8) + getDigitValue(C);
          if (isOctalNumber(*CurPtr)) {
            C = getAndAdcanceChar(CurPtr);
            char2digits = (char2digits * 8) + getDigitValue(C);
          }
        }
        String->push_back(static_cast<char>(char2digits));
        break;

      case 'x':
        char2digits = getHexDigitValue(C = getAndAdcanceChar(CurPtr));
        if (clang::isHexDigit(*CurPtr)) {
          C = getAndAdcanceChar(CurPtr);
          char2digits = (char2digits * 16) + getHexDigitValue(C);
        }
        String->push_back(static_cast<char>(char2digits));
        break;

      default:
        break;
      }
    } else {
      String->push_back(C);
    }
  }

  FormToken(Result, CurPtr, svlang::tok::_STRING_LITERAL);
  Result.setLiteralData(&*String->begin());
  return true;
}

bool Lexer::lexIdentifier(Token &Result, const char *CurPtr,
                          const bool isSimple) {
  unsigned Size;
  unsigned char Char;

  if (!isSimple) {
    (void)getAndAdcanceChar(CurPtr);
  }

  while (true) {
    Char = getCharAndSize(CurPtr, Size);

    if (isIdentifier(Char)) {
      CurPtr = ConsumeChar(CurPtr, Size);
      continue;
    }

    if (!isSimple && clang::isPrintable(Char)) {
      CurPtr = ConsumeChar(CurPtr, Size);
      if (clang::isWhitespace(Char)) {
        break;
      }
      continue;
    }
    break;
  }

  const char *TokStart = BufferPtr;
  FormToken(Result, CurPtr, tok::_IDENTIFIER);
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
  unsigned SizeTmp;

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

    // /
    // /=
    // //
    // /*
  case '/':
    Char = getCharAndSize(CurPtr, SizeTmp);
    // Comments
    if (Char == '/') {
      SkipLineComment(CurPtr);
      goto LexNextToken;
    } else if (Char == '*') {
      SkipBlockComment(CurPtr);
      goto LexNextToken;
    }
    // Binary arithmetic assignment operator
    if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_SLASH_EQUAL;
    } else {
      // Binary arithmetic operator
      Kind = tok::_SLASH;
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

    // '{
  case '\'':
    // 7.9.11 Associative array literals:
    // 10.10.1 Unpacked array concatenations compared with array assignment
    // patterns
    if (getCharAndSize(CurPtr, SizeTmp) == '{') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_APOSTROPHE_L_BRACE;
      break;
    }
    return lexBaseFormat(Result, CurPtr);

    // (
    // (*
  case '(':
    // 5.12 Attributes
    if (getCharAndSize(CurPtr, SizeTmp) == '*') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_L_PAREN_STAR;
    } else {
      Kind = tok::_L_PAREN;
    }
    break;

    // )
  case ')':
    Kind = tok::_R_PAREN;
    break;

    // {
  case '{':
    Kind = tok::_L_BRACE;
    break;

    // }
  case '}':
    Kind = tok::_R_BRACE;
    break;

  case '"':
    return lexStringLiteral(Result, CurPtr);

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
    return lexIdentifier(Result, CurPtr, true);

  case '\\':
    return lexIdentifier(Result, CurPtr, false);

    // =
    // ==
    // ===
    // ==?
  case '=':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      (void)getAndAdcanceChar(CurPtr);
      switch (getCharAndSize(CurPtr, SizeTmp)) {
        // Binary case equality operators
        // a === b: a equal to b, including x and z
      case '=':
        Kind = tok::_EQUAL_EQUAL_EQUAL;
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        break;

        // Binary wildcard equality operators
        // a ==? b: a equals b, X and Z values in b act as wildcards
      case '?':
        Kind = tok::_EQUAL_EQUAL_QUESTION;
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        break;

      // Binary logical equality operators
      // a == b: a equal to b, result can be unknown
      default:
        Kind = tok::_EQUAL_EQUAL;
        break;
      }
    } else {
      // Binary assignment operator
      Kind = tok::_EQUAL;
      // CurPtr = ConsumeChar(CurPtr, SizeTmp);
    }
    break;

    // +
    // ++
    // +=
    // +:
  case '+':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
      // Unary increment, decrement operators
    case '+':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_PLUS_PLUS;
      break;
    // Binary arithmetic assignment operators
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_PLUS_EQUAL;
      break;
    // Vector bit-select
    case ':':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_PLUS_COLON;
      break;
      // Binary arithmetic operators
      // a + b: a plus b
    default:
      Kind = tok::_PLUS;
      break;
    }
    break;

    // -
    // --
    // -=
    // -:
    // ->
  case '-':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
      // Unary decrement operators
    case '-':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_MINUS_MINUS;
      break;
    // Binary arithmetic assignment operators
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_MINUS_EQUAL;
      break;
    // Vector bit-select
    case ':':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_MINUS_COLON;
      break;

    case '>':
      (void)getAndAdcanceChar(CurPtr);
      // event_trigger
      if (getCharAndSize(CurPtr, SizeTmp) == '>') {
        Kind = tok::_MINUS_GREATER_GREATER;
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
      } else {
        // Binary logical operators
        Kind = tok::_ARROW;
      }
      break;

    default:
      Kind = tok::_MINUS;
      break;
    }
    break;

    // *
    // **
    // *=
    // *>
    // *)
  case '*':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '*':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_STAR_STAR;
      break;
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_STAR_EQUAL;
      break;
      // 30.4.5 Full connection and parallel connection paths
    case '>':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_STAR_GREATER;
      break;
      // 5.12 Attributes
    case ')':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_STAR_R_PAREN;
      break;
    default:
      Kind = tok::_STAR;
      break;
    }
    break;

    // %
    // %=
  case '%':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_PERCENT_EQUAL;
      break;
    }
    // 11.4.3 Arithmetic operators
    // a%b: a modulo b
    Kind = tok::_PERCENT;
    break;

    // &
    // &&
    // &&&
    // &=
  case '&':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '&':
      (void)getAndAdcanceChar(CurPtr);
      if (getCharAndSize(CurPtr, SizeTmp) == '&') {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_AMP_AMP_AMP;
      } else {
        Kind = tok::_AMP_AMP;
      }
      break;
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_AMP_EQUAL;
      break;
    default:
      Kind = tok::_AMP;
      break;
    }
    break;

    // |
    // ||
    // |=
    // |->
  case '|':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '|':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_PIPE_PIPE;
      break;
    case '-':
      (void)getAndAdcanceChar(CurPtr);
      if (getCharAndSize(CurPtr, SizeTmp) == '>') {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_PIPE_MINUS_GREATER;
        break;
      }
      Kind = tok::_PIPE;
      break;
    case '=':
      (void)getAndAdcanceChar(CurPtr);
      if (getCharAndSize(CurPtr, SizeTmp) == '>') {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_PIPE_EQUAL_GREATER;
        break;
      }
      Kind = tok::_PIPE_EQUAL;
      break;
    default:
      Kind = tok::_PIPE;
    }
    break;

    // ^
    // ^~
    // ^=
  case '^':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '~':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_CARET_TILDE;
      break;
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_CARET_EQUAL;
      break;
    default:
      Kind = tok::_CARET;
      break;
    }
    break;

    // <
    // <=
    // <<
    // <<=
    // <<<
    // <<<=
  case '<':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_LESS_EQUAL;
      break;
    case '-':
      (void)getAndAdcanceChar(CurPtr);
      if (getCharAndSize(CurPtr, SizeTmp) == '>') {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_LESS_MINUS_GREATER;
        break;
      }
      Kind = tok::_LESS;
      break;
    case '<':
      (void)getAndAdcanceChar(CurPtr);
      switch (getCharAndSize(CurPtr, SizeTmp)) {
      case '<':
        (void)getAndAdcanceChar(CurPtr);
        if (getCharAndSize(CurPtr, SizeTmp) == '=') {
          CurPtr = ConsumeChar(CurPtr, SizeTmp);
          Kind = tok::_LESS_LESS_LESS_EQUAL;
          break;
        }
        Kind = tok::_LESS_LESS_LESS;
        break;
      case '=':
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_LESS_LESS_EQUAL;
        break;
      default:
        Kind = tok::_LESS_LESS;
        break;
      }
      break;
    default:
      Kind = tok::_LESS;
      break;
    }
    break;

    // >
    // >=
    // >>
    // >>>
    // >>=
    // >>>=
  case '>':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_GREATER_EQUAL;
      break;
    case '>':
      (void)getAndAdcanceChar(CurPtr);
      switch (getCharAndSize(CurPtr, SizeTmp)) {
      case '=':
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_GREATER_GREATER_EQUAL;
        break;
      case '>':
        (void)getAndAdcanceChar(CurPtr);
        if (getCharAndSize(CurPtr, SizeTmp) == '=') {
          CurPtr = ConsumeChar(CurPtr, SizeTmp);
          Kind = tok::_GREATER_GREATER_GREATER_EQUAL;
        } else {
          Kind = tok::_GREATER_GREATER_GREATER;
        }
        break;
      default:
        Kind = tok::_GREATER_GREATER;
        break;
      }
      break;
    default:
      Kind = tok::_GREATER;
      break;
    }
    break;

    // ?
  case '?':
    if (lastToken.is(tok::_INTEGER_BASE)) {
      return lexNumericLiteral(Result, CurPtr);
    } else {
      Kind = tok::_QUESTION;
    }
    break;

    // :
    // :=
    // :/
    // ::
  case ':':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '=':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_COLON_EQUAL;
      break;
    case '/':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_COLON_SLASH;
      break;
    case ':':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_COLON_COLON;
      break;
    default:
      Kind = tok::_COLON;
      break;
    }
    break;

    // !
    // !=
    // !==
    // !=?
  case '!':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      (void)getAndAdcanceChar(CurPtr);
      switch (getCharAndSize(CurPtr, SizeTmp)) {
      case '=':
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_EXCLAIM_EQUAL_EQUAL;
        break;
      case '?':
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::_EXCLAIM_EQUAL_QUESTION;
        break;
      default:
        Kind = tok::_EXCLAIM_EQUAL;
        break;
      }
    } else {
      Kind = tok::_EXCLAIM;
    }
    break;

    // ~
    // ~&
    // ~|
    // ~^
  case '~':
    Char = getCharAndSize(CurPtr, SizeTmp);
    switch (Char) {
    case '&':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_TILDE_AMP;
      break;
    case '|':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_TILDE_PIPE;
      break;
    case '^':
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::_TILDE_CARET;
      break;
    default:
      Kind = tok::_TILDE;
      break;
    }
    break;

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
