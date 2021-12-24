#include <gtest/gtest.h>

#include <any>
#include <string>
#include <type_traits>
#include <vector>

#include "../src/Lox/AstPrinter.h"
#include "../src/Lox/Expression.h"
#include "../src/Lox/Parser.h"
#include "../src/Lox/Token.h"

using namespace lox::parser;
using Tokens = std::vector<Token>;
using TT = lox::parser::Token::TokenType;

auto printer = AstPrinter();

TEST(ParserTests, TestEmptyTokens) {
  Tokens tokens = {{TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_FALSE(stmts.size());
}

TEST(ParserTests, TestPrimaryFalse) {
  Tokens tokens = {{TT::FALSE, "false", 0}, {TT::SEMICOLON, 0}, {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(false)");
}

TEST(ParserTests, TestPrimaryTrue) {
  Tokens tokens = {{TT::TRUE, "true", 0}, {TT::SEMICOLON, 0}, {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(true)");
}

TEST(ParserTests, TestPrimaryNil) {
  Tokens tokens = {{TT::NIL, "nil", 0}, {TT::SEMICOLON, 0}, {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(nil)");
}

TEST(ParserTests, TestPrimaryNumber) {
  Tokens tokens = {{TT::NUMBER, "400", 0}, {TT::SEMICOLON, 0}, {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(400.000000)");
}

TEST(ParserTests, TestPrimaryNumberPostfix) {
  Tokens tokens = {{TT::NUMBER, "400", 0},
                   {TT::PLUS_PLUS, "++", 0},
                   {TT::SEMICOLON, 0},
                   {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(( ++ 400.000000 ))");
}

TEST(ParserTests, TestPrimaryString) {
  Tokens tokens = {{TT::STRING, "str", 0}, {TT::SEMICOLON, 0}, {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(\"str\")");
}

TEST(ParserTests, TestPrimaryGrouping) {
  Tokens tokens = {{TT::LEFT_PAREN, "(", 0},
                   {TT::STRING, "str", 0},
                   {TT::RIGHT_PAREN, ")", 0},
                   {TT::SEMICOLON, 0},
                   {TT::END, 0}};
  auto p = Parser(tokens);
  auto stmts = p.parse();
  EXPECT_TRUE(stmts.size() == 1);
  EXPECT_EQ(printer.print(stmts[0]), "(( \"str\" ))");
}

// TEST(ParserTests, TestPrimaryGroupingParseErrorMissingRightParen) {
//     Tokens tokens = {
//         {TT::LEFT_PAREN, "(", 0},
//         {TT::STRING, "str", 0},
//         {TT::SEMICOLON, 0},
//         {TT::END, 0}};
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr == nullptr);
// }

// TEST(ParserTests, TestUnaryBang) {
//     Tokens tokens = {
//         {TT::BANG, "!", 0},
//         {TT::TRUE, "true", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);
//     Unary* un = static_cast<Unary*>(expr.get());
//     EXPECT_EQ(un->op.type, TT::BANG);
// }

// TEST(ParserTests, TestUnaryMinus) {
//     Tokens tokens = {
//         {TT::MINUS, "-", 0},
//         {TT::NUMBER, "400", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);
//     Unary* un = static_cast<Unary*>(expr.get());
//     EXPECT_EQ(un->op.type, TT::MINUS);
// }

// TEST(ParserTests, TestUnaryNumberPrefix) {
//     Tokens tokens = {
//         {TT::MINUS_MINUS, "--", 0},
//         {TT::NUMBER, "400", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);
//     Unary* un = static_cast<Unary*>(expr.get());
//     EXPECT_EQ(un->op.type, TT::MINUS_MINUS);
// }

// TEST(ParserTests, TestBinaryFactor) {
//     Tokens tokens = {
//         {TT::NUMBER, "1", 0},
//         {TT::STAR, "*", 0},
//         {TT::NUMBER, "2", 0},
//         {TT::SLASH, "/", 0},
//         {TT::NUMBER, "3", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "( / ( * 1.000000 2.000000 ) 3.000000 )"
//     );
// }

// TEST(ParserTests, TestBinaryTerm) {
//     Tokens tokens = {
//         {TT::NUMBER, "1", 0},
//         {TT::PLUS, "+", 0},
//         {TT::NUMBER, "2", 0},
//         {TT::MINUS, "-", 0},
//         {TT::NUMBER, "3", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "( - ( + 1.000000 2.000000 ) 3.000000 )"
//     );
// }

// TEST(ParserTests, TestBinaryPrecedence) {
//     Tokens tokens = {
//         {TT::NUMBER, "1", 0},
//         {TT::PLUS, "+", 0},
//         {TT::NUMBER, "2", 0},
//         {TT::STAR, "*", 0},
//         {TT::NUMBER, "3", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "( + 1.000000 ( * 2.000000 3.000000 ) )"
//     );
// }

// TEST(ParserTests, TestBinaryComparison) {
//     Tokens tokens = {
//         {TT::NUMBER, "1", 0},
//         {TT::GREATER, ">", 0},
//         {TT::NUMBER, "2", 0},
//         {TT::LESS_EQUAL, "<=", 0},
//         {TT::NUMBER, "3", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "( <= ( > 1.000000 2.000000 ) 3.000000 )"
//     );
// }

// TEST(ParserTests, TestBinaryEquality) {
//     Tokens tokens = {
//         {TT::NUMBER, "1", 0},
//         {TT::EQUAL_EQUAL, "==", 0},
//         {TT::NUMBER, "2", 0},
//         {TT::BANG_EQUAL, "!=", 0},
//         {TT::NUMBER, "3", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "( != ( == 1.000000 2.000000 ) 3.000000 )"
//     );
// }

// TEST(ParserTests, TestConditionTernaryPartial) {
//     Tokens tokens = {
//         {TT::TRUE, "true", 0},
//         {TT::QUESTION, "?", 0},
//         {TT::NUMBER, "1", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "(if (true) 1.000000 )"
//     );
// }

// TEST(ParserTests, TestConditionTernaryFull) {
//     Tokens tokens = {
//         {TT::TRUE, "true", 0},
//         {TT::QUESTION, "?", 0},
//         {TT::NUMBER, "1", 0},
//         {TT::COLON, ":", 0},
//         {TT::NUMBER, "0", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//         result,
//         "(if (true) 1.000000 else 0.000000 )"
//     );
// }

// TEST(ParserTests, TestBlock) {
//     Tokens tokens = {
//         {TT::TRUE, "true", 0},
//         {TT::COMMA, ",", 0},
//         {TT::TRUE, "true", 0},
//         {TT::COMMA, ",", 0},
//         {TT::TRUE, "true", 0},
//         {TT::END, 0},
//     };
//     auto p = Parser(tokens);
//     auto expr = p.parse();
//     EXPECT_TRUE(expr);

//     auto printer = AstPrinter();
//     auto result = printer.print(expr);
//     EXPECT_EQ(
//        result, "( true, true, true, )"
//     );
// }