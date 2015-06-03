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
}

void InvokeExpressionParserTest()
{
    TestParsePrimitive();
    std::cout << "Expresion Parser Test Complete" << std::endl;
}
