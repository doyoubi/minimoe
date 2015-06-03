#include "ExpressionParser.h"

namespace minimoe
{
    Expression::Ptr SymbolStack::ParseExpression(
        TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        return ParseOr(head, tail, errors);
    }

    Expression::Ptr SymbolStack::ParseBinary(TokenIter & head, TokenIter tail, ParseFunctionType innerParser,
        CodeTokenType tokenTypes[], BinaryOperator binaryOpTypes[], size_t count, CompileError::List & errors)
    {
        auto exp = (this->*innerParser)(head, tail, errors);
        if (exp == nullptr) return nullptr;

        while (true)
        {
            auto index = tokenTypes;
            for (; index < tokenTypes + count; index++)
            {
                if (ParseSingleToken(TokenIter(head), tail, *index))
                    break;
            }
            if (index == tokenTypes + count)
                return exp;

            CompileError::List rhsErrors;
            auto rhs = (this->*innerParser)(++head, tail, rhsErrors);
            if (rhs == nullptr)
                return exp;

            auto binaryExp = std::make_shared<BinaryExpression>();
            binaryExp->binaryOperator =
                *index == CodeTokenType::Add ? BinaryOperator::Add :
                *index == CodeTokenType::Sub ? BinaryOperator::Sub :
                *index == CodeTokenType::Mul ? BinaryOperator::Mul :
                *index == CodeTokenType::Div ? BinaryOperator::Div :
                *index == CodeTokenType::Mod ? BinaryOperator::Mod :
                *index == CodeTokenType::LT ? BinaryOperator::LT :
                *index == CodeTokenType::GT ? BinaryOperator::GT :
                *index == CodeTokenType::LE ? BinaryOperator::LE :
                *index == CodeTokenType::GE ? BinaryOperator::GE :
                *index == CodeTokenType::EQ ? BinaryOperator::EQ :
                *index == CodeTokenType::NE ? BinaryOperator::NE :
                *index == CodeTokenType::And ? BinaryOperator::And :
                *index == CodeTokenType::Or ? BinaryOperator::Or :
                BinaryOperator::UnKnown;
            binaryExp->leftOperand = exp;
            binaryExp->rightOperand = rhs;
            exp = binaryExp;
        }
    }

    Expression::Ptr SymbolStack::ParseOr(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        CodeTokenType tokenTypes[] = { CodeTokenType::Or };
        BinaryOperator binaryOpTypes[] = { BinaryOperator::Or };
        size_t count = sizeof(tokenTypes) / sizeof(CodeTokenType);
        return ParseBinary(head, tail, &SymbolStack::ParseAnd, tokenTypes, binaryOpTypes, count, errors);
    }

    Expression::Ptr SymbolStack::ParseAnd(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        CodeTokenType tokenTypes[] = { CodeTokenType::And };
        BinaryOperator binaryOpTypes[] = { BinaryOperator::And };
        size_t count = sizeof(tokenTypes) / sizeof(CodeTokenType);
        return ParseBinary(head, tail, &SymbolStack::ParsePrimitive, tokenTypes, binaryOpTypes, count, errors);
    }

    Expression::Ptr SymbolStack::ParsePrimitive(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        if (head == tail)
        {
            // here we assume that no empty CodeLine exist, so prev(head) will not crash
            auto token = *std::prev(head);
            errors.push_back({
                CompileErrorType::Parser_NoMoreToken,
                token,
                //*std::prev(tokenIter),  // should not do this! It will crash as a result of compiler's bug
                "expect token but no more token found"
            });
            return nullptr;
        }

        auto token = *head;
        switch (token->type)
        {
        case CodeTokenType::Integer:
        case CodeTokenType::Float:
        case CodeTokenType::String:
            {
                auto literalExp = std::make_shared<LiteralExpression>();
                literalExp->type =
                    token->type == CodeTokenType::Integer ? LiteralType::Integer :
                    token->type == CodeTokenType::Float ? LiteralType::Float :
                    token->type == CodeTokenType::String ? LiteralType::String : 
                    LiteralType::UnKnown;
                literalExp->value = token->value;
                ++head;
                return literalExp;
            }
        case CodeTokenType::Add:
        case CodeTokenType::Sub:
        case CodeTokenType::Not:
            {
                auto exp = ParsePrimitive(++head, tail, errors);
                if (exp == nullptr) return nullptr;
                auto unaryExp = std::make_shared<UnaryExpression>();
                unaryExp->unaryOperator =
                    token->type == CodeTokenType::Add ? UnaryOperator::Positive :
                    token->type == CodeTokenType::Sub ? UnaryOperator::Negative :
                    token->type == CodeTokenType::Not ? UnaryOperator::Not :
                    UnaryOperator::UnKnown;
                unaryExp->operand = exp;
                return unaryExp;
            }
        case CodeTokenType::OpenBracket:
            {
                auto exp = ParsePrimitive(++head, tail, errors);
                if (exp == nullptr) return nullptr;
                if (ParseSingleToken(head, tail, CodeTokenType::CloseBracket))
                    return exp;
                errors.push_back({
                    CompileErrorType::Parser_CloseBracketNotFound,
                    *head,
                    "expect close bracket but not found"
                });
                return nullptr;
            }
        }
    }

    bool SymbolStack::ParseSingleToken(TokenIter & token, TokenIter tail, CodeTokenType type)
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
}

