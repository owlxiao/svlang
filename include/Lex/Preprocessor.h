#ifndef SVLANG_LEX_PREPROCESSOR_H
#define SVLANG_LEX_PREPROCESSOR_H

#include <llvm/Support/Allocator.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>

#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "Syntax/MacroInfo.h"

namespace svlang {

class Lexer;

class Preprocessor {
public:
  Preprocessor(llvm::SourceMgr &SM);

  ~Preprocessor() = default;

public:
  // Lex the next token for this preprocessor
  void Lex(Token &Token);

  void enterMainSourceFile();

  bool handleCompilerDirective(Token &Result);

public:
  /// Handle `define directive
  bool handleDefineDirective(Token &DefineTok);
  // Helper functions
  Syntax::MacroFormalArgument* parseFormalArgument(Token &Tok);
  bool readMacroArgumentList(Syntax::MacroInfo *MI, Token &Tok);
  Syntax::MacroInfo* readMacroArgumentListAndMacroText(const Token &MacroNameTok);
  /// End

  // Print the Token to stderr
public:
  void DumpToken(const Token &Tok) const;

  llvm::BumpPtrAllocator &getPreprocessorAllocator() { return BP; }

private:
  llvm::SourceMgr &SourceMgr;

  std::unique_ptr<Lexer> CurLexer;

  llvm::BumpPtrAllocator BP;
};

} // namespace svlang

#endif // SVLANG_LEX_PREPROCESSOR_H
