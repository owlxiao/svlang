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

private:
  MacroFormalArgument *ArgumentList = nullptr;
};
} // namespace Syntax
} // namespace svlang

#endif // MACROINFO_H
