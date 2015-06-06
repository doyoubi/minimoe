#ifndef MINIMOE_UTILS_PARSER_H
#define MINIMOE_UTILS_PARSER_H

#include "Compiler/Lexer/Lexer.h"

namespace minimoe
{
    // for tokens
    bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type);
    bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type, CompileError::List & errors);
    bool CheckReachTheEnd(TokenIter head, TokenIter tail, CompileError::List & errors);

    // for lines
    bool CheckEndOfFile(LineIter head, LineIter tail, CompileError::List & errors);
}

#endif
