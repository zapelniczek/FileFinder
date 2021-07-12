#pragma once
#include <string>
#include <vector>

class FileSearcher
{
private:
    std::string currFile;

    bool cntFlag = true;
    bool adviceFlag = true;
    
    std::vector<char> rootNames;

    void checkRootNames();

public:

    void setFlags();

    void showAdvice();

    void setRootNames();

    void setFileName(const std::string& name) 
    {
        currFile = name;
    }

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

