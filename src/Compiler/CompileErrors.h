#ifndef MINIMOE_COMPILE_ERROR_H
#define MINIMOE_COMPILE_ERROR_H

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
        Parser_UnExpectedTokenType,
        Parser_WrongFunctionName,
        Parser_OneElementListShouldEndWithComma,
        Parser_NotOneElementListShouldNotEndWithComma,

        Parser_NoMoreLine,
    };
}

#endif
