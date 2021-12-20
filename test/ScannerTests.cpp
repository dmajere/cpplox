#include <string_view>
#include <gtest/gtest.h>
#include "../src/Lox/scanner.h"

constexpr std::string_view kNumberInt = "4040";
constexpr std::string_view kNumberFloat = "40.40";
constexpr std::string_view kNumberFloatPartial = "40.";
constexpr std::string_view kString = "abc!@#$%^&*()-=_+{}[]<>,.123";
constexpr std::string_view kStringEmpty = "";
constexpr std::string_view kStringEscapeCharacters = "\\\"\\t\\n";
constexpr std::string_view kStringUnterminated = "unterminated string";
constexpr std::string_view kLiteral = "name123";
constexpr std::string_view kArithmeticExpression = "1 + 2 - 3 * 4 / ( 5  + 6 )";
constexpr std::string_view kLogicalExpression = "a == b and c != d or e > !g";
constexpr std::string_view kConditionExpression = "if (pred) { return result; }";
constexpr std::string_view kTernaryExpression = "pred ? cons : else";
constexpr std::string_view kForLoopExpression = "for (init; pred; inc++) { body; }";
constexpr std::string_view kWhileLoopExpression = "while (pred) { body; }";
constexpr std::string_view kFunctionDefinition = "fun name(a, b) {\n  return a + b;\n}";
constexpr std::string_view kSignleLineComment = "// comment should be ignored";
constexpr std::string_view kMultiLineComment = "/* multi \n line \n comment */";
constexpr std::string_view kClassDefinition = "class Child < Parent {\ninit(name) {\n this.name = name; }\nname() {\n return this.name; }\n}";


TEST(ScannerTests, TestNumberInt) {
    auto s = lox::parser::Scanner(std::string{kNumberInt});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, kNumberInt);
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestNumberFloat) {
    auto s = lox::parser::Scanner(std::string{kNumberFloat});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, kNumberFloat);
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestNumberFloatPartial) {
    auto s = lox::parser::Scanner(std::string{kNumberFloatPartial});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, "40");
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestString) {
    auto s = lox::parser::Scanner("\"" + std::string{kString} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.lexeme, kString);
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestEmptyString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringEmpty} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.lexeme, "");
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestEscapeCharacterString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringEscapeCharacters} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.lexeme, kStringEscapeCharacters);
}

TEST(ScannerTests, TestUnterminatedString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringUnterminated});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.lexeme, "");
}

TEST(ScannerTests, TestNoCode) {
    auto s = lox::parser::Scanner(std::string{kStringEmpty});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 1);
}

TEST(ScannerTests, TestLiteral) {
    auto s = lox::parser::Scanner(std::string{kLiteral});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.type, lox::parser::Token::TokenType::IDENTIFIER);
    EXPECT_EQ(tok.lexeme, kLiteral);
    EXPECT_EQ(tok.line, 1);
}

TEST(ScannerTests, TestArithmetics) {
    auto s = lox::parser::Scanner(std::string{kArithmeticExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 14);
    EXPECT_EQ(tokens[2].type, lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tokens[3].type, lox::parser::Token::TokenType::MINUS);
    EXPECT_EQ(tokens[8].type, lox::parser::Token::TokenType::LEFT_PAREN);
}

TEST(ScannerTests, TestLogicalExpression) {
    auto s = lox::parser::Scanner(std::string{kLogicalExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 13);
    EXPECT_EQ(tokens[1].type, lox::parser::Token::TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[3].type, lox::parser::Token::TokenType::AND);
    EXPECT_EQ(tokens[10].type, lox::parser::Token::TokenType::BANG);
}

TEST(ScannerTests, TestConditionExpression) {
    auto s = lox::parser::Scanner(std::string{kConditionExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 10);
}

TEST(ScannerTests, TestTernaryExpression) {
    auto s = lox::parser::Scanner(std::string{kTernaryExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[1].type, lox::parser::Token::TokenType::QUESTION);
    EXPECT_EQ(tokens[3].type, lox::parser::Token::TokenType::COLON);
}

TEST(ScannerTests, TestForLoopExpression) {
    auto s = lox::parser::Scanner(std::string{kForLoopExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 14);
    EXPECT_EQ(tokens[7].type, lox::parser::Token::TokenType::PLUS_PLUS);
}

TEST(ScannerTests, TestWhileLoopExpression) {
    auto s = lox::parser::Scanner(std::string{kWhileLoopExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 9);
}

TEST(ScannerTests, TestFunctionDefinition) {
    auto s = lox::parser::Scanner(std::string{kFunctionDefinition});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 15);
    EXPECT_EQ(tokens[0].type, lox::parser::Token::TokenType::FUN);
    EXPECT_EQ(tokens[2].type, lox::parser::Token::TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[8].type, lox::parser::Token::TokenType::RETURN);
    EXPECT_EQ(tokens[8].line, 2);
    EXPECT_EQ(tokens[13].line, 3);
}

TEST(ScannerTests, TestSingleLineComment) {
    auto s = lox::parser::Scanner(std::string{kSignleLineComment});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 1);
}

TEST(ScannerTests, TestMultiLineComment) {
    auto s = lox::parser::Scanner(std::string(kMultiLineComment));
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 1);
}

TEST(ScannerTests, TestClassDefinition) {
    auto s = lox::parser::Scanner(std::string{kClassDefinition});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 29);
}