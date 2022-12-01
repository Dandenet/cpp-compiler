#include "Scaner.h"
#include "Parser.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
        std::cout << "Has no file path" << std::endl;
    else {

        TScaner scaner(argv[1]);
        TParser parser(&scaner);
        parser.Parse();

    }



    return 0;
}
