#ifdef _WIN32

#ifndef PROFILER_INC
#define PROFILER_INC

// Only compile, if you wish some information about the speed the functions in SpeedSim have

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <tchar.h>
#include <windows.h>

using namespace std;

typedef unsigned long DWORD;

struct CallTime
{
    LARGE_INTEGER Timesum;
};

struct Func
{
    DWORD Index;
    LARGE_INTEGER TimeSum;
    LARGE_INTEGER tmpTime;
    DWORD NumCalls;
    string name;

    Func(DWORD i=0, const string& n = "") {
        Index = i;
        name = n;
        TimeSum.QuadPart = 0;
        NumCalls = 0;
    }
};

class CProfiler
{
public:
    static CProfiler& GetInst();
    ~CProfiler();

    void AddFunction(DWORD Index, const string& name);
    void BeginFunction(DWORD FuncNr);
    void EndFunction(DWORD FuncNr=-1);

    void GetResults(ostream& os);

private:
    CProfiler();
    CProfiler(CProfiler& p);

    typedef map<DWORD, Func>::iterator FuncIt;

    map<DWORD, Func> m_Functions;    
    FuncIt m_LastBeganFunction;
    DWORD m_LastFuncIndex;
};

#endif

#endif