#ifndef SVLANG_LEX_TOKEN_H
#define SVLANG_LEX_TOKEN_H

#include <clang/Basic/SourceLocation.h>
#include <llvm/Support/SMLoc.h>

#include "Basic/TokenKinds.h"

namespace svlang {

class Token {
public:
  void startToken() {
    Kind = tok::_UNKNOWN;
    PtrData = nullptr;
    Length = 0;
  }

  void setKind(tok::TokenKind K) { Kind = K; }

  void setLength(unsigned int Len) { Length = Len; }

  void setLiteralData(const char *Ptr) { PtrData = const_cast<char *>(Ptr); }

  void setLocation(const char *Loc) { Location = Loc; }

  tok::TokenKind getKind() const { return Kind; }

  bool is(tok::TokenKind K) const { return Kind == K; }
  bool isNot(tok::TokenKind K) const { return Kind != K; }

  const char *getLiteralData() const {
    return reinterpret_cast<const char *>(PtrData);
  }

  llvm::SMLoc getLocation() const {
    return llvm::SMLoc::getFromPointer(Location);
  }

private:
  const char *Location;

  clang::SourceLocation::UIntTy Length;

  void *PtrData;

  tok::TokenKind Kind;
};

} // namespace svlang

#endif // SVLANG_LEX_TOKEN_H
