#pragma once
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <filesystem>
#include <future>
#include <mutex>
#include <atomic>

namespace fs = std::filesystem;
namespace chr = std::chrono;

/*The target OS for the class in Windows.
Class should work on other OS but the performance may be worse than the target OS.*/
class FileSearcher
{
    enum class SearchType : int {Stem = 1, Extension = 2, FullName = 3, NotSet = 0};
private:
    std::wstring currFile;

    bool cntFlag = true;
    bool adviceFlag = true;

    SearchType typeOfSearch;

    std::atomic<int> m_threads = 1;
    inline static std::atomic<int> max_threads; // You need to test the optimal number for yourself to get the best results

    std::thread::id m_thredID;
    
    std::vector<char> rootNames;
    std::vector<char> pendingRoots; 

    std::multimap <fs::path, fs::file_type> findResults; // Container is multimap because on some system you may
                                                          // have file without extension with the same name as folder.
    std::vector<std::future<void>> futures; 

    mutable std::mutex m_mutex;
   
private:

    void checkRootNames();

    void setTypeOfSearch();

    void searchOnDisk(fs::path);
  
    std::wstring getEntryName(const fs::path&) const;

    std::wstring getEntryType(fs::file_type) const;

    void distribute_work();

    void retrieveFutures();

public:

    FileSearcher();

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

