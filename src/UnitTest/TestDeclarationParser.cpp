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
}

void InvokeDeclarationParserTest()
{
    TestTag();
    TestType();
    std::cout << "Declaration Parser Test Complete" << std::endl;
}
