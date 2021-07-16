#pragma once
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
namespace chr = std::chrono;

/*The target OS for the class in Windows.
Class should work on other OS but the performance may be worse than the target OS.*/
class FileSearcher
{
    enum class SearchType : int {Stem = 1, Extension = 2, FullName = 3};
private:
    std::wstring currFile;

    bool cntFlag = true;
    bool adviceFlag = true;

    SearchType typeOfSearch;
    
    std::vector<char> rootNames;
    std::multimap <fs::path, fs::file_type> findResults; // Container is multimap because on some system you may
                                                          // have file without extension with the same name as folder.
    void checkRootNames();

    void setTypeOfSearch();
    
    std::wstring getEntryName(const fs::path&) const;

    std::wstring getEntryType(fs::file_type) const;

public:

    void searchFile();

    void displaySearchResults(chr::milliseconds time);

    void setFlags();

    void showAdvice();

    void setRootNames();

    void setFileName(const std::wstring& name);

    void setAdviceFlag(bool flag) 
    { 
        adviceFlag = flag;
    }

    bool getAdviceFlag() const
    {
        return adviceFlag;
    }

    void setContinueFlag(bool flag)
    {
        cntFlag = flag;
    }

    bool getContinueFlag() const
    {
        return cntFlag;
    }
};

