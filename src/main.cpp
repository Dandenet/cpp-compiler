#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Scaner.h"

int main(int argc, char** argv)
{
    std::ifstream in("res/main.cpp");
    if (in.is_open() == false)
        std::exit(1);
    std::stringstream ss;
    ss << in.rdbuf();
    in.close();



    TScaner scaner(ss.str().c_str());
    LexType type;
    do {
        std::string lex;
        type = scaner.Scan(&lex);

        switch (type) {
        case TypeConst:
        case TypeIdent:
        case TypeExp:
        case TypeInt:
        case TypeShort:
        case TypeLong:
        case TypeDouble:
        case TypeVoid:
        case TypeWhile:
        case TypeMain:
        case TypeClass:
            std::cout << std::setw(5) << type << std::setw(15) << lex << std::endl;
            break;
        case TypeDot:
            std::cout << std::setw(5) << type << std::setw(15) << '.' << std::endl;
            break;
        case TypeComma:
            std::cout << std::setw(5) << type << std::setw(15) << ',' << std::endl;
            break;
        case TypeSemicolon:
            std::cout << std::setw(5) << type << std::setw(15) << ';' << std::endl;
            break;
        case TypeLBracket:
            std::cout << std::setw(5) << type << std::setw(15) << '(' << std::endl;
            break;
        case TypeRBracket:
            std::cout << std::setw(5) << type << std::setw(15) << ')' << std::endl;
            break;
        case TypeLParenthesis:
            std::cout << std::setw(5) << type << std::setw(15) << '{' << std::endl;
            break;
        case TypeRParenthesis:
            std::cout << std::setw(5) << type << std::setw(15) << '}' << std::endl;
            break;

        case TypePlus:
            std::cout << std::setw(5) << type << std::setw(15) << '+' << std::endl;
            break;
        case TypeMinus:
            std::cout << std::setw(5) << type << std::setw(15) << '-' << std::endl;
            break;
        case TypeLT:
            std::cout << std::setw(5) << type << std::setw(15) << '<' << std::endl;
            break;
        case TypeGT:
            std::cout << std::setw(5) << type << std::setw(15) << '>' << std::endl;
            break;
        case TypeLE:
            std::cout << std::setw(5) << type << std::setw(15) << "<=" << std::endl;
            break;
        case TypeGE:
            std::cout << std::setw(5) << type << std::setw(15) << ">=" << std::endl;
            break;
        case TypeEQ:
            std::cout << std::setw(5) << type << std::setw(15) << "==" << std::endl;
            break;
        case TypeNE:
            std::cout << std::setw(5) << type << std::setw(15) << "!=" << std::endl;
            break;
        case TypeOr:
            std::cout << std::setw(5) << type << std::setw(15) << "||" << std::endl;
            break;
        case TypeAnd:
            std::cout << std::setw(5) << type << std::setw(15) << "&&" << std::endl;
            break;
        case TypeMul:
            std::cout << std::setw(5) << type << std::setw(15) << '*' << std::endl;
            break;
        case TypeDiv:
            std::cout << std::setw(5) << type << std::setw(15) << '/' << std::endl;
            break;
        case TypeMod:
            std::cout << std::setw(5) << type << std::setw(15) << '%' << std::endl;
            break;
        case TypeAssign:
            std::cout << std::setw(5) << type << std::setw(15) << '=' << std::endl;
            break;
        default:
            break;
        }
    } while (type != TypeEnd);

    return 0;
}
