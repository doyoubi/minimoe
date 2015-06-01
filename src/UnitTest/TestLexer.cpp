#include <string>

#include "Compiler/Lexer/Lexer.h"
#include "UnitTest/Test.h"

using std::string;
using std::vector;
using namespace minimoe;

struct LexerTester
{
    CodeFile::Ptr codeFile;
    CodeLine::List::iterator lineIterator;
    CodeToken::List::iterator tokenIterator;
    CompileError::List::iterator errorIterator;

    LexerTester(const string & code)
        : codeFile(CodeFile::Parse(code))
    {
    }

    void FirstLine(size_t count, const string & file, size_t line);
    void NextLine(const string & file, size_t line);
    void LastLine(const string & file, size_t line);
    void FirstToken(size_t count, const string & file, size_t line);
    void Token(size_t row, size_t column, const string & value, CodeTokenType type, const string & file, size_t line);
    void LastToken(const string & file, size_t line);
    void begin_check_error(size_t count, const string & file, size_t line);
    void error(CompileErrorType errorType, size_t row, size_t column, const string & value,
        const string & file, size_t line);
    void end_check_error(const string & file, size_t line);
};

void LexerTester::FirstLine(size_t count, const string & file, size_t line)
{
    test_assert(codeFile->lines.size() == count, file, line);
    lineIterator = codeFile->lines.begin();
}
void LexerTester::NextLine(const string & file, size_t line)
{
    lineIterator++;
}
void LexerTester::LastLine(const string & file, size_t line)
{
    test_assert(lineIterator == codeFile->lines.end(), file, line);
}
void LexerTester::FirstToken(size_t count, const string & file, size_t line)
{
    auto & codeLine = *lineIterator;
    test_assert(codeLine->tokens.size() == count, file, line);
    tokenIterator = codeLine->tokens.begin();
}
void LexerTester::Token(size_t row, size_t column, const string & value, CodeTokenType type,
    const string & file, size_t line)
{
    auto token = *tokenIterator;
    test_assert(tokenIterator != (*lineIterator)->tokens.end(), file, line);
    test_assert(token->row == row, file, line);
    test_assert(token->column == column, file, line);
    test_assert(token->value == value, file, line);
    test_assert(token->type == type, file, line);
    tokenIterator++;
}
void LexerTester::LastToken(const string & file, size_t line)
{
    test_assert(tokenIterator == (*lineIterator)->tokens.end(), file, line);
}
void LexerTester::begin_check_error(size_t count, const string & file, size_t line)
{
    test_assert(codeFile->errors.size() == count, file, line);
    errorIterator = codeFile->errors.begin();
}
void LexerTester::error(CompileErrorType errorType, size_t row, size_t column, const string & value,
    const string & file, size_t line)
{
    test_assert(errorIterator->errorType == errorType, file, line);
    test_assert(errorIterator->token->row == row, file, line);
    test_assert(errorIterator->token->column == column, file, line);
    test_assert(errorIterator->token->value == value, file, line);
    test_assert(errorIterator->token->type == CodeTokenType::UnKnown, file, line);
    ++errorIterator;
}
void LexerTester::end_check_error(const string & file, size_t line)
{
    test_assert(errorIterator == codeFile->errors.end(), file, line);
}

#define FIRST_LINE(code, COUNT)                             LexerTester tester(code); tester.FirstLine(COUNT, __FILE__, __LINE__);
#define NEXT_LINE                                           tester.NextLine(__FILE__, __LINE__);
#define LAST_LINE                                           tester.LastLine(__FILE__, __LINE__);
#define FIRST_TOKEN(COUNT)                                  tester.FirstToken(COUNT, __FILE__, __LINE__);
#define TOKEN(ROW, COLUMN, VALUE, TYPE)                     tester.Token(ROW, COLUMN, VALUE, TYPE, __FILE__, __LINE__);
#define LAST_TOKEN                                          tester.LastToken(__FILE__, __LINE__);
#define BEGIN_CHECK_ERROR(COUNT)                            tester.begin_check_error(COUNT, __FILE__, __LINE__);
#define CHECK_ERROR(ERRORTYPE, ROW, COLUMN, VALUE)          tester.error(ERRORTYPE, ROW, COLUMN, VALUE, __FILE__, __LINE__);
#define END_CHECK_ERROR                                     tester.end_check_error(__FILE__, __LINE__);

void testEmptyFile()
{
    string code = "";
    FIRST_LINE(code, 0);
    LAST_LINE;
    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testPrimitiveToken()
{
    string code = "1+2**3\n";
    FIRST_LINE(code, 1);
    FIRST_TOKEN(6);
    TOKEN(1, 1, "1", CodeTokenType::Integer);
    TOKEN(1, 2, "+", CodeTokenType::Add);
    TOKEN(1, 3, "2", CodeTokenType::Integer);
    TOKEN(1, 4, "*", CodeTokenType::Mul);
    TOKEN(1, 5, "*", CodeTokenType::Mul);
    TOKEN(1, 6, "3", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testLongToken()
{
    string code =
        "111+2222+12345";
    FIRST_LINE(code, 1);
    FIRST_TOKEN(5);
    TOKEN(1, 1, "111", CodeTokenType::Integer);
    TOKEN(1, 4, "+", CodeTokenType::Add);
    TOKEN(1, 5, "2222", CodeTokenType::Integer);
    TOKEN(1, 9, "+", CodeTokenType::Add);
    TOKEN(1, 10, "12345", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testCodeFileStructure()
{
    string code =
        "1\n"
        "2+1\n"
        "3+2+1\n"
        "\n"
        "5\n";
    FIRST_LINE(code, 4);
    FIRST_TOKEN(1);
    TOKEN(1, 1, "1", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(2, 1, "2", CodeTokenType::Integer);
    TOKEN(2, 2, "+", CodeTokenType::Add);
    TOKEN(2, 3, "1", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(5);
    TOKEN(3, 1, "3", CodeTokenType::Integer);
    TOKEN(3, 2, "+", CodeTokenType::Add);
    TOKEN(3, 3, "2", CodeTokenType::Integer);
    TOKEN(3, 4, "+", CodeTokenType::Add);
    TOKEN(3, 5, "1", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(5, 1, "5", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testBlank()
{
    string code =
        "1 \n"
        " 2\n"
        " 3 \n"
        "1 + 2\n"
        " 1+ 2\n"
        " 1 +2\n";
    FIRST_LINE(code, 6);
    FIRST_TOKEN(1);
    TOKEN(1, 1, "1", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(2, 2, "2", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(3, 2, "3", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(4, 1, "1", CodeTokenType::Integer);
    TOKEN(4, 3, "+", CodeTokenType::Add);
    TOKEN(4, 5, "2", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(5, 2, "1", CodeTokenType::Integer);
    TOKEN(5, 3, "+", CodeTokenType::Add);
    TOKEN(5, 5, "2", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(6, 2, "1", CodeTokenType::Integer);
    TOKEN(6, 4, "+", CodeTokenType::Add);
    TOKEN(6, 5, "2", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testMultipleLine()
{
    string code =
        "\n"
        "\n"
        " 1 \n"
        "\n"
        "\n"
        "2 3\n"
        "\n"
        "4\n";
    FIRST_LINE(code, 3);
    FIRST_TOKEN(1);
    TOKEN(3, 2, "1", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(2);
    TOKEN(6, 1, "2", CodeTokenType::Integer);
    TOKEN(6, 3, "3", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(8, 1, "4", CodeTokenType::Integer);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testError()
{
    string code = "1a2bb3";
    FIRST_LINE(code, 1);
    FIRST_TOKEN(3);
    TOKEN(1, 1, "1", CodeTokenType::Integer);
    TOKEN(1, 3, "2", CodeTokenType::Integer);
    TOKEN(1, 6, "3", CodeTokenType::Integer);
    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(3);
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 2, "a");
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 4, "b");
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 5, "b");
    END_CHECK_ERROR;
}

struct InvokeLexerTest
{
    InvokeLexerTest()
    {
        testEmptyFile();
        testPrimitiveToken();
        testLongToken();
        testCodeFileStructure();
        testBlank();
        testMultipleLine();
        testError();
        std::cout << "Lexer Test Complete" << std::endl;
    }
} invokeTest;
