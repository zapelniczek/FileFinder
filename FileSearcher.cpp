#include "FileSearcher.h"
#include <iostream>
#include <algorithm>

void FileSearcher::searchFile()
{
    using namespace std::string_literals;

    for (const auto& i : rootNames)
    {
        try
        {
            fs::path rootPath(char(std::toupper(i)) + ":\\"s);
            if (!fs::exists(rootPath))
                continue;

            for (const auto& i : fs::recursive_directory_iterator(rootPath, fs::directory_options::skip_permission_denied)) //Search exclude sym_links
            {
                std::wstring entryName;

                switch (typeOfSearch)
                {
                case SearchType::Extension:
                {
                    entryName = i.path().extension().wstring();
                    break;
                }
                case SearchType::FullName:
                {
                    entryName = i.path().filename().wstring();
                    break;
                }
                case SearchType::Stem: 
                {
                   entryName = i.path().stem().wstring();
                   break;
                }
                }
                if (!entryName.empty() && entryName == currFile)
                    findResults.insert({ fs::absolute(i.path()), i.status().type() });
            }
        }

        catch (fs::filesystem_error& error)
        {
            std::cerr << "Error occured: " << error.what() << '\n'
                << "at path: " << error.path1() << ".\n\n";
        }
    }
}

void FileSearcher::displaySearchResults(chr::milliseconds time)
{
    std::cout << "Searching for the file took: " << time.count() << "ms.\n";

    if(!findResults.empty())
        std::cout << "The subsequent file(s) has been found: \n";
    else
    {
        std::cout << "File has not been found.\n\n";
        return;
    }

    for (auto& [path, type] : findResults)
    {
        std::wcout << (type == fs::file_type::directory ? "Directory" : "Regular file") << " at path: "
              << path.lexically_normal().wstring() << '\n'; 
        // This may give incorrect results if you are using UNIX systems - more types of files
    }
    std::cout << '\n';
}

void FileSearcher::setFileName(const std::wstring& name)
{
    currFile = name;
    setTypeOfSearch();
}

void FileSearcher::setTypeOfSearch()
{
    if (currFile[0] == '.')
        typeOfSearch = SearchType::Extension;

    else if (currFile.find('.') != std::string::npos)
        typeOfSearch = SearchType::FullName;

    else
        typeOfSearch = SearchType::Stem;
}

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