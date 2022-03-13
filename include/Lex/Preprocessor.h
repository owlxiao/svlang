#ifndef SVLANG_LEX_PREPROCESSOR_H
#define SVLANG_LEX_PREPROCESSOR_H

#include <llvm/Support/SourceMgr.h>

#include <memory>

#include "Lex/Lexer.h"
#include "Lex/Token.h"

namespace svlang {

class Preprocessor {
public:
  Preprocessor(llvm::SourceMgr &SM);

  ~Preprocessor() = default;

public:
  // Lex the next token for this preprocessor
  void Lex(Token &Token);

  void enterMainSourceFile();

private:
  llvm::SourceMgr &SourceMgr;

  std::unique_ptr<Lexer> CurLexer;
};

} // namespace svlang

#endif // SVLANG_LEX_PREPROCESSOR_H