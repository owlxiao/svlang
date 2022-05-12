#ifndef SVLANG_LEX_TOKEN_H
#define SVLANG_LEX_TOKEN_H

#include <clang/Basic/SourceLocation.h>
#include <llvm/Support/SMLoc.h>

#include "Basic/TokenKinds.h"
#include "Syntax/SyntaxIdentifierInfo.h"

namespace svlang {

class Token {
public:
  void startToken() {
    Kind = tok::_UNKNOWN;
    PtrData = nullptr;
    Length = 0;
    Location = nullptr;
  }

  void setKind(tok::TokenKind K) { Kind = K; }

  void setLength(unsigned int Len) { Length = Len; }

  void setLiteralData(const char *Ptr) { PtrData = const_cast<char *>(Ptr); }

  void setLocation(llvm::SMLoc Loc) { Location = Loc.getPointer(); }

  void setIdentifierInfo(Syntax::IdentifierInfo *II) {
    PtrData = (void*)II;
  }

  tok::TokenKind getKind() const { return Kind; }

  bool is(tok::TokenKind K) const { return Kind == K; }
  bool isNot(tok::TokenKind K) const { return Kind != K; }

  const char *getLiteralData() const {
    return reinterpret_cast<const char *>(PtrData);
  }

  llvm::StringRef getIdentifier() const {
    return llvm::StringRef(reinterpret_cast<const char*>(PtrData), getLength());
  }

  llvm::SMLoc getLocation() const {
    return llvm::SMLoc::getFromPointer(Location);
  }

  Syntax::IdentifierInfo* getIdentifierInfo() const {
    return (Syntax::IdentifierInfo*)PtrData;
  }

  size_t getLength() const { return Length; }

private:
  const char *Location;

  size_t Length;

  void *PtrData;

  tok::TokenKind Kind;
};

} // namespace svlang

#endif // SVLANG_LEX_TOKEN_H
