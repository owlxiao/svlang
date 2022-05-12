#include <llvm/ADT/StringRef.h>

#include <iostream>

#include "Lex/Preprocessor.h"
#include "Syntax/SyntaxIdentifierTable.h"

namespace svlang {

bool Preprocessor::handleCompilerDirective(Token &Result) {
  Token SavedGraveAccent = Result;
  Lex(Result);

  Syntax::IdentifierInfo *II = Result.getIdentifierInfo();
  switch (II->CPKind) {
  default:
    break;
  }

  return true;
}

} // namespace svlang
