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
            if (CheckReachTheEnd(tokenIt, tokenEnd, errors))
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
            if (CheckReachTheEnd(tokenIt, tokenEnd, errors))
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

    bool NewDeclaration(CodeTokenType type)
    {
        return type == CodeTokenType::Phrase
            || type == CodeTokenType::Sentence
            || type == CodeTokenType::Block
            || type == CodeTokenType::Tag
            || type == CodeTokenType::Type
            || type == CodeTokenType::Module
            || type == CodeTokenType::Using
            || type == CodeTokenType::CPS
            || type == CodeTokenType::Category;
    }

    FunctionDeclaration::Ptr FunctionDeclaration::Parse(
        LineIter & head, LineIter tail, CompileError::List & errors)
    {
        auto func = std::make_shared<FunctionDeclaration>();

        ParseLineFunc ParseFirstLine = [&](TokenIter & tokenIt, TokenIter tokenEnd){
            auto token = *tokenIt;
            FunctionType type =
                token->type == CodeTokenType::Phrase ? FunctionType::Phrase :
                token->type == CodeTokenType::Sentence ? FunctionType::Sentence :
                token->type == CodeTokenType::Block ? FunctionType::Block :
                FunctionType::UnKnown;
            if (type == FunctionType::UnKnown)
                return false;
            func->type = type;
            ++tokenIt;

            if (CheckReachTheEnd(tokenIt, tokenEnd, errors))
                return false;

            // parse function fragment
            while (true)
            {
                auto & tk = *tokenIt;
                auto fragment = std::make_shared<FunctionFragment>();
                if (tk->type == CodeTokenType::OpenBracket)
                {
                    auto decl = ArgumentDeclaration::Parse(tokenIt, tokenEnd, errors);
                    fragment->name = decl->name;
                    fragment->type = FunctionFragmentType::Argument;
                    func->arguments.push_back(decl);
                }
                else
                {
                    fragment->name = tk->value;
                    if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Identifier, errors))
                        return false;
                    fragment->type = FunctionFragmentType::Name;
                }
                func->fragments.push_back(fragment);
                if (tokenIt == tokenEnd)
                    return true;
                if ((*tokenIt)->type == CodeTokenType::Colon)
                {
                    ++tokenIt;
                    func->alias = (*tokenIt)->value;
                    if (!CheckSingleTokenType(tokenIt, tokenEnd, CodeTokenType::Identifier, errors))
                        return false;
                    return true;
                }
            }
            return true;
        };

        auto helper = GenParseLineHelper(head, tail, errors);
        if (!helper(ParseFirstLine))
            return nullptr;

        auto it = head;
        func->startIter = head;
        while (it != tail
           && (DEBUGCHECK(!(*it)->tokens.empty()), !NewDeclaration((*it)->tokens.front()->type)))
        {
            ++it;
        }
        auto blockEnd = it;

        for (it = head; it != blockEnd; ++it)
        {
            auto tokens = (*it)->tokens;
            DEBUGCHECK(!tokens.empty());
            if (tokens.front()->type == CodeTokenType::End)
            {
                CheckParseToLineEnd(std::next(tokens.begin()), tokens.end(), errors);
                break;
            }
        }
        if (it == blockEnd)
        {
            errors.push_back({
                CompileErrorType::Parser_ExpectEndForFunctionDeclaration,
                (*head)->tokens.front(),
                "function declaration should be end with \"end\""
            });
            return nullptr;
        }
        else func->endIter = it;

        return func;
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

    std::string FunctionTypeToString(FunctionType type)
    {
        return
            type == FunctionType::Phrase ? "Phrase" :
            type == FunctionType::Sentence ? "Sentence" :
            type == FunctionType::Block ? "Block" :
            (ERRORMSG("invalid FunctionType"), "invalid");
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
        string s = FunctionTypeToString(type) + ":" + name + "(" + argument + ")";
        size_t lines = std::distance(startIter, endIter);
        s += "{" + std::to_string(lines) + "}";
        return s;
    }

    std::string VariableDeclaration::ToLog()
    {
        return "not implemented";
    }

    // FunctionDeclaration helper function
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
