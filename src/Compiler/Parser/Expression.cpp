#include <string>
#include "ExpressionParser.h"
#include "Utils/Debug.h"

namespace minimoe
{
    using std::string;

    string ErrorTag = "$(ErrorTag)";

    string UnaryExpression::ToLog()
    {
        string s;
        s +=
            unaryOperator == UnaryOperator::Negative ? "-" :
            unaryOperator == UnaryOperator::Not ? "not" :
            unaryOperator == UnaryOperator::Positive ? "+" :
            ErrorTag;
        s += "(" + operand->ToLog() + ")";
        return s;
    }

    string BinaryExpression::ToLog()
    {
        string s;
        s +=
            binaryOperator == BinaryOperator::Add ? "+(" :
            binaryOperator == BinaryOperator::Sub ? "-(" :
            binaryOperator == BinaryOperator::Mul ? "*(" :
            binaryOperator == BinaryOperator::Div ? "/(" :
            binaryOperator == BinaryOperator::Mod ? "%(" :
            binaryOperator == BinaryOperator::LT ? "<(" :
            binaryOperator == BinaryOperator::GT ? ">(" :
            binaryOperator == BinaryOperator::LE ? "<=(" :
            binaryOperator == BinaryOperator::GE ? ">=(" :
            binaryOperator == BinaryOperator::EQ ? "==(" :
            binaryOperator == BinaryOperator::NE ? "<>(" :
            binaryOperator == BinaryOperator::And ? "and(" :
            binaryOperator == BinaryOperator::Or ? "or(" :
            ErrorTag;
        s += leftOperand->ToLog() + ", " + rightOperand->ToLog() + ")";
        return s;
    }

    std::string TypeToString(Type type)
    {
        DEBUGCHECK(type != Type::UserDefined);
        return
            type == Type::Array ? "Array" :
            type == Type::Boolean ? "Boolean" :
            type == Type::Float ? "Float" :
            type == Type::Function ? "Function" :
            type == Type::Integer ? "Integer" :
            type == Type::NullType ? "Null" :
            type == Type::String ? "String" :
            type == Type::UserDefined ? "Object" :
            type == Type::Tag ? "Tag" :
            "UnKnown";
    }

    std::string KeywordToString(Keyword keyword)
    {
        return
            keyword == Keyword::Continuation ? "continuation" :
            keyword == Keyword::Else ? "else" :
            keyword == Keyword::False ? "false" :
            keyword == Keyword::FunctioinResult ? "result" :
            keyword == Keyword::GetItem ? "[]" :
            keyword == Keyword::If ? "if" :
            keyword == Keyword::Null ? "null" :
            keyword == Keyword::RedirectTo ? "RedirectTo" :
            keyword == Keyword::Size ? "size" :
            keyword == Keyword::True ? "true" :
            keyword == Keyword::Var ? "var" :
            "UnKnown";
    }

    string LiteralExpression::ToLog()
    {
        return value;
    }

    string SymbolExpression::ToLog()
    {
        string s;
        if (symbol->symbolType == SymbolType::Type)
        {
            if (symbol->builtInType == Type::UserDefined)
                s += symbol->typeDeclaration->name;
            else s += TypeToString(symbol->builtInType);
        }
        else if (symbol->symbolType == SymbolType::Variable)
        {
            s = "(" + symbol->name + ":";
            if (symbol->varDeclaration->type == Type::UserDefined)
                s += symbol->varDeclaration->userDefinedType->name;
            else s += TypeToString(symbol->varDeclaration->type);
            s += ")";
        }
        else if (symbol->symbolType == SymbolType::Keyword)
        {
            s = KeywordToString(symbol->keyword);
        }
        else ERRORMSG("invalid SymbolType");
        return s;
    }

    string FunctionInvokeExpression::ToLog()
    {
        string name, argument;
        for (auto & fragment : function->fragments)
        {
            if (fragment->type == FunctionFragmentType::Name)
            {
                if (!name.empty()) name += "_";
                name += fragment->name;
            }
        }
        for (auto & arg : arguments)
        {
            if (!argument.empty()) argument += ", ";
            argument += arg->ToLog();
        }
        string s = name + "(" + argument + ")";
        return s;
    }

    string ListExpression::ToLog()
    {
        return "not implemented";
    }

}
