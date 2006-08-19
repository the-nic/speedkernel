#include "ProfileClass.h"

CProfiler& CProfiler::GetInst()
{
    static CProfiler inst;
    return inst;
}

CProfiler::CProfiler()
{
    m_Functions.clear();
}

CProfiler::~CProfiler(){}

void CProfiler::AddFunction(DWORD Index, const string& name)
{
    Func f(Index, name);
    m_Functions.insert(pair<DWORD, Func>(Index, f));
}

void CProfiler::BeginFunction(DWORD FuncIndex)
{
#ifdef _DEBUG
    FuncIt it = m_Functions.find(FuncIndex);
    if(it == m_Functions.end())
    {
        MessageBoxA(NULL, "Ungültiger Funktionsindex!", "Fehler", 0);
        return;
    }
#else
    FuncIt it = m_Functions.find(FuncIndex);
#endif
    QueryPerformanceCounter(&it->second.tmpTime);
    it->second.NumCalls++;
    m_LastBeganFunction = it;
    m_LastFuncIndex = FuncIndex;    
}

void CProfiler::EndFunction(DWORD FuncNr)
{
    LARGE_INTEGER tmp;
    QueryPerformanceCounter(&(tmp));
    tmp.QuadPart -= m_LastBeganFunction->second.tmpTime.QuadPart;
    m_LastBeganFunction->second.TimeSum.QuadPart += tmp.QuadPart;
}

bool operator<(const Func& fu1, const Func& fu2)
{
    LARGE_INTEGER t1, t2, freq;
    t1.QuadPart = 0;
    t2.QuadPart = 0;
    QueryPerformanceFrequency(&freq);

    return fu1.TimeSum.QuadPart < fu2.TimeSum.QuadPart;
}

void CProfiler::GetResults(ostream& os)
{
    string s;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    
    vector<Func> tmp;
    for(FuncIt it = m_Functions.begin(); it != m_Functions.end(); it++)
        tmp.push_back(it->second);
    sort(tmp.begin(), tmp.end());
    
    os << "Funktionsaufrufe" << endl;
    for(size_t f = 0; f < tmp.size(); f++)
    {
        double t = double(tmp[f].TimeSum.QuadPart) / freq.QuadPart;
        t /= tmp[f].NumCalls;
        os << tmp[f].name << endl;
        os << "Anzahl der Aufrufe: " << tmp[f].NumCalls << endl;
        os << "Durchschnittliche Laufzeit: " << t << "   Summe: " << t * tmp[f].NumCalls << endl << endl;
    }
}
