#ifndef SVLANG_LEX_TOKEN_H
#define SVLANG_LEX_TOKEN_H

#include <clang/Basic/SourceLocation.h>

#include "Basic/TokenKinds.h"

namespace svlang {

class Token {
public:
  void startToken() {
    Kind = tok::_UNKNOWN;
    PtrData = nullptr;
    Length = 0;
    Location = clang::SourceLocation().getRawEncoding();
  }

  void setKind(tok::TokenKind K) { Kind = K; }

  void setLength(unsigned int Len) { Length = Len; }

  void setLocation(clang::SourceLocation Loc) {
    Location = Loc.getRawEncoding();
  }

  void setPtrData(const char* Ptr) {
    PtrData = const_cast<char*>(Ptr);
  }

  bool is(tok::TokenKind K) const { return Kind == K; }
  bool isNot(tok::TokenKind K) const { return Kind != K; }

  void* getPtrData() { return PtrData; }

private:
  clang::SourceLocation::UIntTy Location;

  clang::SourceLocation::UIntTy Length;

  void *PtrData;

  tok::TokenKind Kind;
};

} // namespace svlang

#endif // SVLANG_LEX_TOKEN_H
