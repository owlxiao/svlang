#ifndef SVLANG_BASIC_TOKENKINDS_H
#define SVLANG_BASIC_TOKENKINDS_H

#include <llvm/Support/Compiler.h>

namespace svlang {
namespace tok {
// enum TokenKind : unsigned int { _UNKNOWN, _EOF, _NUM_TOKENS };

enum TokenKind : unsigned int {
#define TOK(X) _##X,
#include "Syntax/TokenKinds.def"
  _NUM_TOKENS
};

const char *getTokenName(TokenKind Kind) LLVM_READNONE;

} // namespace tok
} // namespace svlang

#endif // SVLANG_BASIC_TOKENKINDS
