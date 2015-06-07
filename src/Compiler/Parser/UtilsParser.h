#ifndef MINIMOE_UTILS_PARSER_H
#define MINIMOE_UTILS_PARSER_H

#include <functional>

#include "Compiler/Lexer/Lexer.h"

namespace minimoe
{
    // for tokens
    bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type);
    bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type, CompileError::List & errors);
    bool CheckReachTheEnd(TokenIter head, TokenIter tail, CompileError::List & errors);
    bool CheckParseToLineEnd(TokenIter head, TokenIter tail, CompileError::List & errors);

    // for lines
    bool CheckEndOfFile(LineIter head, LineIter tail, CompileError::List & errors);

    typedef std::function<bool(TokenIter & head, TokenIter tail)> ParseLineFunc;
    typedef std::function<bool(ParseLineFunc&)> ParseLineHelper;
    ParseLineHelper GenParseLineHelper(LineIter & head, LineIter tail, CompileError::List & errors);
}

#endif
