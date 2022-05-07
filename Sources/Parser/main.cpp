#include "Riff/Riff.hpp"

#include <iostream>
#include <stdio.h>

int main(int argc, char** argv)
{
    Riff riff;

    if (argc == 2)
    {
        if (auto er = riff.LoadFrom(argv[1]); er != Error::None)
        {
            std::cout << "Error while loading " << argv[1] << " with error " << (u32) er << "\n";
        }
        
        riff.DumpToStdOut();

        __debugbreak();
    }

    return 0;
}