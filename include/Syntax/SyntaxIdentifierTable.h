#ifndef SYNTAXIDENTIFIERTABLE_H
#define SYNTAXIDENTIFIERTABLE_H

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Compiler.h>

#include <string_view>
#include <unordered_map>

#include "Basic/TokenKinds.h"
#include "SyntaxIdentifierInfo.h"

namespace svlang{

using SyntaxIT_t = LLVM_READNONE std::unordered_map<std::string_view, Syntax::IdentifierInfo>;

extern SyntaxIT_t CompilerDirectivesTable;

extern Syntax::IdentifierInfo* getCompilerDirective(llvm::StringRef Name);

}

#endif // SYNTAXIDENTIFIERTABLE_H
