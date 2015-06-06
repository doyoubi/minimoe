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
        TEST_ASSERT(errors.size() == 0);
        auto tag = TagDeclaration::Parse(codeFile->lines.begin(), codeFile->lines.end(), errors);
        TEST_ASSERT(tag != nullptr);
        TEST_ASSERT(tag->name == "MyTag");
    }
}

void InvokeDeclarationParserTest()
{
    TestTag();
    std::cout << "Expresion Parser Test Complete" << std::endl;
}
