#ifndef SCANER_H
#define SCANER_H

#include <string>
#include <map>

#include "Types.h"

class TScaner
{
    static std::map<std::string, LexType> m_Table;

public:
    TScaner(const char* text);
    ~TScaner();

    TScaner(const TScaner&) = delete;
    TScaner(TScaner&&) = delete;

    LexType Scan(std::string* lexem);
    LexType PeekNext(std::string* lexem);

    void PrintError(const std::string& message);


    class State
    {
    private:
        size_t m_Pos;
        size_t m_Column;
        size_t m_Line;

        friend class TScaner;
    };



    State SaveState();
    void Restore(const State& state);

private:
    char*  m_Text;
    size_t m_Pos;
    size_t m_Column;
    size_t m_Line;
};

#endif // SCANER_H
