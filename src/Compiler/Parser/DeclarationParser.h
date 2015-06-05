#ifndef MINIMOE_DECLARATION_PARSER_H
#define MINIMOE_DECLARATION_PARSER_H

#include <memory>
#include <vector>

#include "Keyword.h"

namespace minimoe
{
    class Declaration : public std::enable_shared_from_this<Declaration>
    {
    public:
        virtual std::string ToLog() = 0;
    };

    class TypeDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<TypeDeclaration> Ptr;

        std::string name;
        std::vector<std::string> members;

        std::string ToLog() override;
    };

    class TagDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<TagDeclaration> Ptr;

        std::string ToLog() override;
    };

    enum class FunctionType
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

    enum class FunctionFragmentType
    {
        Name,
        Argument,
    };

    class FunctionFragment
    {
    public:
        typedef std::shared_ptr<FunctionFragment> Ptr;
        typedef std::vector<Ptr> List;

        FunctionFragmentType type;
        std::string name; // both used for funation name and argument name
    };

    class ArgumentDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<ArgumentDeclaration> Ptr;
        typedef std::vector<Ptr> List;

        FunctionArgumentType type;

        std::string ToLog() override;
    };

    class FunctionDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<FunctionDeclaration> Ptr;
        typedef std::vector<Ptr> List;

        FunctionFragment::List fragments;
        FunctionType type;
        ArgumentDeclaration::List arguments;

        std::string ToLog() override;

        static Ptr Make(FunctionType type);
        FunctionDeclaration::Ptr name(std::string s);
        FunctionDeclaration::Ptr arg(FunctionArgumentType type, std::string s);
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

    class VariableDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<VariableDeclaration> Ptr;

        Type type;
        TypeDeclaration::Ptr userDefinedType;  // used when type == Type::UserDefined

        std::string ToLog() override;

        std::string strValue;
        Keyword builtInValue; // true, false, null
    };

}

#endif
