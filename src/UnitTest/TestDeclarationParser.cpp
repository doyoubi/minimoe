#include <iostream>
#include <string>

#include "Test.h"
#include "Compiler\Parser\DeclarationParser.h"

using std::string;
using namespace minimoe;

void TestTag()
{
    {
        string code =
            "tag MyTag\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto tag = TagDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(tag != nullptr);
        TEST_ASSERT(tag->ToLog() == "Tag(MyTag)");
        TEST_ASSERT(errors.empty());
    }
    {
        string code =
            "tag MyTag doyoubi\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto tag = TagDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(tag != nullptr);
        TEST_ASSERT(tag->ToLog() == "Tag(MyTag)");
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_CanNotParseLeftToken);
    }
    {
        string code =
            "tag\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto tag = TagDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(tag == nullptr);
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_NoMoreToken);
    }
}

void TestType()
{
    {
        string code =
            "type MyType\n"
            "end\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto type = TypeDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(type != nullptr);
        TEST_ASSERT(type->ToLog() == "Type(MyType)");
        TEST_ASSERT(errors.empty());
    }
    {
        string code =
            "type MyType2\n"
            "    mem1\n"
            "    mem2\n"
            "end\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto type = TypeDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(type != nullptr);
        TEST_ASSERT(type->ToLog() == "Type(MyType2, mem1, mem2)");
        TEST_ASSERT(errors.empty());
    }
    {
        string code =
            "type MyType3 ident\n"
            "   mem1 \n"
            "end";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto type = TypeDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(type != nullptr);
        TEST_ASSERT(type->ToLog() == "Type(MyType3, mem1)");
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_CanNotParseLeftToken);
        TEST_ASSERT(errors.front().token->row == 1);
    }
    {
        string code =
            "type\n"
            "end";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto type = TypeDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(type == nullptr);
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_NoMoreToken);
    }
}

void TestArgument()
{
    {
        string code = "(arg1)";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(codeFile->lines.size() == 1);
        auto tokens = codeFile->lines.front()->tokens;
        auto arg = ArgumentDeclaration::Parse(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(arg != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(arg->ToLog() == "Normal(arg1)");
    }
    {
        string code = "(blockbody arg1)";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(codeFile->lines.size() == 1);
        auto tokens = codeFile->lines.front()->tokens;
        auto arg = ArgumentDeclaration::Parse(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(arg != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(arg->ToLog() == "BlockBody(arg1)");
    }
    {
        string code = "(2333 arg2)";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(codeFile->lines.size() == 1);
        auto tokens = codeFile->lines.front()->tokens;
        auto arg = ArgumentDeclaration::Parse(tokens.begin(), tokens.end(), errors);
        TEST_ASSERT(arg == nullptr);
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_InvalidArgumentDeclaration);
    }
}

void TestFunctionDeclaration()
{
    {
        string code =
        "block print\n"
        "end\n";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto func = FunctionDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(func != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(func->ToLog() == "Block:print(){0}");
        TEST_ASSERT(func->startIter == std::next(codeFile->lines.begin()));
    }
    {
        string code =
            "phrase SumFrom(low)To(high) : SumFrom\n"
            "    result = 1\n"
            "end\n"
            "Tag t";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto func = FunctionDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(func != nullptr);
        TEST_ASSERT(errors.empty());
        TEST_ASSERT(func->ToLog() == "Phrase:SumFrom_To(low, high){1}");
        TEST_ASSERT(func->alias == "SumFrom");
        TEST_ASSERT(func->startIter == std::next(codeFile->lines.begin()));
    }
    {
        string code =
            "sentence echo\n"
            "    print(doyoubi)\n"
            "Tag t";
        CompileError::List errors;
        auto codeFile = CodeFile::Parse(code);
        TEST_ASSERT(errors.empty());
        auto func = FunctionDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(func == nullptr);
        TEST_ASSERT(errors.size() == 1);
        TEST_ASSERT(errors.front().errorType == CompileErrorType::Parser_ExpectEndForFunctionDeclaration);
    }
}

void InvokeDeclarationParserTest()
{
    TestTag();
    TestType();
    TestArgument();
    TestFunctionDeclaration();
    std::cout << "Declaration Parser Test Complete" << std::endl;
}
