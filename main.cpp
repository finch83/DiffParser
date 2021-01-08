#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cctype>

constexpr  int NumberOfParamsWithOptions = 5;
constexpr  int NumberOfParamsNoOptions = 4;

void usage(const char* appName, const std::string& sError)
{
    std::cout << sError << std::endl;
    std::cout << "\t" << "Usage: " << appName << " [compare option] sysctlD8File sysctlD10File outputFile\n";
    std::cout << "\t\t" << "Compare option:\n";
    std::cout << "\t\t" << "-v, --value\t\tGet first file value if it is chaged in second file\n";
    std::cout << "\t\t" << "-vv, --values\t\tGet both files value if they are chaged in second file\n";
    std::cout << "\t\t" << "-r, --removed\t\tGet first file value if it is removed in second file\n";
    std::cout << "\t\t" << "-a, --added\t\tGet second file value if it is added in second file\n";
}



void splitString(const std::string& src, std::string& dstKey, std::string& dstValue)
{
//todo reiplement this s...t
    std::size_t found = src.find("=");
    if (found != std::string::npos)
    {
        dstKey = src.substr(0, found);
        if (dstKey[0] == ' ')
        {
            dstKey.erase(dstKey.at(0));
        }

        if (dstKey[found - 1] == ' ')
        {
            dstKey.erase(found - 1);
        }

        dstValue = src.substr(found+1);
        if (dstValue[0] == ' ')
        {
            dstValue.erase(dstValue.begin());
        }

        if ( (dstValue.size() > 0) && (dstValue.at(dstValue.size() - 1) == ' ') )
        {
            dstValue.erase(dstValue.size() - 1);
        }
    }
}

int main(int argc, char** argv)
{
    const std::set<std::string> setParams {"-v", "--value",
                                           "-vv", "--values",
                                           "-r", "--removed",
                                           "-a", "--added"};
    std::vector<std::string> vFileNames{3};


    if ( argc < NumberOfParamsNoOptions)
    {
        usage(argv[0], "Wrong number of afruments");
        return 1;
    }

    std::set<std::string>::iterator sit = setParams.end();
    std::string sRunParam{};
    int nParamNumber{};
// todo analyze params
    for(int i = 1; i < argc; i++)
    {
        sit = setParams.find(argv[i]);
        if ((sit != setParams.end()) &&
            (argc == NumberOfParamsWithOptions))
        {
            sRunParam = *sit;
            nParamNumber = i;
            std::cout << "Param: " << sRunParam << std::endl;
            break;
        }
    }
    for(int i = 1, j = 0; i < argc; i++)
    {
        if (i == nParamNumber) continue;
        else
        {
            vFileNames[j++] = argv[i];
        }
    }

    std::string sSrcFileNameD8(vFileNames[0]);
    std::string sSrcFileNameD10(vFileNames[1]);
    std::string sDstFileNameDiff_D8_D10(vFileNames[2]);

    std::string sLine;
    std::vector<std::string> vLines;

    std::ifstream fSrcD8(vFileNames[0].c_str());
    std::ifstream fSrcD10(vFileNames[1].c_str());
    std::ofstream fDstDiff_D8_D10(vFileNames[2].c_str(), std::ofstream::out | std::ofstream::trunc);

    if (fSrcD8.is_open())
    {
        std::cout << "file: " << sSrcFileNameD8 << " opend" << std::endl;
        while (!fSrcD8.eof())
        {
            std::getline(fSrcD8, sLine);
            vLines.push_back(sLine);
        }
    }
    else
    {
        usage(argv[0], "Error open file: " + sSrcFileNameD8);
    }
    std::map<std::string, std::string> mSrcD8;
    std::string sKey, sVal;
    for(int i=0; i< vLines.size(); ++i)
    {
        splitString(vLines.at(i), sKey, sVal);
        mSrcD8.insert(std::pair<std::string, std::string>(sKey, sVal));
    }

////////////
    vLines.clear();
    if (fSrcD10.is_open())
    {
        std::cout << "file: " << sSrcFileNameD10 << " opend" << std::endl;
        while (!fSrcD10.eof())
        {
            std::getline(fSrcD10, sLine);
            vLines.push_back(sLine);
        }
    }
    else
    {
        std::cout << "Error open file: " << sSrcFileNameD10 << std::endl;
    }
    std::map<std::string, std::string> mSrcD10;
    for(int i=0; i< vLines.size(); ++i)
    {
        splitString(vLines.at(i), sKey, sVal);
        mSrcD10.insert(std::pair<std::string, std::string>(sKey, sVal));
    }


/*
// diff values between 8 and 10
    std::map<std::string, std::string> mDiff_D8_D10;
    for(auto p8 : mSrcD8)
    {
        for(auto p10 : mSrcD10)
        {
            if (p8.first == p10.first)
            {
                if (p8.second != p10.second)
                {
                    mDiff_D8_D10.insert(p8);
                    std::cout << p8.first << " " << p8.second << std::endl;
                }
            }
        }
    }
*/

// both diff values between 8 and 10
    std::map<std::string, std::string> mDiff_D8_D10;
    std::pair<std::string, std::string> pD8D10;
    for(auto p8 : mSrcD8)
    {
        for(auto p10 : mSrcD10)
        {
            if (p8.first == p10.first)
            {
                if (p8.second != p10.second)
                {
                    pD8D10.first = p8.first;
                    pD8D10.second = p8.second + " \\ " + p10.second;
                    mDiff_D8_D10.insert(pD8D10);
//                    std::cout << pD8D10.first << " " << pD8D10.second << std::endl;
                }
            }
        }
    }


/*
// removed params from 10
    std::map<std::string, std::string> mDiff_D8_D10;
    bool isPresent = false;
    for(auto p8 : mSrcD8)
    {
        isPresent = false;
        for(auto p10 : mSrcD10)
        {
            if (p8.first == p10.first)
            {
                isPresent = true;
            }
        }
        if  (!isPresent)
        {
            mDiff_D8_D10.insert(p8);
        }
    }
*/

/*
// added params in 10
    std::map<std::string, std::string> mDiff_D8_D10;
    bool isPresent = false;
    for(auto p10 : mSrcD10)
    {
        isPresent = false;
        for(auto p8 : mSrcD8)
        {
            if (p10.first == p8.first)
            {
                isPresent = true;
            }
        }
        if  (!isPresent)
        {
            mDiff_D8_D10.insert(p10);
        }
    }
*/

    if (fDstDiff_D8_D10.is_open())
    {
        for(auto p : mDiff_D8_D10)
            fDstDiff_D8_D10 << p.first << " = " << p.second << std::endl;
    }
    else
    {
        std::cout << "Error open file: " << sDstFileNameDiff_D8_D10 << std::endl;
    }

    return 0;
}
