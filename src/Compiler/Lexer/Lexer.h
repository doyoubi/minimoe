#ifndef MINIMOE_LEXER_H
#define MINIMOE_LEXER_H

#include <string>
#include <vector>
#include <memory>

#include "Compiler/CompileErrors.h"

namespace minimoe
{
    enum class CodeTokenType
    {
        IntegerLiteral, // Integer literal
        FloatLiteral,   // Float literal
        StringLiteral,  // String literal
        Identifier, // used for Symbol and Function

        Module,
        Using,
        Phrase,
        Sentence,
        Block,
        Type,

        CPS,
        Category,
        Deferred,
        Argument,
        Assignable,
        List,

        End,

        OpenSquareBracket,  // [
        CloseSquareBracket, // ]
        OpenBracket,  // (
        CloseBracket, // )
        Comma,        // ,
        Colon,        // :
        Add,          // +
        Sub,          // -
        Mul,          // *
        Div,          // /
        Mod,          // %
        LT,           // <
        GT,           // >
        LE,           // <=
        GE,	          // >=
        EQ,           // ==
        NE,           // <>
        Assign,       // =
        GetMember,    // .
        And,          // and
        Or,           // or
        Not,          // not

        UnKnown,
    };

    std::string TokenTypeToString(CodeTokenType type);

    struct CodeToken
    {
        typedef std::shared_ptr<CodeToken> Ptr;
        typedef std::vector<Ptr> List;

        size_t row;
        size_t column;
        std::string value;
        CodeTokenType type;

        CodeToken(size_t _row, size_t _column, std::string _value, CodeTokenType _type)
            : row(_row), column(_column), value(_value), type(_type)
        {}
    };

    struct CompileError
    {
        typedef std::vector<CompileError> List;

        CompileErrorType errorType;
        CodeToken::Ptr token;
        std::string errorMsg;
    };

    struct CodeLine
    {
        typedef std::shared_ptr<CodeLine> Ptr;
        typedef std::vector<Ptr> List;

        CodeToken::List tokens;
    };

    struct CodeFile
    {
        typedef std::shared_ptr<CodeFile> Ptr;
        typedef std::vector<Ptr> List;

        CodeLine::List lines;
        CompileError::List errors;

        static Ptr Parse(const std::string & codeString);
        bool UnEscapeString(const std::string & s, CodeToken::Ptr & token);
    };

}

#endif
