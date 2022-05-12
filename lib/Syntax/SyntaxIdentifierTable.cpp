#include <llvm/ADT/StringRef.h>

#include "Syntax/SyntaxIdentifierTable.h"

namespace svlang {

static Syntax::IdentifierInfo Add(tok::CPKeywordKind Kind) {
  Syntax::IdentifierInfo II{Kind};
  return II;
}

SyntaxIT_t CompilerDirectivesTable = {
#define COMPILER_DIRECTIVE(X) {#X, Add(tok::_CP_##X)},
#include "Syntax/TokenKinds.def"
};

Syntax::IdentifierInfo *getCompilerDirective(llvm::StringRef Name) {
  auto II = CompilerDirectivesTable.find(Name);
  if (II != CompilerDirectivesTable.end()) {
    return &II->second;
  } else {
    return nullptr;
  }
}

} // namespace svlang
