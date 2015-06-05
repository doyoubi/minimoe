#ifndef MINIMOE_KEYWORD_H
#define MINIMOE_KEYWORD_H

namespace minimoe
{
    enum class Keyword
    {
        True,  // true
        False, // false
        Null,  // null

        FunctioinResult, // result
        If,   // if
        Else, //else
        Continuation, // continuation, used to invoke continuation
        Var, // var, used to declare variable
        GetItem, // [], used to get element of array
        Size, // size(), used to get length of array
        Type, // type(), used to get type of object
        RedirectTo, // RedirectTo, call native function

        Unknown,
    };
}

#endif
