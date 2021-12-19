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
constexpr std::string_view kFunctionDefinition = "fun name(a, b) {\n  return a + b;\n}";
constexpr std::string_view kSignleLineComment = "// comment should be ignored";
constexpr std::string_view kMultiLineComment = "/* multi \n line \n comment */";
constexpr std::string_view kClassDefinition = "class Child < Parent {\ninit(name) {\n this.name = name; }\nname() {\n return this.name; }\n}";


TEST(ScannerTests, TestNumberInt) {
    auto s = lox::parser::Scanner(std::string{kNumberInt});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.Lexeme(), kNumberInt);
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestNumberFloat) {
    auto s = lox::parser::Scanner(std::string{kNumberFloat});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.Lexeme(), kNumberFloat);
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestNumberFloatPartial) {
    auto s = lox::parser::Scanner(std::string{kNumberFloatPartial});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tok.Lexeme(), "40");
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestString) {
    auto s = lox::parser::Scanner("\"" + std::string{kString} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.Lexeme(), kString);
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestEmptyString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringEmpty} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.Lexeme(), "");
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestEscapeCharacterString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringEscapeCharacters} + "\"");
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.Lexeme(), kStringEscapeCharacters);
}

TEST(ScannerTests, TestUnterminatedString) {
    auto s = lox::parser::Scanner("\"" + std::string{kStringUnterminated});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::STRING);
    EXPECT_EQ(tok.Lexeme(), "");
}

TEST(ScannerTests, TestNoCode) {
    auto s = lox::parser::Scanner(std::string{kStringEmpty});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 0);
}

TEST(ScannerTests, TestLiteral) {
    auto s = lox::parser::Scanner(std::string{kLiteral});
    auto tokens = s.scanTokens();
    lox::parser::Token tok = tokens[0];
    EXPECT_EQ(tok.Type(), lox::parser::Token::TokenType::IDENTIFIER);
    EXPECT_EQ(tok.Lexeme(), kLiteral);
    EXPECT_EQ(tok.Line(), 1);
}

TEST(ScannerTests, TestArithmetics) {
    auto s = lox::parser::Scanner(std::string{kArithmeticExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 13);
    EXPECT_EQ(tokens[2].Type(), lox::parser::Token::TokenType::NUMBER);
    EXPECT_EQ(tokens[3].Type(), lox::parser::Token::TokenType::MINUS);
    EXPECT_EQ(tokens[8].Type(), lox::parser::Token::TokenType::LEFT_PAREN);
}

TEST(ScannerTests, TestLogicalExpression) {
    auto s = lox::parser::Scanner(std::string{kLogicalExpression});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 12);
    EXPECT_EQ(tokens[1].Type(), lox::parser::Token::TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[3].Type(), lox::parser::Token::TokenType::AND);
    EXPECT_EQ(tokens[10].Type(), lox::parser::Token::TokenType::BANG);
}

TEST(ScannerTests, TestFunctionDefinition) {
    auto s = lox::parser::Scanner(std::string{kFunctionDefinition});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 14);
    EXPECT_EQ(tokens[0].Type(), lox::parser::Token::TokenType::FUN);
    EXPECT_EQ(tokens[2].Type(), lox::parser::Token::TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[8].Type(), lox::parser::Token::TokenType::RETURN);
    EXPECT_EQ(tokens[8].Line(), 2);
    EXPECT_EQ(tokens[13].Line(), 3);
}

TEST(ScannerTests, TestSingleLineComment) {
    auto s = lox::parser::Scanner(std::string{kSignleLineComment});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 0);
}

TEST(ScannerTests, TestMultiLineComment) {
    auto s = lox::parser::Scanner(std::string(kMultiLineComment));
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 0);
}

TEST(ScannerTests, TestClassDefinition) {
    auto s = lox::parser::Scanner(std::string{kClassDefinition});
    auto tokens = s.scanTokens();
    EXPECT_EQ(tokens.size(), 28);
}