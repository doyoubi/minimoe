#include <string>

#include "UtilsParser.h"
#include "DeclarationParser.h"
#include "Utils/Debug.h"

namespace minimoe
{
    using std::string;

    /***********************
    Parse
    **********************/
    TagDeclaration::Ptr TagDeclaration::Parse(LineIter & head, LineIter tail, CompileError::List & errors)
    {
        string name;
        ParseLineFunc GetName = [&](TokenIter & tokenIt, TokenIter tokenEnd){
            if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Tag, errors))
                return false;
            name = (*tokenIt)->value;
            if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Identifier, errors))
                return false;
            return true;
        };
        auto helper = GenParseLineHelper(head, tail, errors);
        if (!helper(GetName))
            return nullptr;
        auto tag = std::make_shared<TagDeclaration>();
        tag->name = name;
        return tag;
    }

    TypeDeclaration::Ptr TypeDeclaration::Parse(LineIter & head, LineIter tail, CompileError::List & errors)
    {
        auto type = std::make_shared<TypeDeclaration>();

        ParseLineFunc GetName = [&](TokenIter & tokenIt, TokenIter tokenEnd){
            if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Type, errors))
                return false;
            string name = (*tokenIt)->value;
            if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Identifier, errors))
                return false;
            type->name = name;
            return true;
        };

        bool ended = false;
        ParseLineFunc GetMember = [&](TokenIter & tokenIt, TokenIter tokenEnd){
            if ((*tokenIt)->type == CodeTokenType::End)
            {
                ++tokenIt;
                ended = true;
                return true;
            }
            string member = (*tokenIt)->value;
            if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Identifier, errors))
                return false;
            type->members.push_back(member);
            return true;
        };

        auto helper = GenParseLineHelper(head, tail, errors);

        if (!helper(GetName))
            return nullptr;
        while (!ended)
        {
            if (!helper(GetMember))
                return nullptr;
        }
        return type;
    }

    ArgumentDeclaration::Ptr ArgumentDeclaration::Parse(TokenIter & head, TokenIter tail, CompileError::List & errors)
    {
        if (!CheckSingleTokenType(head, tail, CodeTokenType::OpenBracket, errors))
            return nullptr;
        auto arg = std::make_shared<ArgumentDeclaration>();
        auto token = *head;
        if (token->type != CodeTokenType::Identifier)
        {
            arg->type =
                token->type == CodeTokenType::List ? FunctionArgumentType::List :
                token->type == CodeTokenType::BlockBody ? FunctionArgumentType::BlockBody :
                token->type == CodeTokenType::Deferred ? FunctionArgumentType::Deferred :
                token->type == CodeTokenType::Assignable ? FunctionArgumentType::Assignable :
                FunctionArgumentType::UnKnown;
            if (arg->type == FunctionArgumentType::UnKnown)
            {
                errors.push_back({
                    CompileErrorType::Parser_InvalidArgumentDeclaration,
                    token,
                    "argument should be a single identifier or with a qualifier"
                });
                return nullptr;
            }
            ++head;
        }
        else
        {
            arg->type = FunctionArgumentType::Normal;
        }
        arg->name = (*head)->value;
        if (!CheckSingleTokenType(head, tail, CodeTokenType::Identifier, errors))
            return nullptr;
        if (!CheckSingleTokenType(head, tail, CodeTokenType::CloseBracket, errors))
            return nullptr;
        return arg;
    }

    /****************
    ToLog
    ****************/
    std::string TypeDeclaration::ToLog()
    {
        string s = "Type(" + name;
        for (auto & mem : members)
        {
            s += (", " + mem);
        }
        s += ")";
        return s;
    }

    std::string TagDeclaration::ToLog()
    {
        return "Tag(" + name + ")";
    }

    string ArgumentTypeToString(FunctionArgumentType type)
    {
        return
            type == FunctionArgumentType::Assignable ? "Assignable" :
            type == FunctionArgumentType::BlockBody ? "BlockBody" :
            type == FunctionArgumentType::Deferred ? "Deferred" :
            type == FunctionArgumentType::List ? "List" :
            type == FunctionArgumentType::Normal ? "Normal" :
            (ERRORMSG("invali FunctionArgumentType"), "invalid");
    }

    std::string ArgumentDeclaration::ToLog()
    {
        string s = ArgumentTypeToString(type);
        s += "(";
        s += name;
        s += ")";
        return s;
    }

    std::string FunctionDeclaration::ToLog()
    {
        string name, argument;
        for (auto & fragment : fragments)
        {
            if (fragment->type == FunctionFragmentType::Name)
            {
                if (!name.empty()) name += "_";
                name += fragment->name;
            }
            else if (fragment->type == FunctionFragmentType::Argument)
            {
                if (!argument.empty()) argument += ", ";
                argument += fragment->name;
            }
            else ERRORMSG("invalid enum");
        }
        return name + "(" + argument + ")";
    }

    std::string VariableDeclaration::ToLog()
    {
        return "not implemented";
    }

    // FunctionDeclaration
    FunctionDeclaration::Ptr FunctionDeclaration::Make(FunctionType type)
    {
        auto func = std::make_shared<FunctionDeclaration>();
        func->type = type;
        return func;
    }

    FunctionDeclaration::Ptr FunctionDeclaration::name(std::string s)
    {
        auto fragment = std::make_shared<FunctionFragment>();
        fragment->type = FunctionFragmentType::Name;
        fragment->name = s;
        fragments.push_back(fragment);
        return std::dynamic_pointer_cast<FunctionDeclaration>(shared_from_this());
    }

    FunctionDeclaration::Ptr FunctionDeclaration::arg(FunctionArgumentType type, std::string s)
    {
        auto fragment = std::make_shared<FunctionFragment>();
        fragment->type = FunctionFragmentType::Argument;
        fragment->name = s;
        fragments.push_back(fragment);

        auto argument = std::make_shared<ArgumentDeclaration>();
        argument->type = type;
        arguments.push_back(argument);

        return std::dynamic_pointer_cast<FunctionDeclaration>(shared_from_this());
    }
}
