/*
SpeedSim - a OGame (www.ogame.org) combat simulator
Copyright (C) 2004-2006 Maximialian Matthé & Nicolas Höft

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


#include "CIniFile.h"

CIniFile::CIniFile()
{
    m_DelFirstSpaces = false;
    m_DelLastSpaces = false;
}

CIniFile::~CIniFile()
{
}

CIniFile::CIniFile(const char* file, bool remove_leading_spaces, bool remove_trailing_spaces, bool overwrite_existing /*= true*/)
{
    m_DelFirstSpaces = remove_leading_spaces;
    m_DelLastSpaces = remove_trailing_spaces;
    ReadIniFile(file, false, overwrite_existing);
}

bool CIniFile::ReadIniFile(const char* file, bool append, bool overwrite_existing /*= true*/)
{
#ifdef LINUX
    stat f_stats;
    int s_res = stat(file, &f_stats);
    if(s_res == -1)
        return false;
    if(!S_ISREG(f_stats.st_mode))
        return false;
#endif // LINUX
    bool is_unicode = false;

    // open in binary mode (for unicode)
    FILE* inifile = fopen(file, "rb");
    if(!inifile)
        return false;
    // check if unicode file and if not, switch back to non-unicode mode
    unsigned char uc[2];
    fread(uc, 2, 1, inifile);
    if(uc[0] == 0xFF && uc[1] == 0xFE)
        is_unicode = true;
    else
        fseek(inifile, 0, SEEK_SET);

    if(!append)
        ClearData();

    genstr line;
    genstr cur_section;
    genstr::size_type f, f2;
    while(!feof(inifile))
    {
        line = GetNextLine(inifile, is_unicode);
        f = line.find_first_not_of(_T(" "));
        if(f == genstr::npos)
            continue;
        line.erase(0, f);

        if(line[0] == '[')
        {
            // section
            f = line.find(_T("]"));
            if(f == genstr::npos)
                continue;
            cur_section = line.substr(1, f - 1);
        }
        else if((f = line.find(_T("="))) != genstr::npos)
        {
            // get key and value
            genstr key = RemoveSpaces(line.substr(0, f), m_DelFirstSpaces, m_DelFirstSpaces);
            if((f2 = key.find(_T(";"))) != genstr::npos)
                continue;

            genstr value = line.substr(f + 1);
            // check for comment
            if ((f = value.find(_T(";"))) != genstr::npos) {
                value.erase(f);
            }
            SetStr(value, cur_section, key, overwrite_existing);
        }
    }

    return true;
}

genstr CIniFile::GetNextLine(FILE* file, bool is_unicode)
{
    TCHAR str[512];
    // convert data if needed
    if(!is_unicode) {
        char c[512];
        fgets(c, 512, file);
#ifdef UNICODE
        mbstowcs(str, c, 512);
#else
        strncpy(str, c, 512);
#endif
    }
    else {
        wchar_t c[512];
        fgetws(c, 512, file);
#ifdef UNICODE
        wcsncpy(str, c, 512);
#else
        wcstombs(str, c, 512);
#endif
    }

    if(!str)
    {
        if(ferror(file))
            return _T("");
    }
    // delete CR and LF
    size_t l = _tcslen(str) - 1;
    if(str[0] == '\n' || str[0] == '\r')
        return _T("");

    if(str[l] == '\n' || str[l] == '\r')
        str[l] = '\0';

    if(str[l-1] == '\n' || str[l-1] == '\r')
        str[l-1] = '\0';

    return genstr(str);
}


bool CIniFile::GetStr(genstr& val, genstr strSection, genstr strKey)
{
    strSection = ChgCase(strSection, false);
    strKey = ChgCase(strKey, false);
    IniData::const_iterator it;
    if((it = m_mIniData.find(strSection)) != m_mIniData.end())
    {
        // section available, check for key
        map<genstr, genstr>::const_iterator it2 = it->second.find(strKey);
        if(it2 != it->second.end())
        {
            val = it2->second;
            RemoveSpaces(val, m_DelFirstSpaces, m_DelLastSpaces);
            return true;
        }
    }

    return false;
}

bool CIniFile::GetLong(long& val, genstr strSection, genstr strKey)
{
    genstr str;
    if(GetStr(str, strSection, strKey))
    {
        // find first number in string
        genstr nums(_T("0123456789"));
        genstr::size_type f = str.find_first_of(nums);
        if(f == genstr::npos)
            return false;
        str.erase(0, f);

        TCHAR* stopstr;
        val = _tcstol(str.c_str(), &stopstr, 10);
        return true;
    }
    return false;
}

bool CIniFile::WriteIniFile(const char* file, bool write_unicode)
{
    genstr filedata;
    IniData::const_iterator it = m_mIniData.begin();
    // create string to be written in file
    for(; it != m_mIniData.end(); it++)
    {
        filedata += _T("[");
        filedata += m_mRealSecNames[it->first] + _T("]");
        filedata += _T("\n");
        map<genstr, genstr>::const_iterator it2 = it->second.begin();
        for(; it2 != it->second.end(); it2++)
        {
            filedata += m_mRealKeyNames[it2->first] + _T("=") + it2->second + _T("\n");
        }
        filedata += _T("\n");
    }

    // write into file
    FILE* f = fopen(file, "wb");
    if(!f)
        return false;
    size_t length = filedata.length();

    if(write_unicode)
    {
        // write unicode ident
        unsigned char uc[] = {0xFF, 0xFE};
        fwrite(uc, 2, 1, f);

        wchar_t* str = new wchar_t[length + 1];
#ifdef UNICODE
        wcsncpy(str, filedata.c_str(), length);
#else
        mbstowcs(str, filedata.c_str(), length);
#endif
        str[length] = '\0';
        fwrite(str, sizeof(wchar_t), length, f);
        delete[] str;
    }
    else {
        char* str = new char[length + 1];
#ifdef UNICODE
        wcstombs(str, filedata.c_str(), length);
#else
        strncpy(str, filedata.c_str(), length);
#endif
        str[length] = '\0';
        fwrite(str, sizeof(char), length, f);
        delete[] str;
    }
    fclose(f);
    return true;
}


void CIniFile::SetStr(genstr val, genstr strSection, genstr strKey, bool overwrite_existing /*= true*/)
{
    if(!overwrite_existing)
    {
        genstr tmp;
        if(GetStr(tmp, strSection, strKey))
            return;
    }
    m_mRealSecNames[ChgCase(strSection, false)] = strSection;
    strSection = ChgCase(strSection, false);
    m_mRealKeyNames[ChgCase(strKey, false)] = strKey;
    strKey = ChgCase(strKey, false);
    m_mIniData[strSection][strKey] = val;
}

void CIniFile::SetLong(long val, genstr strSection, genstr strKey, bool overwrite_existing /*= true*/)
{
    char longStr[64];
    sprintf(longStr, "%d", val);

    TCHAR str[64];
    // convert into unicode
#ifdef UNICODE
    mbstowcs(str, longStr, 63);
#else
    strncpy(str, longStr, 63);
#endif
    SetStr(str, strSection, strKey, overwrite_existing);
}

genstr CIniFile::RemoveSpaces(genstr str, bool remove_first, bool remove_last)
{
    genstr::size_type f;
    if(remove_first)
    {
        f = str.find_first_not_of(_T(" "));
        if(f != genstr::npos)
            str.erase(0, f);
    }
    if(remove_last)
    {
        int i = str.length();
        while(str[i - 1] == ' ')
            i--;
        str.erase(i);
    }
    return str;
}

void CIniFile::SetRemoveSpaces(bool remove_leading, bool remove_trailing)
{
    m_DelFirstSpaces = remove_leading;
    m_DelLastSpaces = remove_trailing;
}

genstr CIniFile::ChgCase(genstr str, bool to_upper)
{
    genstr::size_type l = str.length();
    for(genstr::size_type i = 0; i < l; i++)
    {
        if(to_upper)
        {
            str[i] = (str[i] > 'a' && str[i] < 'z') ? str[i] - 0x20 : str[i];
        }
        else
            str[i] = (str[i] > 'A' && str[i] < 'Z') ? str[i] + 0x20 : str[i];
    }
    return str;
}

void CIniFile::ClearData()
{
    m_mIniData.clear();
    m_mRealSecNames.clear();
    m_mRealKeyNames.clear();
}

bool CIniFile::RemoveKey(genstr strSection, genstr strKey)
{
    genstr realKey = ChgCase(strKey, false);
    genstr realSec = ChgCase(strSection, false);
    // check if section and key are available
    if(m_mIniData.find(realSec) != m_mIniData.end())
    {
        map<genstr, genstr>::iterator it = m_mIniData[realSec].find(realKey);
        if(it != m_mIniData[realSec].end())
        {
            m_mIniData[realSec].erase(it);
            return true;
        }
    }
    return false;
}

bool CIniFile::RemoveSection(genstr strSection)
{
    genstr realSec = ChgCase(strSection, false);
    IniData::iterator it = m_mIniData.find(realSec);
    if(it != m_mIniData.end())
    {
        m_mIniData.erase(it);
        return true;
    }
    return false;
}
