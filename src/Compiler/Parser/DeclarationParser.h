#ifndef MINIMOE_DECLARATION_PARSER_H
#define MINIMOE_DECLARATION_PARSER_H

#include <memory>
#include <vector>

namespace minimoe
{
    class Declaration
    {};

    class TagDeclaration : public Declaration
    {
    public:
        typedef std::shared_ptr<TagDeclaration> Ptr;

        TagDeclaration(std::string tagName)
            : name(tagName)
        {}
        const std::string name;
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

        FunctionFragment::List fragments;
        FunctionDeclarationType type;
        ArgumentDeclaration::List arguments;
    };

    enum class VariableType
    {
        Integer,
        Float,
        String,
        FunctionAlias,
    };

    class VariableDeclaration : Declaration
    {
    public:
        typedef std::shared_ptr<VariableDeclaration> Ptr;

        std::string name;
        VariableType type;
    };

}

#endif
