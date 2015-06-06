#ifndef MINIMOE_EXPRESSION_PARSER_H
#define MINIMOE_EXPRESSION_PARSER_H

#include <memory>
#include <vector>
#include <string>

#include "Compiler/Lexer/Lexer.h"
#include "DeclarationParser.h"
#include "Keyword.h"

namespace minimoe
{
    /***********************
    Symbol
    *********************/
    enum class SymbolType
    {
        Type,
        Keyword,
        Variable,

        Unknown,
    };

    class Symbol
    {
    public:
        typedef std::shared_ptr<Symbol> Ptr;
        typedef std::vector<Ptr> List;

        SymbolType symbolType;
        // symbol value
        Type builtInType = Type::Unknown;     // only used when SymbolType == SymbolType::Type
        TypeDeclaration::Ptr typeDeclaration; // only used when builtInType == Type::UserDefined
        Keyword keyword = Keyword::Unknown;   // for true, false, null, only used when SymbolType == SymbolType::Keyword
        VariableDeclaration::Ptr varDeclaration; // only used when SymbolType == SymbolType::Variable

        std::string name;

        Symbol(Type langBuiltInType, const std::string & symbolName);
        Symbol(TypeDeclaration::Ptr userDefinedType, const std::string & symbolName);
        Symbol(Keyword builtInKeyword, Type langBuiltInType, const std::string & symbolName);
        Symbol(VariableDeclaration::Ptr variable, const std::string & symbolName);
    };


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

    // reference for types, built in values, variables
    class SymbolExpression : public Expression
    {
    public:
        Symbol::Ptr symbol;  // we can find declaration here

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

    class SymbolStackItem
    {
    public:
        typedef std::shared_ptr<SymbolStackItem> Ptr;
        typedef std::vector<Ptr> List;

        FunctionDeclaration::List functionTables;
        Symbol::List symbolTables;

        void LoadPredefinedSymbol();

        template<class... Params>
        void addSymbol(Params &&... params)
        {
            auto symbol = std::make_shared<Symbol>(std::forward<Params>(params)...);
            symbolTables.push_back(symbol);
        }
    };

    class SymbolStack
    {
    public:
        typedef CodeToken::List::iterator TokenIter;
        typedef Expression::Ptr(SymbolStack::* ParseFunctionType)(TokenIter&, TokenIter, CompileError::List&);

        SymbolStackItem::List stackItems;
        
        void Push(SymbolStackItem::Ptr item);
        void Pop();
        SymbolStackItem::Ptr Top();
        Symbol::Ptr ResolveSymbol(std::string name);

        Expression::Ptr ParseExpression(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseBinary(TokenIter & head, TokenIter tail, ParseFunctionType innerParser,
            CodeTokenType tokenTypes[], BinaryOperator binaryOpTypes[], size_t count, CompileError::List & errors);
        Expression::Ptr ParseOr(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseAnd(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParsePrimitive(TokenIter & head, TokenIter tail, CompileError::List & errors);
        bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type);
        bool CheckSingleTokenType(TokenIter & token, TokenIter tail, CodeTokenType type, CompileError::List & errors);
        bool CheckReachTheEnd(TokenIter head, TokenIter tail, CompileError::List & errors);

        // include types, built in values, variables
        Expression::Ptr ParseSymbol(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseInvokeFunction(TokenIter & head, TokenIter tail, CompileError::List & errors);
        Expression::Ptr ParseOneFunction(TokenIter & head, TokenIter tail,
            FunctionDeclaration::Ptr function, CompileError::List & errors);
        Expression::Ptr ParseFunctionArgumentFragment(TokenIter & head, TokenIter tail, CompileError::List & errors);
        bool CheckFunctionNameFragment(TokenIter & head, TokenIter tail,
            const std::string & name, CompileError::List & errors);

        Expression::Ptr ParseList(TokenIter & head, TokenIter tail, CompileError::List & errors);
    };


}

#endif
