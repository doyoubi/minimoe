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
        const string & file, size_t line, CodeTokenType type = CodeTokenType::UnKnown);
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
    const string & file, size_t line, CodeTokenType type)
{
    test_assert(errorIterator->errorType == errorType, file, line);
    test_assert(errorIterator->token->row == row, file, line);
    test_assert(errorIterator->token->column == column, file, line);
    test_assert(errorIterator->token->value == value, file, line);
    test_assert(errorIterator->token->type == type, file, line);
    ++errorIterator;
}
void LexerTester::end_check_error(const string & file, size_t line)
{
    test_assert(errorIterator == codeFile->errors.end(), file, line);
}

#define FIRST_LINE(code, COUNT)                                       LexerTester tester(code); tester.FirstLine(COUNT, __FILE__, __LINE__);
#define NEXT_LINE                                                     tester.NextLine(__FILE__, __LINE__);
#define LAST_LINE                                                     tester.LastLine(__FILE__, __LINE__);
#define FIRST_TOKEN(COUNT)                                            tester.FirstToken(COUNT, __FILE__, __LINE__);
#define TOKEN(ROW, COLUMN, VALUE, TYPE)                               tester.Token(ROW, COLUMN, VALUE, TYPE, __FILE__, __LINE__);
#define LAST_TOKEN                                                    tester.LastToken(__FILE__, __LINE__);
#define BEGIN_CHECK_ERROR(COUNT)                                      tester.begin_check_error(COUNT, __FILE__, __LINE__);
#define CHECK_ERROR(ERRORTYPE, ROW, COLUMN, VALUE)                    tester.error(ERRORTYPE, ROW, COLUMN, VALUE, __FILE__, __LINE__);
#define CHECK_ERROR_WITH_TYPE(ERRORTYPE, ROW, COLUMN, VALUE, TYPE)    tester.error(ERRORTYPE, ROW, COLUMN, VALUE, __FILE__, __LINE__, TYPE);
#define END_CHECK_ERROR                                               tester.end_check_error(__FILE__, __LINE__);

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
    TOKEN(1, 1, "1", CodeTokenType::IntegerLiteral);
    TOKEN(1, 2, "+", CodeTokenType::Add);
    TOKEN(1, 3, "2", CodeTokenType::IntegerLiteral);
    TOKEN(1, 4, "*", CodeTokenType::Mul);
    TOKEN(1, 5, "*", CodeTokenType::Mul);
    TOKEN(1, 6, "3", CodeTokenType::IntegerLiteral);
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
    TOKEN(1, 1, "111", CodeTokenType::IntegerLiteral);
    TOKEN(1, 4, "+", CodeTokenType::Add);
    TOKEN(1, 5, "2222", CodeTokenType::IntegerLiteral);
    TOKEN(1, 9, "+", CodeTokenType::Add);
    TOKEN(1, 10, "12345", CodeTokenType::IntegerLiteral);
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
    TOKEN(1, 1, "1", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(2, 1, "2", CodeTokenType::IntegerLiteral);
    TOKEN(2, 2, "+", CodeTokenType::Add);
    TOKEN(2, 3, "1", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(5);
    TOKEN(3, 1, "3", CodeTokenType::IntegerLiteral);
    TOKEN(3, 2, "+", CodeTokenType::Add);
    TOKEN(3, 3, "2", CodeTokenType::IntegerLiteral);
    TOKEN(3, 4, "+", CodeTokenType::Add);
    TOKEN(3, 5, "1", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(5, 1, "5", CodeTokenType::IntegerLiteral);
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
    TOKEN(1, 1, "1", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(2, 2, "2", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(3, 2, "3", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(4, 1, "1", CodeTokenType::IntegerLiteral);
    TOKEN(4, 3, "+", CodeTokenType::Add);
    TOKEN(4, 5, "2", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(5, 2, "1", CodeTokenType::IntegerLiteral);
    TOKEN(5, 3, "+", CodeTokenType::Add);
    TOKEN(5, 5, "2", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(3);
    TOKEN(6, 2, "1", CodeTokenType::IntegerLiteral);
    TOKEN(6, 4, "+", CodeTokenType::Add);
    TOKEN(6, 5, "2", CodeTokenType::IntegerLiteral);
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
    TOKEN(3, 2, "1", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(2);
    TOKEN(6, 1, "2", CodeTokenType::IntegerLiteral);
    TOKEN(6, 3, "3", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(8, 1, "4", CodeTokenType::IntegerLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testError()
{
    string code = "1$2##3";
    FIRST_LINE(code, 1);
    FIRST_TOKEN(3);
    TOKEN(1, 1, "1", CodeTokenType::IntegerLiteral);
    TOKEN(1, 3, "2", CodeTokenType::IntegerLiteral);
    TOKEN(1, 6, "3", CodeTokenType::IntegerLiteral);
    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(3);
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 2, "$");
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 4, "#");
    CHECK_ERROR(CompileErrorType::Lexer_UnexpectedChar, 1, 5, "#");
    END_CHECK_ERROR;
}

void testFloat()
{
    {
        string code = "2.333 0.0";
        FIRST_LINE(code, 1);
        FIRST_TOKEN(2);
        TOKEN(1, 1, "2.333", CodeTokenType::FloatLiteral);
        TOKEN(1, 7, "0.0", CodeTokenType::FloatLiteral);
        LAST_TOKEN;

        NEXT_LINE;
        LAST_LINE;

        BEGIN_CHECK_ERROR(0);
        END_CHECK_ERROR;
    }
    {
        string code = "12.";
        FIRST_LINE(code, 1);
        FIRST_TOKEN(1);
        TOKEN(1, 1, "12", CodeTokenType::FloatLiteral);
        LAST_TOKEN;

        NEXT_LINE;
        LAST_LINE;

        BEGIN_CHECK_ERROR(1);
        CHECK_ERROR(CompileErrorType::Lexer_InvalidFloat, 1, 1, "12.");
        END_CHECK_ERROR;
    }
}

void testString()
{
    string code = 
        "\"unix end\"\n"
        "\"windows end\"\r\n"
        "\"\"\n"
        "\"two\"\"string\"\n"
        "\"\\a\\b\\f\\r\\t\\v\\\\ \\' \\\" \"\n"
        "\"unfinished line\n"
        // "\"unfinished escape\\\"\n"  Should not be this.
        // There is a contradiction here. If we support check for "abcd\", we can't support "ab\"cd"
        "\"unfinished escape \\j end\"\n"
        "\"last line\""
        ;
    FIRST_LINE(code, 7);
    FIRST_TOKEN(1);
    TOKEN(1, 1, "unix end", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(2, 1, "windows end", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(3, 1, "", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(2);
    TOKEN(4, 1, "two", CodeTokenType::StringLiteral);
    TOKEN(4, 6, "string", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(5, 1, "\a\b\f\r\t\v\\ ' \" ", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(7, 1, "unfinished escape \\j end", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    FIRST_TOKEN(1);
    TOKEN(8, 1, "last line", CodeTokenType::StringLiteral);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(2);
    CHECK_ERROR(CompileErrorType::Lexer_InCompleteString, 6, 1, "\"unfinished line");
    CHECK_ERROR_WITH_TYPE(CompileErrorType::Lexer_InvalidEscapeChar, 7, 1,
        "unfinished escape \\j end", CodeTokenType::StringLiteral);
    END_CHECK_ERROR;
}

void testIdentifier()
{
    string code = 
        "module using phrase sentence block type\n"
        "cps category expression deferred argument assignable list\n"
        "end and or not\n"
        "UserDefinedIdentifier "
        ;
    FIRST_LINE(code, 4);
    FIRST_TOKEN(6);
    TOKEN(1, 1, "module", CodeTokenType::Module);
    TOKEN(1, 8, "using", CodeTokenType::Using);
    TOKEN(1, 14, "phrase", CodeTokenType::Phrase);
    TOKEN(1, 21, "sentence", CodeTokenType::Sentence);
    TOKEN(1, 30, "block", CodeTokenType::Block);
    TOKEN(1, 36, "type", CodeTokenType::Type);
    LAST_TOKEN;
    NEXT_LINE;

    FIRST_TOKEN(7);
    TOKEN(2, 1, "cps", CodeTokenType::CPS);
    TOKEN(2, 5, "category", CodeTokenType::Category);
    TOKEN(2, 14, "expression", CodeTokenType::Identifier);
    TOKEN(2, 25, "deferred", CodeTokenType::Deferred);
    TOKEN(2, 34, "argument", CodeTokenType::Argument);
    TOKEN(2, 43, "assignable", CodeTokenType::Assignable);
    TOKEN(2, 54, "list", CodeTokenType::List);
    LAST_TOKEN;
    NEXT_LINE;

    FIRST_TOKEN(4);
    TOKEN(3, 1, "end", CodeTokenType::End);
    TOKEN(3, 5, "and", CodeTokenType::And);
    TOKEN(3, 9, "or", CodeTokenType::Or);
    TOKEN(3, 12, "not", CodeTokenType::Not);
    LAST_TOKEN;
    NEXT_LINE;

    FIRST_TOKEN(1);
    TOKEN(4, 1, "UserDefinedIdentifier", CodeTokenType::Identifier);
    LAST_TOKEN;
    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testComment()
{
    string code =
        "--comment1 \n"
        "-- comment2\n"
        "identifier --comment3\n"
        "- -NotComment";
    FIRST_LINE(code, 2);
    FIRST_TOKEN(1);
    TOKEN(3, 1, "identifier", CodeTokenType::Identifier);
    LAST_TOKEN;
    NEXT_LINE;

    FIRST_TOKEN(3);
    TOKEN(4, 1, "-", CodeTokenType::Sub);
    TOKEN(4, 3, "-", CodeTokenType::Sub);
    TOKEN(4, 4, "NotComment", CodeTokenType::Identifier);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void testOperator()
{
    string code = 
        "(),:+-*/%><<=>===<>.--\n"
        "===>[]";
    FIRST_LINE(code, 2);
    FIRST_TOKEN(16);
    TOKEN(1, 1, "(", CodeTokenType::OpenBracket);
    TOKEN(1, 2, ")", CodeTokenType::CloseBracket);
    TOKEN(1, 3, ",", CodeTokenType::Comma);
    TOKEN(1, 4, ":", CodeTokenType::Colon);
    TOKEN(1, 5, "+", CodeTokenType::Add);
    TOKEN(1, 6, "-", CodeTokenType::Sub);
    TOKEN(1, 7, "*", CodeTokenType::Mul);
    TOKEN(1, 8, "/", CodeTokenType::Div);
    TOKEN(1, 9, "%", CodeTokenType::Mod);
    TOKEN(1, 10, ">", CodeTokenType::GT);
    TOKEN(1, 11, "<", CodeTokenType::LT);
    TOKEN(1, 12, "<=", CodeTokenType::LE);
    TOKEN(1, 14, ">=", CodeTokenType::GE);
    TOKEN(1, 16, "==", CodeTokenType::EQ);
    TOKEN(1, 18, "<>", CodeTokenType::NE);
    TOKEN(1, 20, ".", CodeTokenType::GetMember);
    LAST_TOKEN;
    NEXT_LINE;

    FIRST_TOKEN(5);
    TOKEN(2, 1, "==", CodeTokenType::EQ);
    TOKEN(2, 3, "=", CodeTokenType::Assign);
    TOKEN(2, 4, ">", CodeTokenType::GT);
    TOKEN(2, 5, "[", CodeTokenType::OpenSquareBracket);
    TOKEN(2, 6, "]", CodeTokenType::CloseSquareBracket);
    LAST_TOKEN;

    NEXT_LINE;
    LAST_LINE;

    BEGIN_CHECK_ERROR(0);
    END_CHECK_ERROR;
}

void InvokeLexerTest()
{
    testEmptyFile();
    testPrimitiveToken();
    testLongToken();
    testCodeFileStructure();
    testBlank();
    testMultipleLine();
    testError();
    testFloat();
    testString();
    testIdentifier();
    testComment();
    testOperator();
    std::cout << "Lexer Test Complete" << std::endl;
}
