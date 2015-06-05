#ifndef MINIMOE_COMPILE_ERROR_H
#define MINIMOE_COMPILE_ERROR_H

#include <vector>
#include <string>
#include <memory>

namespace minimoe
{
    enum class CompileErrorType
    {
        Lexer_UnexpectedChar,
        Lexer_InvalidFloat,
        Lexer_InCompleteString,
        Lexer_InvalidEscapeChar,

        Parser_NoMoreToken,
        Parser_CloseBracketNotFound,
        Parser_CanNotResolveSymbol,
    };

}

#endif
