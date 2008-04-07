/*
SpeedSim - an OGame (www.ogame.org) combat simulator
Copyright (C) 2004-2008 Maximialian Matthé & Nicolas Höft

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SKERNEL_DEFS
#define SKERNEL_DEFS

/*!
    \file
    \brief This file contains several defines
*/

////////////////////////////////////////////////////////
// global defines

// macros, to decide which array is meant
#define ATTER 0
#define DEFFER 1

// version of kernel
#define KERNEL_VERSION _T("0.9.8.1b")

//! default defense rebuild factor
#define DEF_AUFBAU_FAKTOR 0.80f
// time the simulator waits for 100k units when aborting a simulation
#define WARTEN_PRO_100K 1000

#if !defined(_DEBUG) && defined(_MSC_VER)
/*! \def ASMRAND
    \brief If defined, own assembler random number generator is used
    \warning Use only, when you use Windows with Visual C++, or else you can't compile 
*/
#define ASMRAND
#endif

/*!
    \def INCL_OPTIMIZED_FUNCTIONS
    \brief If defined, optimized functions will be included
    
    This is not needed at the moment so just let it undefined
*/
//#define INCL_OPTIMIZED_FUNCTIONS

/*!
    \def CREATE_ADV_STATS
    \brief Define this that the more accurate statistics (in png format) will be created

    You can see these stats in the BestWorst-Case File. To be able to use this you will need 
    the freetype library, pnglib and zlib.
*/
//#define CREATE_ADV_STATS
#ifdef CREATE_ADV_STATS
#include "pnggraph.h"
#endif

// names for RF versions
#define NAME_RF_NONE _T("RF_NONE")
#define NAME_RF_058 _T("RF_058")
#define NAME_RF_059 _T("RF_059")
#define NAME_RF_062 _T("RF_062")
#define NAME_RF_065 _T("RF_065")
#define NAME_RF_USER _T("RF_USER")

/*!
    \def RFPERC(x)
    \brief RF(x) converts RF(x) into RF x%
*/
#define RFPERC(x) (x == 0 ? 0 : (100-100.0f/x)*100)

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (((a) < (b)) ? (a) : (b))


// maximum number of fleets per team
#define MAX_PLAYERS_PER_TEAM 16

/*!
    \def UNICODE
    \brief Define this, to compile as Unicode version
*/ 
//#define UNICODE 

// remove old defines
#undef _T
#undef _tcscpy
#undef _tcsncpy
#undef _ttoi
#undef _tcscat
#undef _tcsncat
#undef _tcslen
#undef _tfopen
#undef _fgetts
#undef _istdigit

#ifdef UNICODE
    #define _T(x) L ## x
    typedef wchar_t TCHAR;
    #define _tcscpy wcscpy
    #define _tcsncpy wcsncpy
    #define _ttoi(x) wcstol(x, NULL, 10)
    #define _tcscat wcscat
    #define _tcsncat wcsncat
    #define _tcslen wcslen
    #define _tfopen _wfopen
    #define _fgetts fgetws
    #define _istdigit iswdigit
#else
    #define  _T(x) x
    typedef char TCHAR;
    #define _tcscpy strcpy
    #define _tcsncpy strncpy
    #define _ttoi atoi
    #define _tcscat strcat
    #define _tcsncat strncat
    #define _tcslen strlen
    #define _tfopen fopen
    #define _fgetts fgets
    #define _istdigit isdigit
#endif

#undef _stprintf
int _stprintf(TCHAR * target, const TCHAR * format, ...);

/*!
    \var typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > genstring;
    \brief Type for Unicode and non unicode strings

    Use this type for all strings to be unicode compatible
*/
typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > genstring;
typedef basic_stringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > genstrstream;

#define _STR(x) genstring(_T(x))

// DLL-Export
#ifdef WIN32
    #ifdef SPEEDKERNEL_EXPORTS
    #define SPEEDKERNEL_API __declspec(dllexport)
    #else
    #define SPEEDKERNEL_API __declspec(dllimport)
    #endif
#else
    #define SPEEDKERNEL_API
#endif //WIN32


// Standard windows data types
typedef unsigned int        UINT;
typedef unsigned long       DWORD;
typedef unsigned long       ULONG;
typedef unsigned short int  WORD;
typedef unsigned char       BYTE;

#ifndef _MSC_VER
    typedef long long  __int64;
#endif //Win32

size_t GetNextNumber(genstring str, int& num, size_t pos = 0);

///////////////////////////////////////
// Profiling-Defines

// activate profiling?
//#define PROFILING

#ifdef PROFILING
    #include "ProfileClass.h"
    #define PR_BEG_FUNC(ID) CProfiler::GetInst().BeginFunction((ID))
    #define PR_END_FUNC(ID) CProfiler::GetInst().EndFunction((ID))
    #define PR_ADD_FUNC(ID, name) CProfiler::GetInst().AddFunction((ID), (name))

    #define PR_PROF_FUNC(ID) ProfFunc _PF__JKLM(ID)
    struct ProfFunc
    {
        ProfFunc(DWORD Index) { m_Index = Index; PR_BEG_FUNC(Index); };
        ~ProfFunc() { PR_END_FUNC(m_Index); };
        DWORD m_Index;
    };
    // Function IDs and names
    #define F_ADDPTONUMBER      0
    #define N_ADDPTONUMBER      "AddPointsToNumber"
    #define F_CSA_058           1
    #define N_CSA_058           "CanShootAgain_V058"
    #define F_CSA_059           2
    #define N_CSA_059           "CanShootAgain_V059"
    #define F_CSA_062           3
    #define N_CSA_062           "CanShootAgain_V062"
    #define F_CKBARRAYS         4
    #define N_CKBARRAYS         "ComputeCRArrays"
    #define F_DESTEXPLSHIPS     5
    #define N_DESTEXPLSHIPS     "DestroyExplodedShips"
    #define F_INITSIM           6
    #define N_INITSIM           "InitSim"
    #define F_MASHIELDS         7
    #define N_MASHIELDS         "MaxAllShields"
    #define F_RANDNR            8
    #define N_RANDNR            "RandomNumber"
    #define F_SSTOKB            9
    #define N_SSTOKB            "SaveShipsToCR"
    #define F_SDEXPLODE         10
    #define N_SDEXPLODE         "ShipsDontExplode"
    #define F_SSHOOTS           11
    #define N_SSHOOTS           "ShipShoots"
    #define F_UBWCASE           12
    #define N_UBWCASE           "UpdateBestWorstCase"
#else
    #define PR_BEG_FUNC(ID)
    #define PR_END_FUNC(ID)
    #define PR_ADD_FUNC(ID, name)
    #define PR_PROF_FUNC(ID)
#endif

#endif
