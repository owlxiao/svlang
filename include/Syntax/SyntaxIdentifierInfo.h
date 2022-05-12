#ifndef SYNTAXIDENTIFIERINFO_H
#define SYNTAXIDENTIFIERINFO_H

#include <llvm/ADT/StringRef.h>

#include "Basic/TokenKinds.h"

namespace svlang {
namespace Syntax {

struct IdentifierInfo {
  IdentifierInfo(tok::CPKeywordKind Kind) : CPKind(Kind) {};
  tok::CPKeywordKind CPKind;
};

} // namespace Syntax
} // namespace svlang

#endif // SYNTAXIDENTIFIERINFO_H
