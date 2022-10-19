#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Scaner.h"
#include "Parser.h"

int main(int argc, char** argv)
{
    std::ifstream in("res/main.cpp");
    if (in.is_open() == false)
        std::exit(1);
    std::stringstream ss;
    ss << in.rdbuf();
    in.close();



    TScaner scaner(ss.str().c_str());
    TParser parser(&scaner);
    parser.Parse();

    return 0;
}
