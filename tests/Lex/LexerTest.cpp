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
using expectedToken = llvm::ArrayRef<svlang::tok::TokenKind>;
/*
 * 5.7 Numbers
 * */

// 5.7.1 Integer literal constants
// Example 1: Unsized literal constant numbers
TEST_F(LexerTest, Lex_Numbers_Integer_Literal_Constants_Unsized) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  auto &test = "659\n"
               "'h 837FF\n"
               "'o7460\n";
  CheckLex(test, eTokens);
}

// Example 2: Sized literal constant numbers
TEST_F(LexerTest, Lex_Numbers_Integer_Literal_Constants_Sized) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);

  auto &test = "4'b1001\n"
               "5 'D 3\n"
               "3'b01x\n"
               "12'hx\n"
               "12'hz\n";
  CheckLex(test, eTokens);
}

// Example 3: Using sign with literal constant numbers
// TODO: Support sign operator
TEST_F(LexerTest, Lex_Numbers_Integer_Literal_Constants_Sign) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);

  auto &test = "4 'shf\n"
               "16'sd?\n";
  CheckLex(test, eTokens);
}

// Example 4: Automatic left padding of literal constant numbers
TEST_F(LexerTest,
       Lex_Numbers_Integer_Literal_Constants_Automatic_Left_Padding) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  auto &test = "'h x\n"
               "'h 3x\n"
               "'h z3\n"
               "'h 0z3"
               "'h5\n"
               "'hx\n"
               "'hz\n";
  CheckLex(test, eTokens);
}

// Example 5: Automatic left padding of constant literal numbers using a
// single-bit value
TEST_F(
    LexerTest,
    Lex_Numbers_Integer_Literal_Constants_Automatic_Left_Padding_Using_a_signle_bit_value) {
  std::vector<svlang::tok::TokenKind> eTokens{
      4, svlang::tok::_UNBASED_UNSIZED_LITERAL};
  auto &test = "'0\n"
               "'1\n"
               "'x\n"
               "'z";
  CheckLex(test, eTokens);
}

// Example 6: Underscores in literal constant numbers
TEST_F(
    LexerTest,
    Lex_Numbers_Integer_Literal_Constants_Underscores_in_literal_constant_numbers) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  eTokens.push_back(svlang::tok::_INTEGER_BASE);
  eTokens.push_back(svlang::tok::_INTEGER_LITERAL);
  auto &test = "27_195_000\n"
               "16'b0011_0101_1111\n"
               "32 'h 12ab_f001";
  CheckLex(test, eTokens);
}

TEST_F(LexerTest, Lex_Numbers_Real_literal_Constants) {
  std::vector<svlang::tok::TokenKind> eTokens{9, svlang::tok::_REAL_LITERAL};
  auto &test = "1.2\n"
               "0.1\n"
               "2394.26331\n"
               "1.2E12\n"
               "1.30e-2\n"
               "0.1e-0\n"
               "23E10\n"
               "29E-2\n"
               "236.123_763_e-12\n";
  CheckLex(test, eTokens);
}

/*
 * 5.8 Time literals
 * */
TEST_F(LexerTest, Lex_Time_Literals) {
  std::vector<svlang::tok::TokenKind> eTokens{2, svlang::tok::_TIME_LITERAL};

  auto &test = "2.1ns\n"
               "40ps";
  CheckLex(test, eTokens);
}
