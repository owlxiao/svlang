#include <memory>

#include "Basic/TokenKinds.h"
#include "Lex/Lexer.h"
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
  CurLexer.reset(new Lexer(SourceMgr, *this));
}

void Preprocessor::DumpToken(const Token &Tok) const {
  llvm::errs() << tok::getTokenName(Tok.getKind()) << ' ' << '`'
               << Lexer::getSpelling(Tok) << '`';
  llvm::errs() << "\t";
  auto [Line, Col] =
      SourceMgr.getLineAndColumn(Tok.getLocation(), SourceMgr.getMainFileID());
  llvm::errs() << "Line:" << Line;
  llvm::errs() << "\tCol:" << Col;
}

} // namespace svlang
