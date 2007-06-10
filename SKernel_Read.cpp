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

#include "SpeedKernel.h"

// forwards string to reading functions and returns type of string, if known
TEXTTYPE CSpeedKernel::GeneralRead(genstring &s, int FleetID) {
    if(!ReadCR(s)) {
        if(!ReadOwnFleet(s, FleetID)) {
            if(!ReadTechs(s, FleetID)) {
                if(!ReadEspReport(s, FleetID))
                    return TEXT_UNKNOWN;
                else
                    return TEXT_ESPIO;
            }
            else
                return TEXT_TECH;
        }
        else
            return TEXT_OWNFLEET;
    }
    return TEXT_COMBAT;
}


int CSpeedKernel::MultiReadEspReport(genstring reports, TargetInfo* TIs, int nMaxTI)
{
    int nEsp = 0;
    genstring::size_type f;
    //vector<TargetInfo> vTI;
    if(!m_EspLimiter.length())
        return 0;
    f = reports.find(m_EspLimiter);
    while(f != genstring::npos && nEsp < nMaxTI)
    {
        //TargetInfo ti;
        genstring r = reports.substr(0, f);
        ReadEspReport(r, TIs[nEsp]);
        //vTI.push_back(ti);
        nEsp++;
        reports.erase(0, f + m_EspLimiter.length());
        f = reports.find(m_EspLimiter);
    }
    // set first espionage report
    if(nEsp)
        SetTargetInfo(TIs[0], 0);
    return nEsp;
}


bool CSpeedKernel::ReadEspReport(const genstring& r, int FleetID)
{
    TargetInfo ti;
    if(!ReadEspReport(r, ti))
        return false;
    if(FleetID == 0)
        m_LastReadTarget = ti;
    SetTargetInfo(ti, FleetID);
    return true;
}

// reads esp. reports
bool CSpeedKernel::ReadEspReport(genstring r, TargetInfo& TargetData)
{
	genstring tmp;
	size_t i;
    TargetInfo ti;

	genstring::size_type f = 0, f2 = 0;

    // check, how complete this esp report is
    ti.State = REPORT_RES;
    tmp = _T("\n") + m_EspTitles[0];
    if(r.find(tmp) != genstring::npos && m_EspTitles[0].length())
        ti.State = REPORT_FLEET;
    tmp  = _T("\n") + m_EspTitles[1];
    if(r.find(tmp) != genstring::npos && m_EspTitles[1].length())
        ti.State = REPORT_DEFENCE;
    tmp  = _T("\n") + m_EspTitles[2];
    if(r.find(tmp) != genstring::npos && m_EspTitles[2].length())
        ti.State = REPORT_ALL;

    tmp = r;
    // get coordinates
	f = tmp.find(m_Spiostrings[0], 0);
	f2 = tmp.find(m_Spiostrings[1], f);

	if(f != genstring::npos && f2 != genstring::npos)
	{
        genstring tmp2 = tmp.substr(f + m_Spiostrings[0].length(), f2 - f - m_Spiostrings[0].length()+1);
        tmp.erase(0, f2);
        genstring::size_type f3 = tmp2.rfind(_T("["));
        genstring name = tmp2.substr(0, f3 - 1);
        _tcsncpy(ti.Name, name.c_str(), 63);
		// extract position
        bool pos = false;
        f = 0;
        while(!pos && f != genstring::npos)
        {
            f = tmp2.find(_T("["), f + 1);
		    f2 = tmp2.find(_T("]"), f + 1);
		    if(f == genstring::npos || f2 == genstring::npos)
			    ti.Pos = PlaniPos();
		    else
			    ti.Pos = PlaniPos(tmp2.substr(f + 1, f2 - f - 1));
            if(ti.Pos.Gala)
                pos = true;
        }
	}

    // remove unimportant characters
	i = 0;
    f = f2 = 0;
    while(f != genstring::npos) {
        if((f = tmp.find(_T("<"), f)) != genstring::npos) {
            f2 = tmp.find(_T(">"), f);
            if(f2 == genstring::npos)
                tmp.erase(f, 1);
            tmp.erase(f, f2 - f + 1);
        }
        else if((f = tmp.find(_T("("))) != genstring::npos && !m_BracketNames) {
            f2 = tmp.find(_T(")"), f);
            if(f2 == genstring::npos)
                tmp.erase(f, 1);
            tmp.erase(f, f2 - f + 1);
        }
        else if((f = tmp.find(_T("["))) != genstring::npos) {
            f2 = tmp.find(_T("]"), f);
            if(f2 == genstring::npos)
                tmp.erase(f, 1);
            tmp.erase(f, f2 - f + 1);
        }
        else if((f = tmp.find(_T("|"))) != genstring::npos) {
            f2 = tmp.find(_T("|"), f + 1);
            if(f2 == genstring::npos)
                tmp.erase(f, 1);
            tmp.erase(f, f2 - f + 1);
        }
        else if((f = tmp.find(_T("\n"))) != genstring::npos) {
            tmp.erase(f, 1);
        }
        else if((f = tmp.find(_T("\t"))) != genstring::npos) {
            tmp.erase(f, 1);
        }
        else if((f = tmp.find(_T("\r"))) != genstring::npos) {
            tmp.erase(f, 1);
        }
        else if((f = tmp.find(_T("\f"))) != genstring::npos) {
            tmp.erase(f, 1);
        }
        else if((f = tmp.find(_T(":"))) != genstring::npos) {
            tmp.erase(f, 1);
        }
        else if((f = tmp.find(_T("."))) != genstring::npos) {
            tmp.erase(f, 1);
        }
    }

    if(!ParseSpioLine(tmp, ti))
        return false;
    // check, if esp. report is from moon
    if(tmp.find(m_Spiostrings[3]) != genstring::npos && m_Spiostrings[3] != _T(""))
        ti.Pos.bMoon = true;
    else
        ti.Pos.bMoon = false;

    TargetData = ti;
    return true;
}

// analyzes esp. report
bool CSpeedKernel::ParseSpioLine(genstring& s, TargetInfo& ti)
{
	genstring name, anz, item, item2;
	genstring::size_type f, f2;
	f2 = 0; f = 0;
    bool founditem = false;
    int techs[3];
    memset(techs, 0, 3 * sizeof(int));

    // read metal/crystal/deuterium and ignore mines etc.
    if(((f2 = s.find(m_Ress[0], 0)) != s.npos) && s.find(m_wrongRess[0], 0) != f2 && s.find(m_wrongRess2[0], 0) != f2)
    {
        f = f2 + m_Ress[0].length();
        anz = s.substr(f);
        ti.Resources.met = _ttoi(anz.c_str());
        if(ti.Resources.met)
            founditem = true;
    }
    if(((f2 = s.find(m_Ress[1], 0)) != s.npos) && s.find(m_wrongRess[1], 0) != f2 && s.find(m_wrongRess2[1], 0) != f2)
    {
        f = f2 + m_Ress[1].length();
        anz = s.substr(f);
        ti.Resources.kris = _ttoi(anz.c_str());
        if(ti.Resources.kris)
            founditem = true;
    }
    if(((f2 = s.find(m_Ress[2], 0)) != s.npos) && s.find(m_wrongRess[2], 0) != f2 && s.find(m_wrongRess2[2], 0) != f2)
    {
        f = f2 + m_Ress[2].length();
        anz = s.substr(f);
        ti.Resources.deut = _ttoi(anz.c_str());
        if(ti.Resources.deut)
            founditem = true;
    }

    // read in defense and fleet
    SItem sit;
    sit.OwnerID = 0;
    int num = 0;
    for(int i = 0; i <= T_END; i++)
    {
        item = ItemName((ITEM_TYPE)i);
        item2 = ItemName2((ITEM_TYPE)i);
        sit.Num = 0;
        if((f2 = s.find(item.c_str(), 0)) != genstring::npos)
        {
            f = f2 + item.length() + 1;
            anz = s.substr(f-1);
            ITEM_TYPE type = GetItemType(item);
            if(type != T_NONE) {
                sit.Type = type;
                num += sit.Num = _ttoi(anz.c_str());
                if(type < T_SHIPEND)
                    ti.Fleet.push_back(sit);
                else if(type < T_END)
                    ti.Defence.push_back(sit);
                else
                    ti.NumABM = sit.Num;
            }
        }
        // if number if items found zero, try second name
        if((f2 == genstring::npos || sit.Num == 0) && (f2 = s.find(item2.c_str(), 0)) != genstring::npos && item2 != _T(""))
        {
            f = f2 + item2.length() + 1;
            anz = s.substr(f-1);
            ITEM_TYPE type = GetItemType(item2);
            if(type != T_NONE) {
                sit.Type = type;
                num += sit.Num = _ttoi(anz.c_str());
                if(type < T_SHIPEND)
                    ti.Fleet.push_back(sit);
                else
                    ti.Defence.push_back(sit);
            }
        }
    }
    // read in techs
    for(int j = 0; j < 3; j++) {
        if((f2 = s.find(m_TechNames[j], 0)) != genstring::npos)
        {
            f = f2 + m_TechNames[j].length();
            anz = s.substr(f);
            techs[j] = _ttoi(anz.c_str());
            if(techs[j])
                founditem = true;
        }
    }

    if(founditem || num > 0) {
        ti.Techs.Weapon = techs[0];
        ti.Techs.Shield = techs[1];
        ti.Techs.Armour = techs[2];
    }
	return founditem;
}

// reads overview if you're being attacked
bool CSpeedKernel::ReadOverview(const genstring& s, int FleetID)
{
	genstring::size_type f, f2;
	vector<SItem> items;
	items.resize(T_END);
    size_t i, t;
	for(t = 0; t < T_END; t++)
	{
		items[t].Type = (ITEM_TYPE)t;
		items[t].Num = 0;
	}

	genstring tmp, pos, name, anz, name2;

	f = s.find(m_Ovr[0]);
	if(f == genstring::npos)
		return false;
	f += m_Ovr[0].length();
	f2 = s.find(m_Ovr[1], f) + 1;
    if(f2-1 == genstring::npos)
        return false;
	tmp = s.substr(f, f2 - f);
    // extract planet position
    f = tmp.find(_T("["));
    m_Result.Position = PlaniPos(tmp.substr(f+1));
	// planet name including coords
    tmp = tmp.substr(0, f) + _T(" ") + tmp.substr(f);
    _tcscpy(m_Result.PlaniName, tmp.c_str());
	tmp = s;

	for(i = 0; i < T_END; i++)
	{
		name = ItemName((ITEM_TYPE)i);
        name2 = ItemName2((ITEM_TYPE)i);
		f = tmp.find(name.c_str(), 0);
        if(f == genstring::npos && name2 != _T("")) {
                f = tmp.find(name2.c_str(), 0);
        }
		if(f != genstring::npos)
		{
			int p = f;
			while(s[p] != _T(',') && s[p] != _T('('))
				p--;
            if(p < 0)
                continue;
			anz = s.substr(p + 2, f - p - 1);
			items[i].Num = _ttoi(anz.c_str());
		}
	}
    for(i = 0; i < items.size(); i++) {
        items[i].OwnerID = FleetID;
    }
	SetFleet(&items, NULL);
	m_NumShipsAtt = items;

	return true;
}

// reads defense and own fleet
bool CSpeedKernel::ReadOwnFleet(genstring OwnFleet, int FleetID) {
    vector<SItem> items;
    vector<SItem> def;
    bool dodef = false;
	genstring name, anz, name2;
    genstring::size_type f;
    int i, num = 0;
    while((f = OwnFleet.find(_T("."))) != genstring::npos)
    {
        OwnFleet.erase(f, 1);
    }

    // fleet from fleet menu
    for(i = 0; i < T_SHIPEND; i++) {
        if(i == T_SAT)
            continue;
        name = ItemName((ITEM_TYPE)i);
        name2 = ItemName2((ITEM_TYPE)i);
        f = OwnFleet.find(name.c_str(), 0);
        if(f == genstring::npos && name2 != _T("")) {
            f = OwnFleet.find(name2.c_str(), 0);
			name = name2;
        }
        if(f != genstring::npos) {
            f += name.length();
            if(f >= OwnFleet.length())
                break;
            anz = OwnFleet.substr(f , 10);
            num += _ttoi(anz.c_str());
            SItem si;
            si.Num = _ttoi(anz.c_str());
            si.OwnerID = FleetID;
            si.Type = (ITEM_TYPE)i;
            items.push_back(si);
        }
    }
    // defense from defense menu
    for(i = T_RAK; i < T_END; i++) {
        name = ItemName((ITEM_TYPE)i) + _T(" (") + m_Defense;
        name2 = ItemName2((ITEM_TYPE)i) + _T(" (") + m_Defense;
        f = OwnFleet.find(name.c_str(), 0);
        if(f == genstring::npos && name2 != _T(" (") + m_Defense){
			name = name2;
			f = OwnFleet.find(name2.c_str(), 0);
		}
        if(f != genstring::npos) {
            dodef = true;
            f += name.length();
            if(f >= OwnFleet.length())
                break;
            int p = OwnFleet.find(_T(' '), f+1);
            if(p == genstring::npos)
                continue;
            anz = OwnFleet.substr(f, p - f);
            num += _ttoi(anz.c_str());
            SItem si;
            si.Type = (ITEM_TYPE)i;
            si.Num = _ttoi(anz.c_str());
            si.OwnerID = FleetID;
            def.push_back(si);
        }
    }
    if(!num)
        return false;
    if(dodef) {
        // add old attacking fleet
        SItem tmpItem[T_END];
        GetSetFleet(NULL, tmpItem, FleetID);
        for(i = 0; i < T_END; i++) {
            if(tmpItem[i].Type < T_SHIPEND)
                def.push_back(tmpItem[i]);
        }
        SetFleet(NULL, &def);
    }
    // if not defense, check for esp. report
    else if(OwnFleet.find(m_Ress[0]) != genstring::npos || OwnFleet.find(m_Ress[1]) != genstring::npos ||
        OwnFleet.find(m_Ress[2]) != genstring::npos) {
        return false;
    }
    else {
        SetFleet(&items, NULL);
    }
    return true;
}

// reads combat reports
bool CSpeedKernel::ReadCR(const genstring &KB) {
    vector<SItem> fleet[2][MAX_PLAYERS_PER_TEAM];
    ShipTechs techs[2][MAX_PLAYERS_PER_TEAM];
    int i;
    genstring name, anz, tmp;
	int numpl_a = 0, numpl_d = 0;
    tmp = KB;
    genstring::size_type f, f2, f3 = 0;
    genstring OrgKBNames[T_END];

    f = tmp.find(m_Attacker);
    f2 = tmp.find(m_Defender);
    if(f == genstring::npos || f2 == genstring::npos || f > f2)
        return false;
    // convert tabs to spaces and remove double spaces
    while(f3 != genstring::npos) {
        if((f3 = tmp.find(_T('\t'))) != genstring::npos) {
            tmp[f3] = _T(' ');
        }
        else if((f3 = tmp.find(_T("\r\n"))) != genstring::npos) {
            tmp.erase(f3, 1);
        }
        else if((f3 = tmp.find(_T("  "))) != genstring::npos) {
            tmp.erase(f3, 1);
        }
    }
    // save names and remove spaces from these
    for(i = 0; i < T_END; i++) {
        f3 = 0;
        OrgKBNames[i] = m_KBNames[i];
        while((f3 = m_KBNames[i].find(_T(" "), f3)) != genstring::npos) {
            f2 = 0;
            while(f2 != genstring::npos && f3 != genstring::npos) {
                f2 = tmp.find(m_KBNames[i]);
                if(f2 != genstring::npos) {
                    tmp.erase(f2 + f3, 1);
                }
                else {
                    m_KBNames[i].erase(f3, 1);
                }
            }
        }
    }

    tmp += _T(" \n");
    f = 0;
    // get attacker's / defender's tables
    size_t stoppos = tmp.find(m_Defender);
    while(((f = tmp.find(m_Attacker, f)) < stoppos) && f != genstring::npos) {
        vector<SItem> tmp_att;
        ShipTechs tmp_Techs;
        f2 = tmp.find(m_KBTable[0], f + m_Attacker.length());
        if(f2 != genstring::npos)
            f2 = tmp.find(m_Attacker, f + m_Attacker.length());
        if(f2 == genstring::npos || f2 > stoppos)
            f2 = tmp.find(m_Defender, f + m_Attacker.length());
        if(f2 == genstring::npos || f2 > stoppos)
            f2 = tmp.find(_T("\n\n"), f + m_Attacker.length());
        if(f2 > stoppos)
            f2 = stoppos;
        if(ReadCRTable(tmp.substr(f, f2-f), tmp_att, tmp_Techs)) {
            // set owner
            for(size_t k = 0; k < tmp_att.size(); k++) {
                tmp_att[k].OwnerID = numpl_a;
            }
			// set fleet / techs
            techs[0][numpl_a] = tmp_Techs;
			fleet[0][numpl_a] = tmp_att;
            f = f2;
			numpl_a++;
        }
        else
            break;
            //f += m_Attacker.length();
    }
    f = 0;
    stoppos = tmp.find(m_Attacker, stoppos);
    if(stoppos == genstr::npos)
    {
        stoppos = tmp.length();
    }
    while(((f = tmp.find(m_Defender, f)) < stoppos) && f != genstring::npos) {
        vector<SItem> tmp_def;
        ShipTechs tmp_Techs;
        f2 = tmp.find(m_KBTable[0], f + m_Defender.length());
        if(f2 != genstring::npos)
            f2 = tmp.find(m_Attacker, f + m_Attacker.length());
        if(f2 != genstring::npos || f2 > stoppos)
            f2 = tmp.find(m_Defender, f + m_Attacker.length());
        if(f2 == genstring::npos || f2 > stoppos)
            f2 = tmp.find(_T("\n\n"), f + m_Attacker.length());
        if(f2 > stoppos)
            f2 = stoppos;
        if(ReadCRTable(tmp.substr(f, f2-f), tmp_def, tmp_Techs)) {
            // set owner
            for(size_t k = 0; k < tmp_def.size(); k++) {
                tmp_def[k].OwnerID = numpl_d;
            }
            techs[1][numpl_d] = tmp_Techs;
            fleet[1][numpl_d] = tmp_def;
            numpl_d++;
            f = f2;
        }
        else
            break;
            //f += m_Defender.length();
    }

    // restore old names
    for(i = 0; i < T_END; i++) {
        m_KBNames[i] = OrgKBNames[i];
    }
    if(!numpl_a || !numpl_d)
        return false;
    // set fleet and techs
    for(i = 0; i < numpl_a; i++) {
        SetFleet(&fleet[0][i], NULL);
        SetTechs(&techs[0][i], NULL, i);
    }
    for(i = 0; i < numpl_d; i++) {
        SetFleet(NULL, &fleet[1][i]);
        SetTechs(NULL, &techs[1][i], i);
    }
    return true;
}

// read separate table from combat report
bool CSpeedKernel::ReadCRTable(genstring Table, vector<SItem> &Fleet, ShipTechs& techs) {
    genstring::size_type f=0, f2=0, f3=0;
    genstring anz;
    SItem tmp_SItem;
    int team = -1;
    // empty techs
    memset(&techs, 0, sizeof(ShipTechs));

	f2 = Table.find(m_KBTable[1]);
    Fleet.clear();
    // weapon tech
    f = Table.find(m_KBTechs[0]);
    if(f != genstring::npos && f < f2) {
        anz = Table.substr(f + m_KBTechs[0].length() + 1, 5);
        techs.Weapon = _ttoi(anz.c_str()) / 10;
    }
    // shield tech
	f = Table.find(m_KBTechs[1]);
    if(f != genstring::npos && f < f2) {
        anz = Table.substr(f+ m_KBTechs[1].length() + 1, 5);
        techs.Shield = _ttoi(anz.c_str()) / 10;
    }
    // hull tech
	f = Table.find(m_KBTechs[2]);
    if(f != genstring::npos && f < f2) {
        anz = Table.substr(f + m_KBTechs[2].length() + 1, 5);
        techs.Armour = _ttoi(anz.c_str()) / 10;
    }
    f = Table.find(m_KBTable[0]);
    if(f != genstring::npos && f < f2) {
        f += m_KBTable[0].length() + 1;
        f2 = Table.find(_T("\n"), f);
    }
    if(f2 == genstring::npos || f == genstring::npos)
        return false;

    // 'collect' unit types in correct order
    while(f < f2) {
        f3 = Table.find(_T(' '), f);
        if(f3 > f2)
            f3 = Table.find(_T('\n'), f);
        genstring tmp2 = Table.substr(f, f3-f);
        tmp_SItem.Type = GetItemType(tmp2);
        Fleet.push_back(tmp_SItem);
        f = f3 + 1;
    }
    f = Table.find(m_KBTable[1], f2 ) + m_KBTable[1].length();
    if((f2 = Table.find(_T("\n"), f)) == genstring::npos)
        f2 = Table.length();
    if(f == genstring::npos)
        return false;

    // remove dots from 'Number'-line
    for(size_t i = f; i < f2; i++) {
        if(Table[i] == '.')
            Table.erase(i, 1);
    }

    int n = 0;
    // get number of units of different types
    while(f < f2 && f < Table.length())
    {
        f3 = Table.find(_T(' '), f+1);
        if(f3 == genstring::npos)
            f3 = Table.length();
        anz = Table.substr(f, f3-f);
        if(n < Fleet.size())
            Fleet[n++].Num = _ttoi(anz.c_str());
        else
            break;
        f = f3 + 1;
    }
    // try to calculate the technologies from the table, if needed
    ITEM_TYPE t = Fleet[0].Type;
	f = Table.find(m_KBTable[1]);
    if((f2 = Table.find(m_KBTable[2], f)) != genstring::npos && techs.Weapon == 0) {
        genstring val = Table.substr(f2 + m_KBTable[2].length());
#ifdef UNICODE
        char c[10];
        wcstombs(c, val.c_str(), 9);
        float weap = atof(c);
#else
        float weap = atof(val.c_str());
#endif
        techs.Weapon = (weap / Dams[MAX_PLAYERS_PER_TEAM][ATTER][t] - 1) * 10;
    }
    if((f2 = Table.find(m_KBTable[3], f)) != genstring::npos && techs.Shield == 0) {
        genstring val = Table.substr(f2 + m_KBTable[3].length());
#ifdef UNICODE
        char c[10];
        wcstombs(c, val.c_str(), 9);
        float shield = atof(c);
#else
        float shield = atof(val.c_str());
#endif
        techs.Shield = (shield / MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][t] - 1) * 10;
    }
    if((f2 = Table.find(m_KBTable[4], f)) != genstring::npos && techs.Armour == 0) {
        genstring val = Table.substr(f2 + m_KBTable[4].length());
#ifdef UNICODE
        char c[10];
        wcstombs(c, val.c_str(), 9);
        float hull = atof(c);
#else
        float hull = atof(val.c_str());
#endif
        techs.Armour = (hull / MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][t] - 1) * 10;
    }

    return true;
}

bool CSpeedKernel::ReadTechs(const genstring &TechText, int FleetID) {
    int EngineTechs[3];
    int Techs[3];
    genstring anz, tech;
    size_t f = 0;
    bool bFound = false;

    memset(EngineTechs, 0, sizeof(int) * 3);
    memset(Techs, 0, sizeof(int) * 3);

    int i;
    f = TechText.length();
    // get weapons/shield/hull
    for(i = 0; i < 3; i++) {
        tech = m_TechNames[i] + _T(" (") + m_TechPreString;
        if((f = TechText.find(tech, 0)) != genstring::npos) {
            anz = TechText.substr(f + tech.length(), 5);
            Techs[i] = _ttoi(anz.c_str());
            if(Techs[i])
                bFound = true;
        }
    }
    // engines
    for(i = 0; i < 3; i++) {
        tech = m_TechNames[i+3] + _T(" (") + m_TechPreString;
        if((f = TechText.find(tech, 0)) != genstring::npos) {
            anz = TechText.substr(f + tech.length(), 5);
            EngineTechs[i] = _ttoi(anz.c_str());
            if(EngineTechs[i])
                bFound = true;
        }
    }
    if(!bFound)
        return false;
    ShipTechs t;
    t.Weapon = Techs[0];
    t.Shield = Techs[1];
    t.Armour = Techs[2];
    SetTechs(&t, NULL, FleetID);
    SetSpeed(10, EngineTechs[0], EngineTechs[1], EngineTechs[2], FleetID);
    return true;
}
