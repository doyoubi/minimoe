#include <sstream>

#include "Compiler/Lexer/Lexer.h"
#include "Utils/Debug.h"

namespace minimoe
{
    using std::string;

    std::string TokenTypeToString(CodeTokenType type)
    {
        return
            type == CodeTokenType::Add ? "+" :
            type == CodeTokenType::And ? "and" :
            type == CodeTokenType::Argument ? "argument" :
            type == CodeTokenType::Assign ? "=" :
            type == CodeTokenType::Assignable ? "assignable" :
            type == CodeTokenType::Block ? "block" :
            type == CodeTokenType::Category ? "category" :
            type == CodeTokenType::CloseBracket ? ")" :
            type == CodeTokenType::CloseSquareBracket ? "]" :
            type == CodeTokenType::Colon ? ":" :
            type == CodeTokenType::Comma ? "," :
            type == CodeTokenType::CPS ? "cps" :
            type == CodeTokenType::Deferred ? "deferred" :
            type == CodeTokenType::Div ? "/" :
            type == CodeTokenType::End ? "end" :
            type == CodeTokenType::EQ ? "==" :
            type == CodeTokenType::FloatLiteral ? "Float" :
            type == CodeTokenType::GE ? ">=" :
            type == CodeTokenType::GetMember ? "." :
            type == CodeTokenType::GT ? ">" :
            type == CodeTokenType::Identifier ? "identifier" :
            type == CodeTokenType::IntegerLiteral ? "Integer" :
            type == CodeTokenType::LE ? "<=" :
            type == CodeTokenType::List ? "List" :
            type == CodeTokenType::LT ? "<" :
            type == CodeTokenType::Mod ? "%" :
            type == CodeTokenType::Module ? "module" :
            type == CodeTokenType::Mul ? "*" :
            type == CodeTokenType::NE ? "<>" :
            type == CodeTokenType::Not ? "not" :
            type == CodeTokenType::OpenBracket ? "(" :
            type == CodeTokenType::OpenSquareBracket ? "[" :
            type == CodeTokenType::Or ? "or" :
            type == CodeTokenType::Phrase ? "phrase" :
            type == CodeTokenType::Sentence ? "sentence" :
            type == CodeTokenType::StringLiteral ? "String" :
            type == CodeTokenType::Sub ? "-" :
            type == CodeTokenType::Tag ? "Tag" :
            type == CodeTokenType::Type ? "Type" :
            type == CodeTokenType::Using ? "using" :
            ERRORMSG("invalid CodeTokenType"), "UnKnown";
    }

    CodeFile::Ptr CodeFile::Parse(const string & codeString)
    {
        auto codeFile = std::make_shared<CodeFile>();

        enum class State
        {
            Begin,
            InInteger,
            InFloat,
            InString,
            InStringEscaping,
            InIdentifier,
            InComment,
            InPreComment,
        };

        size_t row = 1;
        State state = State::Begin;
        auto charIt = std::begin(codeString);
        auto rowBegin = std::begin(codeString);
        const auto headUnusedTag = std::end(codeString);
        auto head = headUnusedTag;

        auto addToken = [&](const string value, CodeTokenType type){
            auto tokenHead = head == headUnusedTag ? charIt : head;
            auto token = std::make_shared<CodeToken>(
                row, distance(rowBegin, tokenHead) + 1, value, type
                );
            if (type == CodeTokenType::Identifier)
            {
                token->type =
                    value == "module" ? CodeTokenType::Module :
                    value == "using" ? CodeTokenType::Using :
                    value == "phrase" ? CodeTokenType::Phrase :
                    value == "sentence" ? CodeTokenType::Sentence :
                    value == "block" ? CodeTokenType::Block :
                    value == "type" ? CodeTokenType::Type :
                    value == "cps" ? CodeTokenType::CPS :
                    value == "category" ? CodeTokenType::Category :
                    value == "deferred" ? CodeTokenType::Deferred :
                    value == "argument" ? CodeTokenType::Argument :
                    value == "assignable" ? CodeTokenType::Assignable :
                    value == "list" ? CodeTokenType::List :
                    value == "end" ? CodeTokenType::End :
                    value == "and" ? CodeTokenType::And :
                    value == "or" ? CodeTokenType::Or :
                    value == "not" ? CodeTokenType::Not :
                    value == "tag" ? CodeTokenType::Tag :
                    CodeTokenType::Identifier;
            }
            else if (type == CodeTokenType::StringLiteral)
            {
                bool success = codeFile->UnEscapeString(value, token);
                if (!success)
                {
                } // treat it as an valid string, but without escape, raise an error and go on.
            }

            if (codeFile->lines.empty() || row > codeFile->lines.back()->tokens.back()->row)
                codeFile->lines.push_back(std::make_shared<CodeLine>());
            codeFile->lines.back()->tokens.push_back(token);
        };

        auto addError = [&](CompileErrorType errorType, const string value, const string errorMsg){
            auto tokenHead = head == headUnusedTag ? charIt : head;
            auto token = std::make_shared<CodeToken>(
                row, distance(rowBegin, tokenHead) + 1, value, CodeTokenType::UnKnown
                );
            CompileError error = {
                errorType, token, errorMsg
            };
            codeFile->errors.push_back(error);
        };

        while (true)
        {
            char c = charIt == std::end(codeString) ? '\0' : *charIt;
            char nextChar = '\0';
            switch (state)
            {
            case State::Begin:
                switch (c)
                {
                case '[':
                    addToken("[", CodeTokenType::OpenSquareBracket);
                    break;
                case ']':
                    addToken("]", CodeTokenType::CloseSquareBracket);
                    break;
                case '(':
                    addToken("(", CodeTokenType::OpenBracket);
                    break;
                case ')':
                    addToken(")", CodeTokenType::CloseBracket);
                    break;
                case ',':
                    addToken(",", CodeTokenType::Comma);
                    break;
                case ':':
                    addToken(":", CodeTokenType::Colon);
                    break;
                case '+':
                    addToken("+", CodeTokenType::Add);
                    break;
                case '-':
                    state = State::InPreComment;
                    break;
                case '*':
                    addToken("*", CodeTokenType::Mul);
                    break;
                case '/':
                    addToken("/", CodeTokenType::Div);
                    break;
                case '%':
                    addToken("%", CodeTokenType::Mod);
                    break;
                case '<':
                    nextChar = std::next(charIt) != codeString.end() ?  *std::next(charIt) : '\0';
                    if (nextChar == '=')
                    {
                        addToken("<=", CodeTokenType::LE);
                        ++charIt;
                    }
                    else if (nextChar == '>')
                    {
                        addToken("<>", CodeTokenType::NE);
                        ++charIt;
                    }
                    else
                        addToken("<", CodeTokenType::LT);
                    break;
                case '>':
                    nextChar = std::next(charIt) != codeString.end() ? *std::next(charIt) : '\0';
                    if (nextChar == '=')
                    {
                        addToken(">=", CodeTokenType::GE);
                        ++charIt;
                    }
                    else
                        addToken(">", CodeTokenType::GT);
                    break;
                case '=':
                    nextChar = std::next(charIt) != codeString.end() ? *std::next(charIt) : '\0';
                    if (nextChar == '=')
                    {
                        addToken("==", CodeTokenType::EQ);
                        ++charIt;
                    }
                    else
                        addToken("=", CodeTokenType::Assign);
                    break;
                case '.':
                    addToken(".", CodeTokenType::GetMember);
                    break;
                case '\n':
                    row++;
                    rowBegin = std::next(charIt);
                    break;
                case '\0':
                case '\t':
                case '\r':
                case ' ':
                    break;
                case '"':
                    state = State::InString;
                    head = charIt;
                    break;
                default:
                    if ('0' <= c && c <= '9')
                    {
                        head = charIt;
                        state = State::InInteger;
                    }
                    else if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_')
                    {
                        head = charIt;
                        state = State::InIdentifier;
                    }
                    else
                    {
                        addError(CompileErrorType::Lexer_UnexpectedChar, string(1, c), "illegal char found: '" + string(1, c) + "'");
                        // ignore this char and go on from State::Begin
                    }
                    break;
                }
                break;
            case State::InPreComment:
                if (c == '-')
                    state = State::InComment;
                else
                {
                    --charIt;
                    addToken("-", CodeTokenType::Sub);
                    head = headUnusedTag;
                    state = State::Begin;
                }
                break;
            case State::InComment:
                if (c == '\n')
                {
                    state = State::Begin;
                    --charIt;
                }
                break;
            case State::InIdentifier:
                if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_')
                {
                    // go on
                }
                else
                {
                    addToken(string(head, charIt), CodeTokenType::Identifier);
                    head = headUnusedTag;
                    state = State::Begin;
                    --charIt;
                }
                break;
            case State::InString:
                if (c == '\n')
                {
                    addError(CompileErrorType::Lexer_InCompleteString, string(head, charIt),
                        "incomplete string, multiple line string is not allowed");
                    head = headUnusedTag;
                    state = State::Begin;
                    --charIt; // let the next loop handle the row change
                }
                else if (c == '\\')
                {
                    state = State::InStringEscaping;
                }
                else if (c == '"')
                {
                    addToken(string(std::next(head), charIt), CodeTokenType::StringLiteral);
                    head = headUnusedTag;
                    state = State::Begin;
                }
                else {} // go on
                break;
            case State::InStringEscaping:
                if (c == '\n')
                {
                    addError(CompileErrorType::Lexer_InCompleteString, string(head, charIt),
                        "incomplete string, multiple line string is not allowed");
                    head = headUnusedTag;
                    state = State::Begin;
                    --charIt; // let the next loop handle the row change
                }
                else state = State::InString; // not handle escape char here, just let it there
                break;
            case State::InInteger:
                if ('0' <= c && c <= '9')
                {
                    // go on
                }
                else if (c == '.')
                {
                    char nextChar = std::next(charIt) == codeString.end() ? '\0' : *std::next(charIt);
                    if ('0' <= nextChar && nextChar <= '9')
                        state = State::InFloat;
                    else
                    {
                        // ignore this '.' and treat this token as Float, but raise error
                        addToken(string(head, charIt), CodeTokenType::FloatLiteral);
                        addError(CompileErrorType::Lexer_InvalidFloat, string(head, std::next(charIt)),
                            "'.' should be followed by digit");
                        state = State::Begin;
                        head = headUnusedTag;
                    }
                }
                else
                {
                    --charIt;
                    // decrease because the current char is not belong to this token
                    // and charIt will increase at the end of loop
                    addToken(string(head, std::next(charIt)), CodeTokenType::IntegerLiteral);
                    state = State::Begin;
                    head = headUnusedTag;
                }
                break;
            case State::InFloat:
                if ('0' <= c && c <= '9')
                {
                    // go on
                }
                else
                {
                    --charIt;
                    // decrease because the current char is not belong to this token
                    // and charIt will increase at the end of loop
                    addToken(string(head, std::next(charIt)), CodeTokenType::FloatLiteral);
                    state = State::Begin;
                    head = headUnusedTag;
                }
            } // end of switch
            if (charIt == std::end(codeString)) break;
            ++charIt;
        }

        return codeFile;
    }

    bool CodeFile::UnEscapeString(const string & s, CodeToken::Ptr & token)
    {
        std::stringstream ss;
        bool escaping = false;
        for (size_t i = 0; i <= s.size(); i++)
        {
            char c = i == s.size() ? '\0' : s[i];
            if (escaping)
            {
                if (c == 'a')
                    ss << '\a';
                else if (c == 'b')
                    ss << '\b';
                else if (c == 'f')
                    ss << '\f';
                else if (c == 'n')
                    ss << '\n';
                else if (c == 'r')
                    ss << '\r';
                else if (c == 't')
                    ss << '\t';
                else if (c == 'v')
                    ss << '\v';
                else if (c == '\\')
                    ss << '\\';
                else if (c == '\'')
                    ss << '\'';
                else if (c == '"')
                    ss << '\"';
                else if (c == '0')
                    ss << '\0';
                else
                {
                    CompileError error = {
                        CompileErrorType::Lexer_InvalidEscapeChar,
                        token,
                        "invalid escape char found in string literal"
                    };
                    errors.push_back(error);
                    return false;
                }
                escaping = false;
            }
            else
            {
                if (c == '\\')
                    escaping = true;
                else if (i != s.size())
                    ss << c;
            }
        }
        token->value = ss.str();
        return true;
    }

}
