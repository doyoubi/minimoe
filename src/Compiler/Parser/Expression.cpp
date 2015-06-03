#include <string>
#include "ExpressionParser.h"

namespace minimoe
{
    using std::string;

    string UnaryExpression::ToLog()
    {
        return "not implemented";
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
            "(unknown operator)";
        s += leftOperand->ToLog() + ", " + rightOperand->ToLog() + ")";
        return s;
    }

    string LiteralExpression::ToLog()
    {
        return value;
    }

    string VariableExpression::ToLog()
    {
        return "not implemented";
    }

    string TagExpression::ToLog()
    {
        return "not implemented";
    }

    string FunctionInvokeExpression::ToLog()
    {
        return "not implemented";
    }

    string ListExpression::ToLog()
    {
        return "not implemented";
    }

}
