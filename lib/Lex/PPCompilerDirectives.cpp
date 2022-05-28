#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>

#include <memory>

#include "Lex/Preprocessor.h"
#include "Syntax/SyntaxIdentifierTable.h"

namespace svlang {

// text_macro_definition ::=
// `define text_macro_name macro_text
//
// text_macro_name ::=
// text_macro_identifier [ ( list_of_formal_arguments ) ]
//
// list_of_formal_arguments ::=
//    formal_argument { , formal_argument }
//
// formal_argument ::=
//    simple_identifier [ = default_text ]
//
// text_macro_identifier ::=
//    identifier
bool Preprocessor::handleDefineDirective(Token &DefineTok) {
  // First we read macro name
  Token TextMacroName;
  Lex(TextMacroName);

  // Then we read list_of_formal_arguments
  Syntax::MacroInfo *const MI =
      readMacroArgumentListAndMacroText(TextMacroName);

  return true;
}

Syntax::MacroInfo *
Preprocessor::readMacroArgumentListAndMacroText(const Token &MacroNameTok) {
  Syntax::MacroInfo *const MI = BP.Allocate<Syntax::MacroInfo>();

  // Now we enter list_of_formal_arguments
  Token Tok;
  Lex(Tok);

  // This macro has list_of_formal_arguments
  if (Tok.is(tok::_L_PAREN)) {
    // this macro has list_of_formal_arguments
    // Read macro argument list
    MI->setIsFunctionLike();
    if (!readMacroArgumentList(MI, Tok))
      return nullptr;
  }
  return MI;
}

Syntax::MacroFormalArgument *Preprocessor::parseFormalArgument(Token &Tok) {
  auto DefaultText = BP.Allocate<Syntax::MacroFormalArgument::MFATokenList>();
  auto MFA = BP.Allocate<Syntax::MacroFormalArgument>();
  // Lex simple_identifier
  Lex(Tok);
  MFA->SimpleIdentifier = Tok;

  // Is there a default_text
  Lex(Tok);
  if (Tok.is(tok::_EQUAL)) {
    while (true) {
      Lex(Tok);
      if (Tok.is(tok::_COMMA))
        break;
      DefaultText->push_back(Tok);
    }
  }
  MFA->DefaultText = DefaultText->begin();
  return MFA;
}

bool Preprocessor::readMacroArgumentList(Syntax::MacroInfo *MI, Token &Tok) {
  llvm::SmallVector<Syntax::MacroFormalArgument *, 16> Arguments;

  while (true) {
    if (Tok.getKind() == tok::_R_PAREN)
      break;
    Arguments.push_back(parseFormalArgument(Tok));
  }
  return true;
}

bool Preprocessor::handleCompilerDirective(Token &Result) {
  Token SavedGraveAccent = Result;
  Lex(Result);

  Syntax::IdentifierInfo *II = Result.getIdentifierInfo();
  switch (II->CPKind) {
  case tok::_CP_define:
    return handleDefineDirective(Result);
  default:
    break;
  }

  return true;
}

} // namespace svlang
