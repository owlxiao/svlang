#ifndef SVLANG_BASIC_TOKENKINDS_H
#define SVLANG_BASIC_TOKENKINDS_H
#include <llvm/Support/Compiler.h>

namespace tok {
enum TokenKind : unsigned int { _UNKNOWN, _EOF, _NUM_TOKENS };

} // namespace tok

#endif // SVLANG_BASIC_TOKENKINDS
