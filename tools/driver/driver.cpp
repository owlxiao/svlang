#include <clang/Basic/FileManager.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#include "Lex/Preprocessor.h"
#include "Lex/Token.h"

static llvm::cl::list<std::string> InputFile(llvm::cl::Positional,
                                             llvm::cl::ZeroOrMore,
                                             llvm::cl::desc("<input-file>"));

int main(int argc, const char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv);

  for (auto &F : InputFile) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileOrErr =
        llvm::MemoryBuffer::getFile(F);

    if (std::error_code BufferError = FileOrErr.getError()) {
      llvm::errs() << "Error reading " << F << ": " << BufferError.message()
                   << "\n";
      continue;
    }

    // FileSystem
    llvm::SourceMgr SrcMgr;
    SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr), llvm::SMLoc());

    svlang::Preprocessor PP(SrcMgr);
    PP.enterMainSourceFile();
    int n = 0;
    svlang::Token Tok;
    do {
      PP.Lex(Tok);
      PP.DumpToken(Tok);
      llvm::errs() << "\n";
    } while (Tok.isNot(svlang::tok::_EOF));
  }
  return 0;
}
