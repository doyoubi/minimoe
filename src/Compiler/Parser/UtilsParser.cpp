#include "UtilsParser.h"
#include "Utils/Debug.h"

namespace minimoe
{
    // for tokens
    bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type)
    {
        if (token == tail)
            return false;
        if ((*token)->type == type)
        {
            ++token;
            return true;
        }
        return false;
    }

    bool CheckSingleTokenType(TokenIter & token, TokenIter tail,
        CodeTokenType type, CompileError::List & errors)
    {
        DEBUGCHECK(token != tail);
        if ((*token)->type == type)
        {
            ++token;
            return true;
        }
        string errorMsg = "expect token type " + TokenTypeToString(type) + " but got " + TokenTypeToString((*token)->type);
        errors.push_back({
            CompileErrorType::Parser_UnExpectedTokenType,
            *token,
            errorMsg
        });
        return false;
    }

    bool CheckReachTheEnd(TokenIter head, TokenIter tail, CompileError::List & errors)
    {
        if (head != tail)
            return false;
        // here we assume that no empty CodeLine exist, so prev(head) will not crash
        auto token = *std::prev(head);
        errors.push_back({
            CompileErrorType::Parser_NoMoreToken,
            token,
            //*std::prev(tokenIter),  // should not do this! It will crash as a result of compiler's bug
            "expect token but no more token found"
        });
        return true;
    }

    bool CheckParseToLineEnd(TokenIter head, TokenIter tail, CompileError::List & errors)
    {
        if (head == tail)
            return true;
        errors.push_back({
            CompileErrorType::Parser_CanNotParseLeftToken,
            *head,
            "can't parse the left token in this line"
        });
        return false;
    }

    ParseLineHelper GenParseLineHelper(LineIter & head, LineIter tail, CompileError::List & errors)
    {
        // we have to copy tail because tail will no longer be alive after this function finish
        auto helper = [&head, tail, &errors](ParseLineFunc & parseFunc){
            if (CheckEndOfFile(head, tail, errors))
                return false;
            auto tokenIt = (*head)->tokens.begin();
            auto tokenEnd = (*head)->tokens.end();
            if (CheckReachTheEnd(tokenIt, tokenEnd, errors))
                return false;

            if (!parseFunc(tokenIt, tokenEnd))
                return false;

            // just check and give errors message, go on even if error was raised
            CheckParseToLineEnd(tokenIt, tokenEnd, errors);
            ++head;
            return true;
        };
        return helper;
    }

    // for lines
    bool CheckEndOfFile(LineIter head, LineIter tail, CompileError::List & errors)
    {
        if (head != tail)
            return false;
        errors.push_back({
            CompileErrorType::Parser_NoMoreLine,
            nullptr,
            "no more line found"
        });
        return true;
    }
}
