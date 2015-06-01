#include "Compiler/Lexer/Lexer.h"

namespace minimoe
{
    using std::string;

    CodeFile::Ptr CodeFile::Parse(const string & codeString)
    {
        auto codeFile = std::make_shared<CodeFile>();

        enum class State
        {
            Begin,
            InInteger,
            InFloat,
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
            switch (state)
            {
            case State::Begin:
                switch (c)
                {
                case '\n':
                    row++;
                    rowBegin = std::next(charIt);
                    break;
                case '\0':
                case ' ':
                    break;
                case '+':
                    addToken(string(1, c), CodeTokenType::Add);
                    break;
                case '*':
                    addToken(string(1, c), CodeTokenType::Mul);
                    break;
                default:
                    if ('0' <= c && c <= '9')
                    {
                        head = charIt;
                        state = State::InInteger;
                    }
                    else
                    {
                        addError(CompileErrorType::Lexer_UnexpectedChar, string(1, c), "illegal char found: '" + string(1, c) + "'");
                        // ignore this char and go on from State::Begin
                    }
                    break;
                }
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
                        addToken(string(head, charIt), CodeTokenType::Float);
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
                    addToken(string(head, std::next(charIt)), CodeTokenType::Integer);
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
                    addToken(string(head, std::next(charIt)), CodeTokenType::Float);
                    state = State::Begin;
                    head = headUnusedTag;
                }
            } // end of switch
            if (charIt == std::end(codeString)) break;
            ++charIt;
        }

        return codeFile;
    }
}
