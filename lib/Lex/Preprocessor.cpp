#include "Lex/Preprocessor.h"

namespace svlang {

Preprocessor::Preprocessor(llvm::SourceMgr &SM) : SourceMgr(SM) {}

void Preprocessor::Lex(Token &Result) {
  bool returnedToken;

  do {
    returnedToken = CurLexer->Lex(Result);
  } while (!returnedToken);
}

void Preprocessor::enterMainSourceFile() {
  CurLexer.reset(new Lexer(SourceMgr));
}

} // namespace svlang
