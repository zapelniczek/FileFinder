#include <iostream>

#include <filesystem>
#include <numeric>
#include "FileSearcher.h"

int main()
{ 
    std::cout << "Hello, there this program will help you in fiding files on your computer.\n\n";

    FileSearcher searcher;

    std::cout << "Before we start please enter the root names on which searching will take place\n"
        "inserting non-letter value will stop collecting root names: ";
    searcher.setRootNames();

    std::cout << "\nPlease enter the file name in order to find it.\n\n";

    std::wstring fileName;

    std::cout << "Searched file name: ";
    while (std::getline(std::wcin, fileName))
    {
        searcher.setFileName(fileName);

        std::cout << '\n';
        chr::time_point start_point = chr::system_clock::now();
        searcher.searchFile();
        chr::time_point end_point = chr::system_clock::now();
        searcher.displaySearchResults(chr::duration_cast<std::chrono::milliseconds>(end_point - start_point));
        
        if (searcher.getAdviceFlag())
            searcher.showAdvice();
        
        searcher.setFlags();

        if (!searcher.getContinueFlag())
            break;

        std::cout << "Searched file name: ";
    }

    std::cout << "Thanks for using the program.\n";
}
