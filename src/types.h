#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>


using Symbol = char;

enum LexType {
    TypeConst = 1,
    TypeIdent,
    TypeExp,

    /// Keywords
    TypeInt = 10,
    TypeShort,
    TypeLong,
    TypeDouble,
    TypeVoid,
    TypeWhile,
    TypeMain,
    TypeClass,

    /// Special symbols
    TypeDot = 20,
    TypeComma,
    TypeSemicolon,
    TypeLBracket,
    TypeRBracket,
    TypeLParenthesis,
    TypeRParenthesis,

    /// Operations
    TypePlus = 30,
    TypeMinus,
    TypeLT,
    TypeGT,
    TypeLE,
    TypeGE,
    TypeEQ,
    TypeNE,
    TypeOr,
    TypeAnd,
    TypeMul,
    TypeDiv,
    TypeMod,
    TypeAssign,

    /// System symbols
    TypeEnd = 50,
    TypeError,

};




#endif // TYPES_H
