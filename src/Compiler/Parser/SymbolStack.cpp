#include "ExpressionParser.h"
#include "Utils/Debug.h"

namespace minimoe
{
    /**************************************
    Symbol
    *************************************/
    Symbol::Symbol(Type langBuiltInType, const std::string & symbolName)
        : name(symbolName), symbolType(SymbolType::Type), builtInType(langBuiltInType)
    {}
    Symbol::Symbol(TypeDeclaration::Ptr userDefinedType, const std::string & symbolName)
        : name(symbolName), symbolType(SymbolType::Type), builtInType(Type::UserDefined), typeDeclaration(userDefinedType)
    {}
    Symbol::Symbol(Keyword builtInKeyword, Type langBuiltInType, const std::string & symbolName)
        : name(symbolName), symbolType(SymbolType::Keyword), keyword(builtInKeyword)
    {}
    Symbol::Symbol(VariableDeclaration::Ptr variable, const std::string & symbolName)
        : name(symbolName), symbolType(SymbolType::Variable), varDeclaration(variable)
    {}


    /****************************
    parse function
    *******************************/

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
                // just check, don't change the head iter
                if (CheckSingleTokenType(TokenIter(head), tail, *index))
                    break;
            }
            if (index == tokenTypes + count)
                return exp;
            ++head; // if success, change iter here

            CompileError::List rhsErrors;
            auto rhs = (this->*innerParser)(head, tail, rhsErrors);
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
        if (CheckReachTheEnd(head, tail, errors))
            return nullptr;

        CompileError::List currErrors;
        auto temp = head;
        auto funcExp = ParseInvokeFunction(head, tail, currErrors);
        if (funcExp != nullptr)
            return funcExp;

        head = temp;
        auto listExp = ParseList(head, tail, currErrors);
        if (listExp != nullptr)
            return listExp;

        head = temp;
        auto token = *head;
        switch (token->type)
        {
        case CodeTokenType::IntegerLiteral:
        case CodeTokenType::FloatLiteral:
        case CodeTokenType::StringLiteral:
            {
                auto literalExp = std::make_shared<LiteralExpression>();
                literalExp->type =
                    token->type == CodeTokenType::IntegerLiteral ? LiteralType::Integer :
                    token->type == CodeTokenType::FloatLiteral ? LiteralType::Float :
                    token->type == CodeTokenType::StringLiteral ? LiteralType::String : 
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
                if (CheckSingleTokenType(head, tail, CodeTokenType::CloseBracket))
                    return exp;
                errors.push_back({
                    CompileErrorType::Parser_CloseBracketNotFound,
                    *head,
                    "expect close bracket but not found"
                });
                return nullptr;
            }
        case CodeTokenType::Identifier:
            {
                return ParseSymbol(head, tail, errors);
            }
        }

        for (auto & error : currErrors)
            errors.push_back(error);

        return nullptr;
    }

    bool SymbolStack::CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type)
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

    bool SymbolStack::CheckSingleTokenType(TokenIter & token, TokenIter tail,
        CodeTokenType type, CompileError::List & errors)
    {
        DEBUGCHECK(token != tail);
        if ((*token)->type == type)
        {
            ++token;
            return true;
        }
        errors.push_back({
            CompileErrorType::Parser_UnExpectedTokenType,
            *token,
            "expect token type " + TokenTypeToString(type) + " but got " + TokenTypeToString((*token)->type)
        });
        return false;
    }

    bool SymbolStack::CheckReachTheEnd(TokenIter head, TokenIter tail, CompileError::List & errors)
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

    Expression::Ptr SymbolStack::ParseSymbol(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        // should only called by ParsePrimitive
        DEBUGCHECK(head != tail); // already checked in ParsePrimitive
        auto token = *head;
        if (token->type != CodeTokenType::Identifier)
        {
        }
        auto symbol = ResolveSymbol(token->value);
        if (symbol == nullptr)
        {
            errors.push_back({
                CompileErrorType::Parser_CanNotResolveSymbol,
                *head,
                "can't resolve symbol: " + token->value
            });
            return nullptr;
        }
        ++head;
        auto varExp = std::make_shared<SymbolExpression>();
        varExp->symbol = symbol;
        return varExp;
    }

    Expression::Ptr SymbolStack::ParseInvokeFunction(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        // should only called by ParsePrimitive
        DEBUGCHECK(head != tail); // already checked in ParsePrimitive
        CompileError::List currErrors;
        for (auto itemIter = stackItems.rbegin(); itemIter != stackItems.rend(); ++itemIter)
        {
            auto item = *itemIter;
            for (auto & func : item->functionTables)
            {
                auto funcExp = ParseOneFunction(head, tail, func, currErrors);
                if (funcExp != nullptr)
                    return funcExp;
            }
        }
        for (auto & error : currErrors)
            errors.push_back(error);
        return nullptr;
    }

    Expression::Ptr SymbolStack::ParseOneFunction(TokenIter & head, TokenIter tail,
        FunctionDeclaration::Ptr function, CompileError::List & errors)
    {
        if (CheckReachTheEnd(head, tail, errors))
            return nullptr;
        Expression::List arguments;
        for (auto & fragment : function->fragments)
        {
            if (fragment->type == FunctionFragmentType::Name)
            {
                if (!CheckFunctionNameFragment(head, tail, fragment->name, errors))
                    return nullptr;
            }
            else if (fragment->type == FunctionFragmentType::Argument)
            {
                auto argument = ParseFunctionArgumentFragment(head, tail, errors);
                if (argument == nullptr)
                    return nullptr;
                arguments.push_back(argument);
            }
            else ERRORMSG("invalid FunctionFragmentType");
        }
        auto functionExp = std::make_shared<FunctionInvokeExpression>();
        functionExp->arguments = arguments;
        functionExp->function = function;
        return functionExp;
    }

    Expression::Ptr SymbolStack::ParseFunctionArgumentFragment(
        TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        if (CheckReachTheEnd(head, tail, errors))
            return nullptr;
        if (!CheckSingleTokenType(head, tail, CodeTokenType::OpenBracket, errors))
            return nullptr;
        auto argument = ParseExpression(head, tail, errors);
        if (argument == nullptr)
            return nullptr;
        if (!CheckSingleTokenType(head, tail, CodeTokenType::CloseBracket, errors))
            return nullptr;
        return argument;
    }

    bool SymbolStack::CheckFunctionNameFragment(TokenIter & head, TokenIter tail,
        const std::string & name, CompileError::List & errors)
    {
        if (CheckReachTheEnd(head, tail, errors))
            return nullptr;
        if (!CheckSingleTokenType(TokenIter(head)/* just check */, tail, CodeTokenType::Identifier, errors))
            return nullptr;
        auto token = *head;
        if (token->value == name)
        {
            ++head;
            return true;
        }
        errors.push_back({
            CompileErrorType::Parser_WrongFunctionName,
            token,
            "expect function name fragment \"" + name + "\" but get\"" + token->value + "\""
        });
        return false;
    }

    Expression::Ptr SymbolStack::ParseList(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        if (CheckReachTheEnd(head, tail, errors))
            return nullptr;
        if (!CheckSingleTokenType(head, tail, CodeTokenType::OpenBracket, errors))
            return nullptr;
        Expression::List elements;
        CompileError::List currErrors;
        // (), (1,), (1,2) are valid ListExpression, while (1), (1,2,) are not
        while (true)
        {
            auto temp = head;
            auto exp = ParseExpression(head, tail, currErrors);
            if (exp == nullptr)
            {
                head = temp;
                if (elements.size() < 2) break;
                errors.push_back({
                    CompileErrorType::Parser_NotOneElementListShouldNotEndWithComma,
                    *head,
                    "List with more than one element should not end with comma"
                });
                return nullptr;
            }
            elements.push_back(exp);
            if (!CheckSingleTokenType(TokenIter(head), tail, CodeTokenType::Comma, currErrors))
            {
                if (elements.size() != 1) break;
                errors.push_back({
                    CompileErrorType::Parser_OneElementListShouldEndWithComma,
                    *head,
                    "In one element List, there should be a comma after the element"
                });
                return nullptr;
            }
            ++head;
        }
        if (!CheckSingleTokenType(head, tail, CodeTokenType::CloseBracket, errors))
            return nullptr;
        auto list = std::make_shared<ListExpression>();
        list->elements = elements;
        return list;
    }

    /*********************
    SymbolStackItem
    ********************/
    void SymbolStackItem::LoadPredefinedSymbol()
    {
        // built in types
        addSymbol(Type::Array, "Array");
        addSymbol(Type::Boolean, "Boolean");
        addSymbol(Type::Float, "Float");
        addSymbol(Type::Function, "Function");
        addSymbol(Type::Integer, "Integer");
        addSymbol(Type::NullType, "Null");
        addSymbol(Type::String, "String");
        addSymbol(Type::Tag, "Tag");

        // built in values
        addSymbol(Keyword::Null, Type::NullType, "null");
        addSymbol(Keyword::True, Type::Boolean, "true");
        addSymbol(Keyword::False, Type::Boolean, "false");
    }

    /******************
    SymbolStack operation
    *****************/
    void SymbolStack::Push(SymbolStackItem::Ptr item)
    {
        stackItems.push_back(item);
    }

    void SymbolStack::Pop()
    {
        stackItems.pop_back();
    }

    SymbolStackItem::Ptr SymbolStack::Top()
    {
        return stackItems.back();
    }

    Symbol::Ptr SymbolStack::ResolveSymbol(std::string name)
    {
        for (auto it = stackItems.rbegin(); it != stackItems.rend(); ++it)
        {
            for (auto & symbol : (*it)->symbolTables)
                if (symbol->name == name)
                    return symbol;
        }
        return nullptr;
    }

}

