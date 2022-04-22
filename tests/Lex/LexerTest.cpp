#include <gtest/gtest.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

#include <memory>
#include <string>
#include <vector>

#include "Lex/Preprocessor.h"

class LexerTest : public ::testing::Test {
protected:
  llvm::SourceMgr SourceMgr;
  std::unique_ptr<svlang::Preprocessor> PP;

protected:
  std::unique_ptr<svlang::Preprocessor> CreatePP(llvm::StringRef Source) {
    std::unique_ptr<llvm::MemoryBuffer> Buf =
        llvm::MemoryBuffer::getMemBuffer(Source);
    SourceMgr.AddNewSourceBuffer(std::move(Buf), llvm::SMLoc());
    std::unique_ptr<svlang::Preprocessor> PP =
        std::make_unique<svlang::Preprocessor>(SourceMgr);
    PP->enterMainSourceFile();
    return PP;
  }

  std::vector<svlang::Token> Lex(llvm::StringRef Source) {
    PP = CreatePP(Source);
    std::vector<svlang::Token> toks;

    while (1) {
      svlang::Token tok;
      PP->Lex(tok);
      if (tok.is(svlang::tok::_EOF)) {
        break;
      }
      toks.push_back(tok);
    }

    return toks;
  }

  std::vector<svlang::Token>
  CheckLex(llvm::StringRef Source,
           llvm::ArrayRef<svlang::tok::TokenKind> ExpectedTokens) {
    auto toks = Lex(Source);
    EXPECT_EQ(ExpectedTokens.size(), toks.size());
    for (unsigned i = 0, e = ExpectedTokens.size(); i != e; ++i) {
      EXPECT_EQ(ExpectedTokens[i], toks[i].getKind());
    }
    return toks;
  }
};

/*
 * 5. Lexical conventions
 * */

/*
 * 5.4 Comments
 * */
std::vector<svlang::tok::TokenKind> ExpectedNullTokens{0};

TEST_F(LexerTest, Lex_Line_Comment) {
  CheckLex("// This is Line comment", ExpectedNullTokens);
}

TEST_F(LexerTest, Lex_Line_Comment_multi_lines) {
  auto &test = R"(// First line comment
  // Second line comment
)";
  CheckLex(test, ExpectedNullTokens);
}

TEST_F(LexerTest, Lex_Block_Comment) {
  CheckLex("/* This is block comment */", ExpectedNullTokens);
}

TEST_F(LexerTest, Lex_Block_Comment_multi_lines) {
  auto &text = R"(/*
This is
Block Comment
o
n
multiple lines
*/)";
  CheckLex(text, ExpectedNullTokens);
}
