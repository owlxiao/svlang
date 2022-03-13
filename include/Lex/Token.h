#ifndef SVLANG_LEX_TOKEN_H
#define SVLANG_LEX_TOKEN_H

#include <clang/Basic/SourceLocation.h>

#include "Basic/TokenKinds.h"

namespace svlang {

class Token {
public:
  void startToken() {
    // Kind = 0;
    ptrData = nullptr;
    Length = 0;
    Loc = clang::SourceLocation().getRawEncoding();
  }

private:
  clang::SourceLocation::UIntTy Loc;

  clang::SourceLocation::UIntTy Length;

  void *ptrData;

  tok::TokenKind Kind;
};

} // namespace svlang

#endif // SVLANG_LEX_TOKEN_H