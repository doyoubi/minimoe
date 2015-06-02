#ifndef MINIMOE_LEXER_H
#define MINIMOE_LEXER_H

#include <string>
#include <vector>

#include "Compiler/CompileErrors.h"

namespace minimoe
{
    enum class CodeTokenType
    {
        Integer,
        Float,
        String,
        Identifier,

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
        EQ,           // =
        NE,           // <>
        And,          // and
        Or,           // or
        Not,          // not

        Comment,
        UnKnown,
    };

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
