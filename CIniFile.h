/*
SpeedSim - a OGame (www.ogame.org) combat simulator
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


#ifndef CINIFILE_H
#define CINIFILE_H

#pragma warning(disable: 4251 4244 4786 4996 4503)
#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <cctype>

#include "utf8.h"

#undef _tcslen
#undef _tcstol

//#define UNICODE
#ifdef UNICODE
typedef wchar_t TCHAR;
#define _T(x) L ## x
#define _tcslen wcslen
#define _tcstol wcstol
#else
typedef char TCHAR;
#define _T(x) x
#define _tcslen strlen
#define _tcstol strtol
#endif // UNICODE

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif //LINUX

using namespace std;

// wstring for unicode, string for non-unicode
typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > genstr;

// DLL-Export
#ifdef WIN32
#ifdef SPEEDKERNEL_EXPORTS
#define CINIFILE_API __declspec(dllexport)
#else
#define CINIFILE_API __declspec(dllimport)
#endif
#else
#define CINIFILE_API
#endif //WIN32

class CINIFILE_API CIniFile
{
public:
    CIniFile();
    CIniFile(const char* file, bool remove_leading_spaces = false, bool remove_trailing_spaces = true, bool overwrite_existing = true);
    ~CIniFile();

    // reads out all keys from a file
    bool ReadIniFile(const char* file, bool append = false, bool overwrite_existing = true);
    // writes current ini data into file
    bool WriteIniFile(const char* file, bool write_unicode = false);

    bool GetStr(genstr& val, genstr strSection, genstr strKey);
    bool GetLong(long& val, genstr strSection, genstr strKey);
    bool RemoveKey(genstr strSection, genstr strKey);
    bool RemoveSection(genstr strSection);

    void SetStr(genstr val, genstr strSection, genstr strKey, bool overwrite_existing = true);
    void SetLong(long val, genstr strSection, genstr strKey, bool overwrite_existing = true);

    void SetRemoveSpaces(bool remove_leading, bool remove_trailing);

    // resets the ini data
    void ClearData();
private:
    genstr GetNextLine(FILE* file);
    genstr RemoveSpaces(genstr str, bool remove_first, bool remove_last);
    // converts a string into upper or lower case
    genstr ChgCase(genstr str, bool to_upper);
    string WStringToUTF8(wstring str);
    wstring UTF8ToWString(string str);

    // to save strings in correct case (reading=case insensitive, writing=case sensitive)
    // (only for keys and sections)
    map<genstr, genstr> m_mRealSecNames;
    map<genstr, genstr> m_mRealKeyNames;

    typedef map<genstr, map<genstr, genstr> > IniData;
    IniData m_mIniData;

    enum
    {
        F_ANSI,
        F_UTF16LE,
        F_UTF8,
    } m_FileFormat;


    bool m_DelFirstSpaces;
    bool m_DelLastSpaces;
};

#endif
