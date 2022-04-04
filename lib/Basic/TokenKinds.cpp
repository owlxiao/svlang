#include "Basic/TokenKinds.h"

namespace svlang {

static const char *const TokNames[] = {
#define TOK(X) #X,
#include "Syntax/TokenKinds.def"
    nullptr};

const char *tok::getTokenName(TokenKind Kind) {
  if (Kind < tok::_NUM_TOKENS) {
    return TokNames[Kind];
  }
  return nullptr;
}

} // namespace svlang
