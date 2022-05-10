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

  void cmpString(svlang::Token Token, std::string_view eString) {
    // if (eString.compare(Token.getLiteralData())) {
    EXPECT_EQ(eString, Token.getLiteralData())
        << "Expect String:" << eString << std::endl
        << "Actual String:" << Token.getLiteralData() << std::endl;
  }
};

/*
 * 5. Lexical conventions
 * */
using eTokns_t = std::vector<svlang::tok::TokenKind>;
/*
 * 5.5 Operators
 * */
#define TEST_PUNCTUATION(TEST_NAME, TOKEN_KIND, KIND_TEXT)                     \
  TEST_F(LexerTest, Lex_Operators_Punctuation_##TEST_NAME) {                   \
    eTokns_t eTokens;                                                          \
    eTokens.push_back(TOKEN_KIND);                                             \
    auto &test = KIND_TEXT;                                                    \
    CheckLex(test, eTokens);                                                   \
  }

TEST_PUNCTUATION(Plus, svlang::tok::_PLUS, "+")
TEST_PUNCTUATION(Plus_Plus, svlang::tok::_PLUS_PLUS, "++")
TEST_PUNCTUATION(Plus_Equal, svlang::tok::_PLUS_EQUAL, "+=")
TEST_PUNCTUATION(Plus_Colon, svlang::tok::_PLUS_COLON, "+:")

TEST_PUNCTUATION(Equal, svlang::tok::_EQUAL, "=")
TEST_PUNCTUATION(Equal_Equal, svlang::tok::_EQUAL_EQUAL, "==")
TEST_PUNCTUATION(Equal_Equal_Equal, svlang::tok::_EQUAL_EQUAL_EQUAL, "===")
TEST_PUNCTUATION(Equal_Equal_Question, svlang::tok::_EQUAL_EQUAL_QUESTION,
                 "==?")

TEST_PUNCTUATION(Minus, svlang::tok::_MINUS, "-");
TEST_PUNCTUATION(Minus_Minus, svlang::tok::_MINUS_MINUS, "--");
TEST_PUNCTUATION(Minus_Equal, svlang::tok::_MINUS_EQUAL, "-=");
TEST_PUNCTUATION(Minus_Colon, svlang::tok::_MINUS_COLON, "-:");
TEST_PUNCTUATION(Minus_Minus_Greater, svlang::tok::_MINUS_GREATER_GREATER,
                 "->>");
TEST_PUNCTUATION(Minus_Arrow, svlang::tok::_ARROW, "->");

TEST_PUNCTUATION(Star, svlang::tok::_STAR, "*");
TEST_PUNCTUATION(Star_Star, svlang::tok::_STAR_STAR, "**");
TEST_PUNCTUATION(Star_Equal, svlang::tok::_STAR_EQUAL, "*=");
TEST_PUNCTUATION(Star_Greater, svlang::tok::_STAR_GREATER, "*>");
TEST_PUNCTUATION(Star_R_PAREN, svlang::tok::_STAR_R_PAREN, "*)");

TEST_PUNCTUATION(Slash, svlang::tok::_SLASH, "/");
TEST_PUNCTUATION(Slash_Equal, svlang::tok::_SLASH_EQUAL, "/=");

TEST_PUNCTUATION(Percent, svlang::tok::_PERCENT, "%");
TEST_PUNCTUATION(Percent_Equal, svlang::tok::_PERCENT_EQUAL, "%=");

TEST_PUNCTUATION(Amp, svlang::tok::_AMP, "&");
TEST_PUNCTUATION(Amp_Amp, svlang::tok::_AMP_AMP, "&&");
TEST_PUNCTUATION(Amp_Amp_Amp, svlang::tok::_AMP_AMP_AMP, "&&&");
TEST_PUNCTUATION(Amp_Equal, svlang::tok::_AMP_EQUAL, "&=");

TEST_PUNCTUATION(Pipe, svlang::tok::_PIPE, "|");
TEST_PUNCTUATION(Pipe_Pipe, svlang::tok::_PIPE_PIPE, "||");
TEST_PUNCTUATION(Pipe_Minus_Greater, svlang::tok::_PIPE_MINUS_GREATER, "|->");
TEST_PUNCTUATION(Pipe_Equal_greater, svlang::tok::_PIPE_EQUAL_GREATER, "|=>");
TEST_PUNCTUATION(Pipe_Equal, svlang::tok::_PIPE_EQUAL, "|=");

TEST_PUNCTUATION(Caret, svlang::tok::_CARET, "^");
TEST_PUNCTUATION(Caret_Tilde, svlang::tok::_CARET_TILDE, "^~");
TEST_PUNCTUATION(Caret_Equal, svlang::tok::_CARET_EQUAL, "^=");

TEST_PUNCTUATION(Less, svlang::tok::_LESS, "<");
TEST_PUNCTUATION(Less_Equal, svlang::tok::_LESS_EQUAL, "<=");
TEST_PUNCTUATION(Less_Minus_Greater, svlang::tok::_LESS_MINUS_GREATER, "<->");
TEST_PUNCTUATION(Less_Less, svlang::tok::_LESS_LESS, "<<");
TEST_PUNCTUATION(Less_Less_Less, svlang::tok::_LESS_LESS_LESS, "<<<");
TEST_PUNCTUATION(Less_Less_Less_Equal, svlang::tok::_LESS_LESS_LESS_EQUAL,
                 "<<<=");
TEST_PUNCTUATION(Less_Less_Equal, svlang::tok::_LESS_LESS_EQUAL, "<<=");

TEST_PUNCTUATION(Greater, svlang::tok::_GREATER, ">");
TEST_PUNCTUATION(Greater_Equal, svlang::tok::_GREATER_EQUAL, ">=");
TEST_PUNCTUATION(Greater_Greater, svlang::tok::_GREATER_GREATER, ">>");
TEST_PUNCTUATION(Greater_Greater_Greater, svlang::tok::_GREATER_GREATER_GREATER,
                 ">>>");
TEST_PUNCTUATION(Greater_Greater_Equal, svlang::tok::_GREATER_GREATER_EQUAL,
                 ">>=");
TEST_PUNCTUATION(Greater_Greater_Greater_Equal,
                 svlang::tok::_GREATER_GREATER_GREATER_EQUAL, ">>>=");

TEST_PUNCTUATION(Question, svlang::tok::_QUESTION, "?");

TEST_PUNCTUATION(Tilde, svlang::tok::_TILDE, "~");
TEST_PUNCTUATION(Tilde_Amp, svlang::tok::_TILDE_AMP, "~&");
TEST_PUNCTUATION(Tilde_Pipe, svlang::tok::_TILDE_PIPE, "~|");
TEST_PUNCTUATION(Tilde_Caret, svlang::tok::_TILDE_CARET, "~^");

TEST_PUNCTUATION(Exclaim, svlang::tok::_EXCLAIM, "!");
TEST_PUNCTUATION(Exclaim_Equal, svlang::tok::_EXCLAIM_EQUAL, "!=");
TEST_PUNCTUATION(Exclaim_Equal_Equal, svlang::tok::_EXCLAIM_EQUAL_EQUAL, "!==");
TEST_PUNCTUATION(Exclaim_Equal_Question, svlang::tok::_EXCLAIM_EQUAL_QUESTION,
                 "!=?");

TEST_PUNCTUATION(Colon, svlang::tok::_COLON, ":");
TEST_PUNCTUATION(Colon_Equal, svlang::tok::_COLON_EQUAL, ":=");
TEST_PUNCTUATION(Colon_Slash, svlang::tok::_COLON_SLASH, ":/");
TEST_PUNCTUATION(Colon_Colon, svlang::tok::_COLON_COLON, "::");

TEST_PUNCTUATION(Apostrophe_L_Brace, svlang::tok::_APOSTROPHE_L_BRACE, "'{");

TEST_PUNCTUATION(L_Paren, svlang::tok::_L_PAREN, "(");
TEST_PUNCTUATION(L_Paren_Star, svlang::tok::_L_PAREN_STAR, "(*");

TEST_PUNCTUATION(R_Paren, svlang::tok::_R_PAREN, ")");

TEST_PUNCTUATION(L_Brace, svlang::tok::_L_BRACE, "{");

TEST_PUNCTUATION(R_Brace, svlang::tok::_R_BRACE, "}");

TEST_PUNCTUATION(At, svlang::tok::_AT, "@");
TEST_PUNCTUATION(At_At, svlang::tok::_AT_AT, "@@");

TEST_PUNCTUATION(Semi, svlang::tok::_SEMI, ";");

TEST_PUNCTUATION(Hash, svlang::tok::_HASH, "#");
TEST_PUNCTUATION(Hash_Hash, svlang::tok::_HASH_HASH, "##");
TEST_PUNCTUATION(Hash_Minus_Hash, svlang::tok::_HASH_MINUS_HASH, "#-#");
TEST_PUNCTUATION(Hash_Equal_Hash, svlang::tok::_HASH_EQUAL_HASH, "#=#");

TEST_PUNCTUATION(L_Square, svlang::tok::_L_SQUARE, "[");

TEST_PUNCTUATION(R_Square, svlang::tok::_R_SQUARE, "]");

TEST_PUNCTUATION(Period, svlang::tok::_PERIOD, ".");
TEST_PUNCTUATION(Period_Star, svlang::tok::_PERIOD_STAR, ".*");

TEST_PUNCTUATION(Dollar, svlang::tok::_DOLLAR, "$");

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

/*
 * 5.6 Identifiers, keywords, and system names
 * */

// example
TEST_F(LexerTest, Lexer_Identifiers_Identifiers) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_IDENTIFIER);
  eTokens.push_back(svlang::tok::_IDENTIFIER);
  eTokens.push_back(svlang::tok::_IDENTIFIER);
  eTokens.push_back(svlang::tok::_IDENTIFIER);
  eTokens.push_back(svlang::tok::_IDENTIFIER);
  eTokens.push_back(svlang::tok::_IDENTIFIER);

  auto &test = "shiftreg_a\n"
               "busa_index\n"
               "error_condition\n"
               "merge_ab\n"
               "_bus3\n"
               "n$657\n";

  CheckLex(test, eTokens);
}

// 5.6.1 Escaped identifiers
TEST_F(LexerTest, Lexer_Identifiers_Escaped_Identifiers) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);
  eTokens.push_back(svlang::tok::_ESCAPED_IDENTIFIER);

  auto &test = R"(\busa+index
\-clock
\***error-condition***
\net1/\net2
\{a,b}
\a*(b+c)
)";

  CheckLex(test, eTokens);
}

// 5.6.3 System tasks and system functions
TEST_F(LexerTest, Lexer_Identifiers_System_tf_identifier) {
  std::vector<svlang::tok::TokenKind> eTokens;
  eTokens.push_back(svlang::tok::_SYSTEM_TF_IDENTIFIER);
  eTokens.push_back(svlang::tok::_SYSTEM_TF_IDENTIFIER);

  auto &test = "$display\n"
               "$finish";

  CheckLex(test, eTokens);
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

/*
 * 5.9 String literals
 * */
// Example 1:
TEST_F(LexerTest, Lex_String_Literals_A_backslash) {
  std::vector<svlang::tok::TokenKind> eTokens{1, svlang::tok::_STRING_LITERAL};
  auto &test =
      R"("Humpty Dumpty sat on a wall. \
Humpty Dumpty had a great fall")";
  cmpString(CheckLex(test, eTokens)[0],
            "Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall");
}

// Example 2:
TEST_F(LexerTest, Lex_String_Literals_Escape_string_before_backslash) {
  std::vector<svlang::tok::TokenKind> eTokens{1, svlang::tok::_STRING_LITERAL};
  auto &test =
      R"("Humpty Dumpty sat on a wall.\n\
Humpty Dumpty had a great fall")";
  cmpString(CheckLex(test, eTokens)[0],
            "Humpty Dumpty sat on a wall.\nHumpty Dumpty had a great fall");
}

// Example 3:  Special characters in strings
// \ddd
TEST_F(LexerTest, Lex_String_Literals_Escape_string_ddd) {
  std::vector<svlang::tok::TokenKind> eTokens{1, svlang::tok::_STRING_LITERAL};
  auto &test = R"("\110\145\154\154\157\40\127\157\162\154\144")";
  cmpString(CheckLex(test, eTokens)[0], "Hello World");
}

// \xdd
TEST_F(LexerTest, Lex_String_Literals_Escape_string_xdd) {
  std::vector<svlang::tok::TokenKind> eTokens{1, svlang::tok::_STRING_LITERAL};
  auto &test = R"("\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64")";
  cmpString(CheckLex(test, eTokens)[0], "Hello World");
}

// Others
TEST_F(LexerTest, Lex_String_Literals_Escape_string_others) {
  std::vector<svlang::tok::TokenKind> eTokens{1, svlang::tok::_STRING_LITERAL};
  auto &test = R"("\"Hello World\" \\n\\t\\v\\f\\a")";
  cmpString(CheckLex(test, eTokens)[0], "\"Hello World\" \\n\\t\\v\\f\\a");
}
