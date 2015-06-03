#ifndef MINIMOE_EXPRESSION_PARSER_H
#define MINIMOE_EXPRESSION_PARSER_H

#include <memory>
#include <vector>
#include <string>

#include "Compiler/Lexer/Lexer.h"
#include "DeclarationParser.h"

namespace minimoe
{
    /****************************
    Expressioin
    ****************************/
    class Expression
    {
    public:
        typedef std::shared_ptr<Expression> Ptr;
        typedef std::vector<Ptr> List;

        virtual std::string ToLog() = 0;
    };

    enum class UnaryOperator
    {
        Positive,
        Negative,
        Not,
        UnKnown,
    };

    // operator operand
    class UnaryExpression : public Expression
    {
    public:
        UnaryOperator unaryOperator;
        Expression::Ptr operand;

        virtual std::string ToLog();
    };

    enum class BinaryOperator
    {
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE,
        And,
        Or,
        UnKnown,
    };

    // operand operator operand
    class BinaryExpression : public Expression
    {
    public:
        BinaryOperator binaryOperator;
        Expression::Ptr leftOperand;
        Expression::Ptr rightOperand;

        virtual std::string ToLog();
    };

    enum class LiteralType
    {
        Integer,
        Float,
        String,
        UnKnown,
    };

    // string, float, integer
    class LiteralExpression : public Expression
    {
    public:
        LiteralType type;
        std::string value;

        virtual std::string ToLog();
    };

    class VariableExpression : public Expression
    {
    public:
        VariableDeclaration varDeclaration;

        virtual std::string ToLog();
    };

    // reference a tag
    class TagExpression : public Expression
    {
    public:
        TagDeclaration::Ptr tag;

        virtual std::string ToLog();
    };

    // FunctionNamePart1(ParamExpression1)Part2(ParamExpression2)Part3 ...
    class FunctionInvokeExpression : public Expression
    {
    public:
        FunctionDeclaration::Ptr function;
        Expression::List arguments;

        virtual std::string ToLog();
    };

    // (element1, element2, element3...)
    class ListExpression : public Expression
    {
    public:
        Expression::List elements;

        virtual std::string ToLog();
    };

    /****************************
    SymbolStack
    ****************************/

    enum class SymbolType
    {
        Tag,
        Variable,
        FunctionAlias,
    };

    enum class SymbolValueType
    {};

    class Symbol
    {
    public:
        SymbolType type;
        SymbolValueType valueType;
        std::string name;
    };

    class SymbolStackItem
    {
    private:

        std::vector<FunctionDeclaration> functionTables;
        std::vector<Symbol> symbolTables;
    };

    class SymbolStack
    {
    public:
        typedef CodeToken::List::iterator TokenIter;
        typedef Expression::Ptr(SymbolStack::* ParseFunctionType)(TokenIter&, TokenIter, CompileError::List&);

        std::vector<SymbolStackItem> stackItems;

        Expression::Ptr ParseExpression(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseBinary(TokenIter & head, TokenIter tail, ParseFunctionType innerParser,
            CodeTokenType tokenTypes[], BinaryOperator binaryOpTypes[], size_t count, CompileError::List & errors);
        Expression::Ptr ParseOr(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseAnd(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParsePrimitive(TokenIter & head, TokenIter tail, CompileError::List & errors);
        bool ParseSingleToken(TokenIter & token, TokenIter tail, CodeTokenType type);
    };


}

#endif
