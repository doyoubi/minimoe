#ifndef MINIMOE_DECLARATION_PARSER_H
#define MINIMOE_DECLARATION_PARSER_H

#include <memory>
#include <vector>

#include "Keyword.h"

namespace minimoe
{
    class Declaration
    {};

    class TypeDeclaration
    {
    public:
        typedef std::shared_ptr<TypeDeclaration> Ptr;

        std::string name;
    };

    class TagDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<TagDeclaration> Ptr;

    };

    enum class FunctionFragmentType
    {
        Name,
        Argument,
    };

    class FunctionFragment
    {
    public:
        typedef std::vector<FunctionFragment> List;

        FunctionFragmentType type;
        std::string name; // both used for funation name and argument name
    };

    enum class FunctionDeclarationType
    {
        Phrase,     // expression, cannot be an statement
        Sentence,   // statement
        Block,      // block statement
    };

    enum class FunctionArgumentType
    {
        Normal,         // a normal function argument
        List,           // a tuple marshalled as array
        Argument,       // for block only, represents an argument to the block body
        Deferred,       // for sentence and block only, represnets a re-evaluable expression
        Assignable,     // for sentence and block only, represnets a assignable expression
    };

    class ArgumentDeclaration : Declaration
    {
    public:
        typedef std::shared_ptr<ArgumentDeclaration> Ptr;
        typedef std::vector<Ptr> List;

        FunctionArgumentType type;
    };

    class FunctionDeclaration : Declaration
    {
    public:
        typedef std::shared_ptr<FunctionDeclaration> Ptr;
        typedef std::vector<Ptr> List;

        FunctionFragment::List fragments;
        FunctionDeclarationType type;
        ArgumentDeclaration::List arguments;
    };

    enum class Type
    {
        UserDefined,

        Array,
        Boolean,
        Integer,
        Float,
        String,
        Function,
        NullType,
        Tag,

        Unknown,
    };

    class VariableDeclaration : Declaration
    {
    public:
        typedef std::shared_ptr<VariableDeclaration> Ptr;

        Type type;
        TypeDeclaration::Ptr userDefinedType;  // used when type == Type::UserDefined

        std::string strValue;
        Keyword builtInValue; // true, false, null
    };

}

#endif
