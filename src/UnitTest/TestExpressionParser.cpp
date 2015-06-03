#include <iostream>
#include <string>

#include "Compiler/Parser/ExpressionParser.h"
#include "Compiler/Lexer/Lexer.h"
#include "Test.h"

using std::string;
using namespace minimoe;

void Tokenize(const string & codeString, CodeToken::List & tokens)
{
    auto codeFile = CodeFile::Parse(codeString);
    TEST_ASSERT(codeFile->errors.empty());
    TEST_ASSERT(codeFile->lines.size() == 1);
    tokens = codeFile->lines.back()->tokens;
}

void TestParsePrimitive()
{
    CodeToken::List tokens;
    SymbolStack stack;

    // Integer
    {
        Tokenize("233", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto literal = std::dynamic_pointer_cast<LiteralExpression>(exp);
        TEST_ASSERT(literal->type == LiteralType::Integer);
        TEST_ASSERT(literal->value == "233");
    }

    // Float
    {
        Tokenize("2.333", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto literal = std::dynamic_pointer_cast<LiteralExpression>(exp);
        TEST_ASSERT(literal->type == LiteralType::Float);
        TEST_ASSERT(literal->value == "2.333");
    }

    // String
    {
        Tokenize("\"this is a string\"", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto literal = std::dynamic_pointer_cast<LiteralExpression>(exp);
        TEST_ASSERT(literal->type == LiteralType::String);
        TEST_ASSERT(literal->value == "this is a string");
    }
}

void TestBinaryExpression()
{
    CodeToken::List tokens;
    SymbolStack stack;

    {
        Tokenize("1 and 2", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto bi = std::dynamic_pointer_cast<BinaryExpression>(exp);
        TEST_ASSERT(bi != nullptr);
        TEST_ASSERT(bi->ToLog() == "and(1, 2)");
    }
    {
        Tokenize("1 and 2 or 3 and 4 and 5", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto bi = std::dynamic_pointer_cast<BinaryExpression>(exp);
        TEST_ASSERT(bi != nullptr);
        TEST_ASSERT(bi->ToLog() == "or(and(1, 2), and(and(3, 4), 5))");
    }
}

void InvokeExpressionParserTest()
{
    TestParsePrimitive();
    TestBinaryExpression();
    std::cout << "Expresion Parser Test Complete" << std::endl;
}
