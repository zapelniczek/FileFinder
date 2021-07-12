#include <iostream>
#include <chrono>
#include <filesystem>
#include <numeric>
#include "FileSearcher.h"

namespace fs = std::filesystem;
namespace chr = std::chrono;

int main()
{ 
    std::cout << "Hello, there this program will help you in fiding files on your computer.\n\n";

    FileSearcher searcher;

    std::cout << "Before we start please enter the root names on which searching will take place\n"
        "inserting non-letter value will stop collecting root names: ";
    searcher.setRootNames();

    std::cout << "\nPlease enter the file name in order to find it.\n\n";

    std::string fileName;

    std::cout << "Searched file name: ";
    while (std::getline(std::cin, fileName))
    {
        searcher.setFileName(fileName);

        std::cout << '\n';
        chr::time_point start_point = chr::system_clock::now();

        //search
        //result
        if (searcher.getAdviceFlag())
            searcher.showAdvice();
        
        searcher.setFlags();

        if (!searcher.getContinueFlag())
            break;

        std::cout << "Searched file name: ";
    }

    std::cout << "Thanks for using the program.\n";
}
