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

void TestLiteral()
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
    // bracket
    {
        Tokenize("(1)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto literal = std::dynamic_pointer_cast<LiteralExpression>(exp);
        TEST_ASSERT(literal->type == LiteralType::Integer);
        TEST_ASSERT(literal->value == "1");
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

void TestUnaryExpression()
{
    CodeToken::List tokens;
    SymbolStack stack;
    {
        Tokenize("not 1", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto uexp = std::dynamic_pointer_cast<UnaryExpression>(exp);
        TEST_ASSERT(uexp != nullptr);
        TEST_ASSERT(uexp->ToLog() == "not(1)");
    }
    {
        Tokenize("1 and + 2", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto bi = std::dynamic_pointer_cast<BinaryExpression>(exp);
        TEST_ASSERT(bi != nullptr);
        auto u = std::dynamic_pointer_cast<UnaryExpression>(bi->rightOperand);
        TEST_ASSERT(u != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(bi->ToLog() == "and(1, +(2))");
    }
}

void TestVariable()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);

    auto AddVariable = [&stack](Type type, std::string name){
        auto variableItem = std::make_shared<SymbolStackItem>();
        auto declaration = std::make_shared<VariableDeclaration>();
        declaration->type = type;
        variableItem->addSymbol(declaration, name);
        stack.Push(variableItem);
    };

    {
        AddVariable(Type::String, "doyoubi");
        Tokenize("doyoubi 1", tokens);
        CompileError::List errors;
        auto it = tokens.begin();
        auto exp = stack.ParseSymbol(it, tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto sexp = std::dynamic_pointer_cast<SymbolExpression>(exp);
        TEST_ASSERT(sexp != nullptr);
        TEST_ASSERT(sexp->ToLog() == "(doyoubi:String)");
        TEST_ASSERT(sexp->symbol->symbolType == SymbolType::Variable);
        TEST_ASSERT(sexp->symbol == stack.Top()->symbolTables.back());

        auto num = stack.ParsePrimitive(it, tokens.end(), errors);
        TEST_ASSERT(num != nullptr);
        TEST_ASSERT(errors.empty());
        auto one = std::dynamic_pointer_cast<LiteralExpression>(num);
        TEST_ASSERT(one != nullptr);
        TEST_ASSERT(one->ToLog() == "1");
    }
    {
        Tokenize("NotDeclaredVar", tokens);
        CompileError::List errors;
        auto exp = stack.ParseSymbol(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp == nullptr);
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.back().errorType == CompileErrorType::Parser_CanNotResolveSymbol);
        TEST_ASSERT(errors.back().token->value == "NotDeclaredVar");
    }
}

void TestBuiltInValue()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);

    {
        Tokenize("null", tokens);
        CompileError::List errors;
        auto exp = stack.ParseSymbol(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto vexp = std::dynamic_pointer_cast<SymbolExpression>(exp);
        TEST_ASSERT(vexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(vexp->symbol->symbolType == SymbolType::Keyword);
        TEST_ASSERT(vexp->symbol->keyword == Keyword::Null);
        TEST_ASSERT(vexp->symbol->name == "null");
        TEST_ASSERT(vexp->ToLog() == "null");
    }
    {
        Tokenize("true", tokens);
        CompileError::List errors;
        auto exp = stack.ParseSymbol(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto vexp = std::dynamic_pointer_cast<SymbolExpression>(exp);
        TEST_ASSERT(vexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(vexp->symbol->symbolType == SymbolType::Keyword);
        TEST_ASSERT(vexp->symbol->keyword == Keyword::True);
        TEST_ASSERT(vexp->symbol->name == "true");
        TEST_ASSERT(vexp->ToLog() == "true");
    }
    {
        Tokenize("false", tokens);
        CompileError::List errors;
        auto exp = stack.ParseSymbol(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto vexp = std::dynamic_pointer_cast<SymbolExpression>(exp);
        TEST_ASSERT(vexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(vexp->symbol->symbolType == SymbolType::Keyword);
        TEST_ASSERT(vexp->symbol->keyword == Keyword::False);
        TEST_ASSERT(vexp->symbol->name == "false");
        TEST_ASSERT(vexp->ToLog() == "false");
    }
}

void TestBuiltInType()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);

    {
        Tokenize("Integer", tokens);
        CompileError::List errors;
        auto exp = stack.ParseSymbol(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto sexp = std::dynamic_pointer_cast<SymbolExpression>(exp);
        TEST_ASSERT(sexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(sexp->symbol->symbolType == SymbolType::Type);
        TEST_ASSERT(sexp->symbol->builtInType == Type::Integer);
        TEST_ASSERT(sexp->symbol->name == "Integer");
        TEST_ASSERT(sexp->ToLog() == "Integer");
    }
}

void TestFunction()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);

    auto AddFunc = [&stack](FunctionDeclaration::Ptr decl){
        auto funcItem = std::make_shared<SymbolStackItem>();
        funcItem->functionTables.push_back(decl);
        stack.Push(funcItem);
    };

    {
        auto decl = FunctionDeclaration::Make(
            FunctionType::Phrase)->name("func")->arg(FunctionArgumentType::Normal, "param");
        AddFunc(decl);
        Tokenize("func(null)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseInvokeFunction(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto fexp = std::dynamic_pointer_cast<FunctionInvokeExpression>(exp);
        TEST_ASSERT(fexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(fexp->ToLog() == "func(null)");
        stack.Pop();
    }
    {
        auto decl = FunctionDeclaration::Make(
            FunctionType::Phrase)->name("SumFrom")->arg(FunctionArgumentType::Normal, "min")
            ->name("To")->arg(FunctionArgumentType::Normal, "max");
        AddFunc(decl);
        Tokenize("SumFrom(1)To(100)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseInvokeFunction(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto fexp = std::dynamic_pointer_cast<FunctionInvokeExpression>(exp);
        TEST_ASSERT(fexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(fexp->ToLog() == "SumFrom_To(1, 100)");

        Tokenize("SumFrom(1)To", tokens);
        auto e1 = stack.ParseInvokeFunction(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(!errors.empty());
        TEST_ASSERT(errors.back().errorType == CompileErrorType::Parser_NoMoreToken);
        errors.clear();

        Tokenize("SumFrom(1)(100)", tokens);
        auto e2 = stack.ParseInvokeFunction(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(!errors.empty());
        TEST_ASSERT(errors.back().errorType == CompileErrorType::Parser_UnExpectedTokenType);
        errors.clear();

        stack.Pop();
    }
    {
        auto decl = FunctionDeclaration::Make(
            FunctionType::Phrase)->arg(FunctionArgumentType::Normal, "object")
            ->name("IsType")->arg(FunctionArgumentType::Normal, "Type");
        AddFunc(decl);
        Tokenize("(233)IsType(Integer) or true", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());

        auto bi = std::dynamic_pointer_cast<BinaryExpression>(exp);
        TEST_ASSERT(bi != nullptr);
        TEST_ASSERT(errors.empty());

        auto fexp = std::dynamic_pointer_cast<FunctionInvokeExpression>(bi->leftOperand);
        TEST_ASSERT(fexp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(fexp->ToLog() == "IsType(233, Integer)");

        auto t = std::dynamic_pointer_cast<SymbolExpression>(bi->rightOperand);
        TEST_ASSERT(t != nullptr);
        TEST_ASSERT(t->ToLog() == "true");
        stack.Pop();
    }
}

void TestList()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);
    {
        Tokenize("() or true", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(exp->ToLog() == "or(List(), true)");
    }
    {
        Tokenize("(1,)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(exp->ToLog() == "List(1)");
    }
    {
        Tokenize("(1,2)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(exp->ToLog() == "List(1, 2)");
    }
    {
        Tokenize("(1,2,)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseList(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp == nullptr);
        TEST_ASSERT(errors.size() == 1);
        auto error = errors.front();
        TEST_ASSERT(error.errorType == CompileErrorType::Parser_NotOneElementListShouldNotEndWithComma);
    }
    {
        Tokenize("(1)", tokens);
        CompileError::List errors;
        auto exp = stack.ParseList(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp == nullptr);
        TEST_ASSERT(errors.size() == 1);
        auto error = errors.front();
        TEST_ASSERT(error.errorType == CompileErrorType::Parser_OneElementListShouldEndWithComma);
    }
}

void TestComplexExpression()
{
    CodeToken::List tokens;
    SymbolStack stack;
    auto item = std::make_shared<SymbolStackItem>();
    item->LoadPredefinedSymbol();
    stack.Push(item);
    {
        Tokenize("(1,2,3) or (1 and 2) and \"doyoubi\"", tokens);
        CompileError::List errors;
        auto exp = stack.ParseExpression(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(exp != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(exp->ToLog() == "or(List(1, 2, 3), and(and(1, 2), \"doyoubi\"))");
    }
}

void InvokeExpressionParserTest()
{
    TestLiteral();
    TestBinaryExpression();
    TestUnaryExpression();
    TestVariable();
    TestBuiltInValue();
    TestBuiltInType();
    TestFunction();
    TestList();
    TestComplexExpression();
    std::cout << "Expresion Parser Test Complete" << std::endl;
}
