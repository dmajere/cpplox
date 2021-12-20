#include <any>
#include <vector>
#include <string>
#include <type_traits>
#include <gtest/gtest.h>

#include "../src/Lox/token.h"
#include "../src/Lox/expression.h"
#include "../src/Lox/parser.h"
#include "../src/Lox/AstPrinter.h"

using namespace lox::parser;
using Tokens = std::vector<Token>;
using TT = lox::parser::Token::TokenType;

TEST(ParserTests, TestEmptyTokens) {
    Tokens tokens = {{TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_FALSE(expr);
}

TEST(ParserTests, TestPrimaryFalse) {
    Tokens tokens = {
        {TT::FALSE, "false", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Literal* lit = static_cast<Literal*>(expr.get());
    EXPECT_FALSE(std::any_cast<bool>(lit->value));
}

TEST(ParserTests, TestPrimaryTrue) {
    Tokens tokens = {
        {TT::TRUE, "true", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Literal* lit = static_cast<Literal*>(expr.get());
    EXPECT_TRUE(std::any_cast<bool>(lit->value));
}

TEST(ParserTests, TestPrimaryNil) {
    Tokens tokens = {
        {TT::NIL, "nil", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Literal* lit = static_cast<Literal*>(expr.get());
    auto& value_type = lit->value.type();
    EXPECT_EQ(value_type, typeid(nullptr));
}

TEST(ParserTests, TestPrimaryNumber) {
    Tokens tokens = {
        {TT::NUMBER, "400", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Literal* lit = static_cast<Literal*>(expr.get());
    EXPECT_EQ(std::any_cast<double>(lit->value), 400.0);
}

TEST(ParserTests, TestPrimaryNumberPostfix) {
    Tokens tokens = {
        {TT::NUMBER, "400", 0},
        {TT::PLUS_PLUS, "++", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Unary* un = static_cast<Unary*>(expr.get());
    EXPECT_EQ(un->op.type, TT::PLUS_PLUS);
}

TEST(ParserTests, TestPrimaryString) {
    Tokens tokens = {
        {TT::STRING, "str", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Literal* lit = static_cast<Literal*>(expr.get());
    EXPECT_EQ(std::any_cast<std::string>(lit->value), "str");
}

TEST(ParserTests, TestPrimaryGrouping) {
    Tokens tokens = {
        {TT::LEFT_PAREN, "(", 0},
        {TT::STRING, "str", 0},
        {TT::RIGHT_PAREN, ")", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);
    Grouping* group = static_cast<Grouping*>(expr.get());
    Literal* lit = static_cast<Literal*>(group->expression.get());
    EXPECT_EQ(std::any_cast<std::string>(lit->value), "str");
}

TEST(ParserTests, TestPrimaryGroupingParseErrorMissingRightParen) {
    Tokens tokens = {
        {TT::LEFT_PAREN, "(", 0},
        {TT::STRING, "str", 0},
        {TT::END, 0}};
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr == nullptr);
}

TEST(ParserTests, TestUnaryBang) {
    Tokens tokens = {
        {TT::BANG, "!", 0},
        {TT::TRUE, "true", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);
    Unary* un = static_cast<Unary*>(expr.get());
    EXPECT_EQ(un->op.type, TT::BANG);
}

TEST(ParserTests, TestUnaryMinus) {
    Tokens tokens = {
        {TT::MINUS, "-", 0},
        {TT::NUMBER, "400", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);
    Unary* un = static_cast<Unary*>(expr.get());
    EXPECT_EQ(un->op.type, TT::MINUS);
}

TEST(ParserTests, TestUnaryNumberPrefix) {
    Tokens tokens = {
        {TT::MINUS_MINUS, "--", 0},
        {TT::NUMBER, "400", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);
    Unary* un = static_cast<Unary*>(expr.get());
    EXPECT_EQ(un->op.type, TT::MINUS_MINUS);
}

TEST(ParserTests, TestBinaryFactor) {
    Tokens tokens = {
        {TT::NUMBER, "1", 0},
        {TT::STAR, "*", 0},
        {TT::NUMBER, "2", 0},
        {TT::SLASH, "/", 0},
        {TT::NUMBER, "3", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Binary* bin = static_cast<Binary*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(bin);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "( / ( * 1.000000 2.000000 ) 3.000000 )"
    );
}

TEST(ParserTests, TestBinaryTerm) {
    Tokens tokens = {
        {TT::NUMBER, "1", 0},
        {TT::PLUS, "+", 0},
        {TT::NUMBER, "2", 0},
        {TT::MINUS, "-", 0},
        {TT::NUMBER, "3", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Binary* bin = static_cast<Binary*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(bin);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "( - ( + 1.000000 2.000000 ) 3.000000 )"
    );
}

TEST(ParserTests, TestBinaryPrecedence) {
    Tokens tokens = {
        {TT::NUMBER, "1", 0},
        {TT::PLUS, "+", 0},
        {TT::NUMBER, "2", 0},
        {TT::STAR, "*", 0},
        {TT::NUMBER, "3", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Binary* bin = static_cast<Binary*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(bin);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "( + 1.000000 ( * 2.000000 3.000000 ) )"
    );
}

TEST(ParserTests, TestBinaryComparison) {
    Tokens tokens = {
        {TT::NUMBER, "1", 0},
        {TT::GREATER, ">", 0},
        {TT::NUMBER, "2", 0},
        {TT::LESS_EQUAL, "<=", 0},
        {TT::NUMBER, "3", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Binary* bin = static_cast<Binary*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(bin);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "( <= ( > 1.000000 2.000000 ) 3.000000 )"
    );
}

TEST(ParserTests, TestBinaryEquality) {
    Tokens tokens = {
        {TT::NUMBER, "1", 0},
        {TT::EQUAL_EQUAL, "==", 0},
        {TT::NUMBER, "2", 0},
        {TT::BANG_EQUAL, "!=", 0},
        {TT::NUMBER, "3", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Binary* bin = static_cast<Binary*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(bin);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "( != ( == 1.000000 2.000000 ) 3.000000 )"
    );
}

TEST(ParserTests, TestConditionTernaryPartial) {
    Tokens tokens = {
        {TT::TRUE, "true", 0},
        {TT::QUESTION, "?", 0},
        {TT::NUMBER, "1", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Condition* cond = static_cast<Condition*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(cond);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "(if (true) 1.000000 )"
    );
}

TEST(ParserTests, TestConditionTernaryFull) {
    Tokens tokens = {
        {TT::TRUE, "true", 0},
        {TT::QUESTION, "?", 0},
        {TT::NUMBER, "1", 0},
        {TT::COLON, ":", 0},
        {TT::NUMBER, "0", 0},
        {TT::END, 0},
    };
    auto p = Parser(tokens);
    auto expr = p.parse();
    EXPECT_TRUE(expr);

    Condition* cond = static_cast<Condition*>(expr.get());
    auto printer = AstPrinter();
    auto result = printer.visit(cond);
    EXPECT_EQ(
        std::any_cast<std::string>(result),
        "(if (true) 1.000000 else 0.000000 )"
    );
}