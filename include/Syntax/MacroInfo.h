#ifndef MACROINFO_H
#define MACROINFO_H

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Support/Allocator.h>

#include "Lex/Token.h"

namespace svlang {
namespace Syntax {

/// Syntax 22-2—Syntax for text macro definition
///
/// formal_argument ::=
/// simple_identifier [ = default_text ]
struct MacroFormalArgument {
  using MFATokenList = llvm::SmallVector<Token, 8>;

  Token SimpleIdentifier;
  Token* DefaultText;
};
/// End

class MacroInfo {
public:
  // set list of tokens as the argument list of the list_of_actual_arguments
  void setArgumentList(llvm::ArrayRef<MacroFormalArgument> List,
                       llvm::BumpPtrAllocator &PPAllocator) {
    if (List.empty())
      return;

    ArgumentList = PPAllocator.Allocate<MacroFormalArgument>(List.size());
    std::copy(List.begin(), List.end(), ArgumentList);
  }

  /// Retuen true whether this is a function-like macro
  bool isFunctionLike() const { return _IsFunctionLike; }
  bool isObjectLike() const { return !_IsFunctionLike; }

  /// Set this macro has list_of_formal_arguments
  void setIsFunctionLike() { _IsFunctionLike = true; }

private:
  MacroFormalArgument *ArgumentList = nullptr;

  /// True if this macro is function like.
  /// e.g. `define HELLO(X) is a function-like macro
  bool _IsFunctionLike : 1;
};
} // namespace Syntax
} // namespace svlang

#endif // MACROINFO_H
