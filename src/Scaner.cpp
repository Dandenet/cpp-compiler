#include "Scaner.h"

#include <string.h>
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>

inline bool IsDigit(char symbol)
{
    return symbol >= '0' && symbol <= '9';
}

inline bool IsLetter(char symbol)
{
    return (symbol >= 'A' && symbol <= 'Z')  ||
            (symbol >= 'a' && symbol <= 'z');
}

std::map<std::string, LexType> TScaner::m_Table = {
    {"int", TypeInt},
    {"short", TypeShort},
    {"long", TypeLong},
    {"double", TypeDouble},
    {"void", TypeVoid},
    {"while", TypeWhile},
    {"main", TypeMain},
    {"class", TypeClass},
};

TScaner::TScaner(const std::string& name) :
    m_Pos(0), m_Column(0), m_Line(1), m_FileName(name)
{
    std::ifstream in(m_FileName);
    if (in.is_open() == false)
        std::exit(1);
    std::stringstream ss;
    ss << in.rdbuf();
    in.close();

    auto text = ss.str();

    size_t bytes = text.size() + 1;
    m_Text = new char [bytes];
    memcpy(m_Text, text.c_str(), bytes);
}


LexType TScaner::Scan(std::string *lexem)
{
    while (true)
    {

        // skip non-significant symbolы
        while (m_Text[m_Pos] == ' '  ||
               m_Text[m_Pos] == '\n' ||
               m_Text[m_Pos] == '\t') {
            if (m_Text[m_Pos] == '\n') {
                m_Column = 0; ++m_Line;
            }
            ++m_Pos; ++m_Column;
        }

        // skip single line comment
        if (m_Text[m_Pos] == '/' && m_Text[m_Pos + 1] == '/') {
            m_Pos += 2; m_Column += 2;
            while (m_Text[m_Pos] != '\n' && m_Text[m_Pos] != '\0')
            {
                ++m_Pos; ++m_Column;
            }
            continue;
        }

        // skip multy line comment
        if (m_Text[m_Pos] == '/' && m_Text[m_Pos + 1] == '*') {

            m_Pos += 2; m_Column += 2;
            while (m_Text[m_Pos] != '\0' &&
                   !(m_Text[m_Pos] == '*' && m_Text[m_Pos + 1] == '/'))
            {
                if (m_Text[m_Pos] == '\n') {
                    m_Column = 0; ++m_Line;
                }

                ++m_Pos; ++m_Column;
            }

            if (m_Text[m_Pos] != '\0') {
                m_Pos+= 2; m_Column += 2;
            }
            continue;
        }

        // identifier
        if (IsLetter(m_Text[m_Pos]) || m_Text[m_Pos] == '_') {
            std::string buffer;

            do {
                buffer.push_back(m_Text[m_Pos]);
                ++m_Pos;
                ++m_Column;
            } while (IsLetter(m_Text[m_Pos]) ||
                     IsDigit(m_Text[m_Pos])  ||
                     m_Text[m_Pos] == '_');

            // keyword checking
            LexType type = TypeIdent;
            auto it = m_Table.find(buffer);
            if (it != m_Table.end())
                type = it->second;

            if (lexem)
                *lexem = std::move(buffer);

            return type;
        }


        // constants
        if (IsDigit(m_Text[m_Pos]) || m_Text[m_Pos] == '.') {
            bool isContainDigit = false; // is contain digit before a dot
            std::string buffer;

            if (m_Text[m_Pos] == '0' && IsDigit(m_Text[m_Pos + 1])) {
                PrintError(std::format("number can't start from {}", m_Text[m_Pos]));
                ++m_Pos; ++m_Column;
                return TypeError;
            }

            // integer part of the number
            while (IsDigit(m_Text[m_Pos]))
            {
                isContainDigit = true;
                buffer.push_back(m_Text[m_Pos]);
                ++m_Pos; ++m_Column;
            }

            // integer constant
            if (isContainDigit && m_Text[m_Pos] != 'e' && m_Text[m_Pos] != '.') {
                if (lexem)
                    *lexem = std::move(buffer);

                return TypeConst;
            }


            // if the simple dot
            if (!isContainDigit && m_Text[m_Pos] == '.' && !IsDigit(m_Text[m_Pos] + 1))
            {
                ++m_Pos; ++m_Column;
                return TypeDot;
            }

            // the real number
            if (m_Text[m_Pos] == '.') {
                buffer.push_back(m_Text[m_Pos]);
                ++m_Pos; ++m_Column;
            }

            while (IsDigit(m_Text[m_Pos])) {
                buffer.push_back(m_Text[m_Pos]);
                ++m_Pos; ++m_Column;
            }

            if (m_Text[m_Pos] != 'e') {
                PrintError("the real number should have 'e'");
                return TypeError;
            }

            buffer.push_back(m_Text[m_Pos]);
            ++m_Column;
            ++m_Pos;

            // exponent sign
            if (m_Text[m_Pos] == '+' || m_Text[m_Pos] == '-') {
                buffer.push_back(m_Text[m_Pos]);
                ++m_Column;
                ++m_Pos;
            }

            // exponent digits
            if (IsDigit(m_Text[m_Pos])) {
                do {
                    buffer.push_back(m_Text[m_Pos]);
                    ++m_Column;
                    ++m_Pos;

                } while (IsDigit(m_Text[m_Pos]));
            } else {
                PrintError("exponent has no digits");
                return TypeError;
            }

            if (lexem)
                *lexem = std::move(buffer);

            return TypeExp;
        }


        // or operator
        if (m_Text[m_Pos] == '|' && m_Text[m_Pos + 1] == '|') {
            m_Pos += 2;
            m_Column += 2;
            return TypeOr;

        } else if (m_Text[m_Pos] == '|') {
            PrintError(std::format("syntax error '{}'", m_Text[m_Pos]));
            m_Pos += 1;
            m_Column += 1;
            return TypeError;
        }

        // and operator
        if (m_Text[m_Pos] == '&' && m_Text[m_Pos + 1] == '&') {
            m_Pos += 2;
            m_Column += 2;
            return TypeAnd;

        } else if (m_Text[m_Pos] == '&') {
            PrintError(std::format("syntax error '{}'", m_Text[m_Pos]));
            m_Pos += 1;
            m_Column += 1;
            return TypeError;
        }

        // not equal operator
        if (m_Text[m_Pos] == '!' && m_Text[m_Pos + 1] == '=') {
            m_Pos += 2;
            m_Column += 2;
            return TypeNE;
        } else if (m_Text[m_Pos] == '!') {
            PrintError(std::format("syntax error '{}'", m_Text[m_Pos]));
            m_Pos += 1;
            m_Column += 1;
            return TypeError;
        }

        // parse less or equal
        if (m_Text[m_Pos] == '<' && m_Text[m_Pos + 1] == '=') {
            m_Pos += 2;
            m_Column += 2;
            return TypeLE;
        }


        // more or equal
        if (m_Text[m_Pos] == '>' && m_Text[m_Pos + 1] == '=') {
            m_Pos += 2;
            m_Column += 2;
            return TypeGE;
        }

        // equal
        if (m_Text[m_Pos] == '=' && m_Text[m_Pos + 1] == '=') {
            m_Pos += 2;
            m_Column += 2;
            return TypeEQ;
        }


        ++m_Column;
        switch (m_Text[m_Pos++]) {
        case '\0':
            return TypeEnd;
        case ',':
            return TypeComma;
        case ';':
            return TypeSemicolon;
        case '(':
            return TypeLBracket;
        case ')':
            return TypeRBracket;
        case '{':
            return TypeLParenthesis;
        case '}':
            return TypeRParenthesis;
        case '+':
            return TypePlus;
        case '-':
            return TypeMinus;
        case '*':
            return TypeMul;
        case '/':
            return TypeDiv;
        case '%':
            return TypeMod;
        case '<':
            return TypeLT;
        case '>':
            return TypeGT;
        case '=':
            return TypeAssign;

        default:
            PrintError(std::format("Unknown character '0x{:x}'", m_Text[m_Pos - 1]));
            return TypeError;
            break;
        }
    }
}

void TScaner::PrintError(const std::string &message)
{
    std::cout << std::format("{}({}) error: {}", m_FileName, m_Line, message) << std::endl;
}

TScaner::State TScaner::SaveState()
{
    State state;

    state.m_Pos = m_Pos;
    state.m_Column = m_Column;
    state.m_Line = m_Line;
    return state;
}

void TScaner::Restore(const State& state)
{
    m_Pos = state.m_Pos;
    m_Column = state.m_Column;
    m_Line = state.m_Line;
}


LexType TScaner::PeekNext(std::string* lexem)
{
    size_t pos = m_Pos;
    size_t column = m_Column;
    size_t line = m_Line;

    LexType type = Scan(lexem);

    m_Pos = pos;
    m_Column = column;
    m_Line = line;

    return type;
}

TScaner::~TScaner()
{
    delete [] m_Text;
}
