#include "FileSearcher.h"
#include <iostream>
#include <algorithm>

void FileSearcher::showAdvice()
{
    std::cout << "If you want to search another file press 1.\n"
        << "If you want to stop searching press 0.\n"
        << "If you want to hide helping messages press 5.\n"
        << "If you want to show helping messages press 6.\n"
        << "If you want to change root directories press 9.\n\n";
}

void FileSearcher::setFlags()
{
    char option = '5';
    do
    {
        std::cout << "Enter option: ";
        std::cin >> option;

        if (std::cin.rdstate() & std::ios_base::failbit)
        {
            std::cin.clear();
            continue;
        }

        switch (option)
        {
        case '0':
            cntFlag = false;
            std::cout << "Search of files is stopped.\n\n";
            break;

        case '1':
            cntFlag = true;
            std::cout << "Search of files is continued.\n\n";
            break;

        case '5':
            adviceFlag = false;
            std::cout << "Helping messages are now hidden.\n\n";
            break;

        case '6':
            adviceFlag = true;
            std::cout << "Helping messages are now shown.\n\n";
            break;

        case '9':
            rootNames.clear();

            std::cout << "You are changing root names on which searching will take place (previous root names are discarded)\n"
                "inserting non-letter value will stop collecting root names: ";
            setRootNames();

            std::cout << '\n';
            break;

        default:
            std::cout << "Invalid character\n\n";
        }

    } while (option != '0' && option != '1');

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear stream before reading from getline
}

void FileSearcher::checkRootNames()
{
    auto it = std::unique(rootNames.begin(), rootNames.end());
    rootNames.erase(it, rootNames.end());
}

void FileSearcher::setRootNames()
{
    char rootName;

    while (std::cin >> rootName)
    {
        if (std::isalpha(rootName))
            rootNames.push_back(rootName);
        else
            break;
    }
    checkRootNames();

    if (std::cin.rdstate() & std::ios_base::failbit)
        std::cin.clear();

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}