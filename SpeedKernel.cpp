/*
SpeedSim - a OGame (www.ogame.org) combat simulator
Copyright (C) 2004-2007 Maximialian Matthé & Nicolas Höft

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

#pragma warning(disable: 4251)
#include "SpeedKernel.h"


CSpeedKernel& CSpeedKernel::GetInstance()
{
    static CSpeedKernel inst;
    return inst;
}

CSpeedKernel& CSpeedKernel::ForceNewInstance()
{
    CSpeedKernel* sk = new CSpeedKernel;
    return *sk;
}

CSpeedKernel::CSpeedKernel()
{
    int r;
	// set all variables to default values
	m_DefInTF = false;
	m_Result.PlaniName[0] = '\0';
	memset(m_TechsAtt, 0, MAX_PLAYERS_PER_TEAM * 3 * sizeof(int));
	memset(m_TechsDef, 0, MAX_PLAYERS_PER_TEAM * 3 * sizeof(int));
	memset(&m_Result, 0, sizeof(BattleResult));
	m_Result.RessDa = Res();
	m_FuncPtr = NULL;
	m_CompBestWorstCase = true;
	m_Result.GesamtBeute = Res();
	m_Result.NumAtts = 0;
    m_Result.GesamtRecs = 0;
    for(r = 0; r < MAX_PLAYERS_PER_TEAM; r++)
        m_Speed[r] = 10;

	m_NumShipsAtt.resize(T_END);
	m_NumShipsDef.resize(T_END);

	for(r = 0; r < 7; r++)
	{
		m_NumShipsInKBAtt[r].resize(T_END);
		m_NumShipsInKBDef[r].resize(T_END);
	}
    m_NumSetShipsAtt.clear();
    m_NumSetShipsDef.clear();
	srand((unsigned)time(NULL));
    InitRand();

	m_AttObj = new vector<Obj>;
	m_DefObj = new vector<Obj>;

    memset(m_OwnPos, 0, MAX_PLAYERS_PER_TEAM * sizeof(PlaniPos));

	m_DataIsDeleted = false;
	m_SimulateFreedItsData = true;
    LoadLangFile(NULL);
	// set to newest version
    CanShootAgain = CanShootAgain_V065;
    //m_NewShield = true;
    m_NewFuel = true;
    m_DefRebuildFac = DEF_AUFBAU_FAKTOR;
    m_LastScanHadTechs = false;
    m_ShipDataFromFile = false;
    m_RebuildSmallDef = true;

    m_NumPlayersPerTeam[ATTER] = 0;
    m_NumPlayersPerTeam[DEFFER] = 0;
    m_BracketNames = false;
    m_CurrentSim = 0;

    for(r = 0; r < T_SHIPEND; r++) {
        for(int i = 0; i < T_END; i++) {
            m_RF[r][i] = 0;
        }
    }

    ComputeShipData();

    // profiler functions
    PR_ADD_FUNC(F_ADDPTONUMBER, N_ADDPTONUMBER);
    PR_ADD_FUNC(F_CSA_058, N_CSA_058);
    PR_ADD_FUNC(F_CSA_059, N_CSA_059);
    PR_ADD_FUNC(F_CSA_062, N_CSA_062);
    PR_ADD_FUNC(F_CKBARRAYS, N_CKBARRAYS);
    PR_ADD_FUNC(F_DESTEXPLSHIPS, N_DESTEXPLSHIPS);
    PR_ADD_FUNC(F_INITSIM, N_INITSIM);
    PR_ADD_FUNC(F_MASHIELDS, N_MASHIELDS);
    PR_ADD_FUNC(F_RANDNR, N_RANDNR);
    PR_ADD_FUNC(F_SSTOKB, N_SSTOKB);
    PR_ADD_FUNC(F_SDEXPLODE, N_SDEXPLODE);
    PR_ADD_FUNC(F_SSHOOTS, N_SSHOOTS);
    PR_ADD_FUNC(F_UBWCASE, N_UBWCASE);

    // ini-names of ships
    m_IniFleetNames[T_KT] = _T("S_CAR");
    m_IniFleetNames[T_GT] = _T("L_CAR");
    m_IniFleetNames[T_LJ] = _T("L_FI");
    m_IniFleetNames[T_SJ] = _T("H_FI");
    m_IniFleetNames[T_KREUZER] = _T("CRUI");
    m_IniFleetNames[T_SS] = _T("BS");
    m_IniFleetNames[T_KOLO] = _T("COL");
    m_IniFleetNames[T_REC] = _T("REC");
    m_IniFleetNames[T_SPIO] = _T("ESP");
    m_IniFleetNames[T_BOMBER] = _T("BOM");
    m_IniFleetNames[T_SAT] = _T("SOL");
    m_IniFleetNames[T_ZER] = _T("DEST");
    m_IniFleetNames[T_TS] = _T("RIP");
    m_IniFleetNames[T_IC] = _T("IC");
    m_IniFleetNames[T_RAK] = _T("MISS");
    m_IniFleetNames[T_LL] = _T("S_LAS");
    m_IniFleetNames[T_SL] = _T("H_LAS");
    m_IniFleetNames[T_GAUSS] = _T("GAUSS");
    m_IniFleetNames[T_IONEN] = _T("ION");
    m_IniFleetNames[T_PLASMA] = _T("PLAS");
    m_IniFleetNames[T_KS] = _T("S_SDOME");
    m_IniFleetNames[T_GS] = _T("L_SDOME");

    m_BWC_CSS = _T("bwc.css");
    m_CR_CSS = _T("cr.css");

	return;
}

CSpeedKernel::~CSpeedKernel()
{
	delete m_AttObj;
	delete m_DefObj;
}

// returns version of kernel
void CSpeedKernel::GetVersion(TCHAR* out)
{
	_tcscpy(out, KERNEL_VERSION);
}


// resets the simulator
void CSpeedKernel::Reset()
{
	m_NumShipsAtt.clear();
	m_NumShipsDef.clear();
    m_NumSetShipsAtt.clear();
    m_NumSetShipsDef.clear();
	m_AttObj->clear();
	m_DefObj->clear();
	m_DefInTF = false;
    m_FuncPtr = NULL;
    for(int r = 0; r < MAX_PLAYERS_PER_TEAM; r++)
        m_Speed[r] = 10;
    memset(m_OwnPos, 0, MAX_PLAYERS_PER_TEAM * sizeof(PlaniPos));
	memset(m_TechsAtt, 0, MAX_PLAYERS_PER_TEAM * sizeof(ShipTechs));
	memset(m_TechsDef, 0, MAX_PLAYERS_PER_TEAM * sizeof(ShipTechs));
	memset(&m_Result, 0, sizeof(BattleResult));
    CanShootAgain = CanShootAgain_V065;
    ComputeShipData();
    m_NumPlayersPerTeam[ATTER] = 0;
    m_NumPlayersPerTeam[DEFFER] = 0;

    srand((unsigned)time(NULL));
    InitRand();
}

void CheckVector(const vector<SItem>& v)
{
	SItem items[8*128];
	size_t s = v.size();
	for(size_t i = 0; i < v.size(); i++)
		items[i] = v[i];

	return;
}

void CheckVector(const vector<Obj>& v)
{
	Obj items[8*128];
	size_t s = v.size();
	for(size_t i = 0; i < v.size(); i++)
		items[i] = v[i];

	return;
}

size_t GetNextNumber(genstring str, int& num, size_t pos)
{
    size_t i;
    // find next number
    for(i = pos; i < str.length(); i++) {
        if(_istdigit(str[i])) {
            // convert into int
            num = _ttoi(str.substr(i).c_str());
            // get num of chars
            while(_istdigit(str[i]))
                i++;
            break;
        }
    }
    if(i == str.length())
        i = genstring::npos;
    return i;
}

int _stprintf(TCHAR * target, const TCHAR * format, ...)
{
    int i;
    va_list arglist;
    va_start(arglist, format);
#ifndef UNICODE
    i = vsprintf(target, format, arglist);
#else
#if _MSC_VER > 1200 || !defined(_MSC_VER)
    i =  vswprintf(target, 512, format, arglist);
#else
    i =  vswprintf(target, format, arglist);
#endif
#endif
    va_end(arglist);
    return i;
}

string wchar_to_utf8(wstring str)
{
    string utf8str;
    vector<char> utf8chars;
    if(sizeof(wchar_t) == 2)
        utf8::utf16to8(str.begin(), str.end(), back_inserter(utf8chars));
    else if(sizeof(wchar_t) == 4)
        utf8::utf32to8(str.begin(), str.end(), back_inserter(utf8chars));
    for (size_t i = 0; i < utf8chars.size(); i++)
    {
        utf8str += utf8chars[i];
    }
    return utf8str;
}
