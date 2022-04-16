#ifndef SVLANG_LEX_LEXER_H
#define SVLANG_LEX_LEXER_H

#include <llvm/Support/SourceMgr.h>

#include "Lex/Token.h"

namespace svlang {

class Lexer {
public:
  Lexer(llvm::SourceMgr &SM);

public:
  bool Lex(Token &Result);

  char advanceChar(const char *Ptr, unsigned int &Size);

  char advance(const char *&Ptr);

  void FormToken(Token &Result, const char *TokEnd, tok::TokenKind Kind);

  const char *ConsumeChar(const char *Ptr, unsigned int Size);

  char getCharAndSize(const char*Ptr, unsigned &Size);

  // Helper functions for lex token
public:
  void SkipWhiteSpace(Token &Result, const char *CurPtr);
  void SkipLineComment(const char *CurPtr);
  void SkipBlockComment(const char *CurPtr);


public:
  llvm::SMLoc getSourceLocation(const char* Loc) const;

  // Helper functions to lex a token of the specific type
public:
  bool lexNumericLiteral(Token &Result, const char *CurPtr);

  // Helper functions to better explain what token means
public:
  static std::string getSpelling(const Token &Tok);

private:
  const char *BufferStart;
  const char *BufferEnd;
  const char *BufferPtr;
};

} // namespace svlang

#endif // SVLANG_LEX_LEXER_H
