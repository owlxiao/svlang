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

private:
  const char *BufferStart;
  const char *BufferEnd;
  const char *BufferPtr;
};

} // namespace svlang

#endif // SVLANG_LEX_LEXER_H
