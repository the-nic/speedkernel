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

/*
	SKernel_Result.cpp

		-Functions for calculating result and its output

		Function listing:

			Output functions:
				void GenerateBestWorseCase(gen_ofstream& out);
				void GenerateCR(gen_ofstream& out);
                void GenerateCRTable(gen_ofstream &out, const vector<SItem> &Items, int Team, int Player, TCHAR* Title);

			Calculations after a battle:
				void ComputeLoot();
				void ComputeBattleResult();
                void ComputeLosses();

            Misc:
                BattleResult GetBattleResult();
                bool GetSetFleet(SItem* Attacker, SItem* Defender, int OwnerID);
                bool GetFleetAfterSim(SItem* Attacker, SItem* Defender, int FleetID);
*/


BattleResult& CSpeedKernel::GetBattleResult() {
    return m_Result;
}

// returns set ships (after simulating the remaining fleet)
bool CSpeedKernel::GetFleetAfterSim(SItem* Attacker, SItem* Defender, int FleetID)
{
	size_t i;

    if(Attacker && m_NumShipsAtt.size()) {
        for(i = 0; i < T_END; i++) {
            Attacker[i].Type = (ITEM_TYPE)i;
            Attacker[i].Num = 0;
            Attacker[i].OwnerID = FleetID;
        }
        for(i = 0; i < m_NumShipsAtt.size(); i++)
	    {
            if(m_NumShipsAtt[i].OwnerID == FleetID)
		        Attacker[m_NumShipsAtt[i].Type].Num += m_NumShipsAtt[i].Num;
        }
	}
    else if(Attacker) {
        for(i = 0; i < T_END; i++)
            Attacker[i].Type = T_NONE;
    }

    if(Defender && m_NumShipsDef.size()) {
        for(i = 0; i < T_END; i++) {
            Defender[i].Type = (ITEM_TYPE)i;
            Defender[i].Num = 0;
            Defender[i].OwnerID = FleetID;
        }
        for(i = 0; i < m_NumShipsDef.size(); i++)
	    {
            if(m_NumShipsDef[i].OwnerID == FleetID)
		        Defender[m_NumShipsDef[i].Type].Num = m_NumShipsDef[i].Num;
        }
    }
    else if(Defender) {
        for(i = 0; i < T_END; i++)
            Defender[i].Type = T_NONE;
    }

	return true;
}

bool CSpeedKernel::GetFleetAfterSim(vector<SItem>* Attacker, vector<SItem>* Defender, int FleetID)
{
    SItem array_att[T_END], array_def[T_END];
    if (Attacker && m_NumShipsAtt.size())
    {
        Attacker->clear();
        // get array version and convert into vector
        GetFleetAfterSim(array_att, NULL, FleetID);
        for (int i = 0; i < T_SHIPEND; i++)
        {
            if(array_att[i].Num && array_att[i].Type != T_NONE)
                Attacker->push_back(array_att[i]);
        }
    }
    if (Defender && m_NumShipsDef.size())
    {
        Defender->clear();
        // get array version and convert into vector
        GetFleetAfterSim(NULL, array_def, FleetID);
        for (int i = 0; i < T_END; i++)
        {
            if(array_def[i].Num && array_def[i].Type != T_NONE)
                Defender->push_back(array_def[i]);
        }
    }
    return true;
}

bool CSpeedKernel::GetSetFleet(SItem* Attacker, SItem* Defender, int FleetID)
{
    size_t i;
    int Num = 0;

    // prepare array
    if(Attacker)
    {
        for(i = 0; i < T_END; i++)
        {
            Attacker[i].Type = (ITEM_TYPE)i;
            Attacker[i].Num = 0;
            Attacker[i].OwnerID = FleetID;
        }
    }
    if(Attacker && m_NumSetShipsAtt.size())
    {
        for(i = 0; i < m_NumSetShipsAtt.size() && Num < T_END; i++)
	    {
            if(m_NumSetShipsAtt[i].OwnerID == FleetID)
            {
                ITEM_TYPE type = m_NumSetShipsAtt[i].Type;
		        Attacker[type] = m_NumSetShipsAtt[i];
		        Num++;
            }
        }
	}

    Num = 0;
    if(Defender)
    {
        for(i = 0; i < T_END; i++)
        {
            Defender[i].Type = (ITEM_TYPE)i;
            Defender[i].Num = 0;
            Defender[i].OwnerID = FleetID;
        }
    }
    if(Defender && m_NumSetShipsDef.size())
    {
        for(i = 0; i < m_NumSetShipsDef.size() && Num < T_END; i++)
	    {
            if(m_NumSetShipsDef[i].OwnerID == FleetID)
            {
                ITEM_TYPE type = m_NumSetShipsDef[i].Type;
		        Defender[type] = m_NumSetShipsDef[i];
		        Num++;
            }
        }
    }

	return true;
}

bool CSpeedKernel::GetSetFleet(vector<SItem>* Attacker, vector<SItem>* Defender, int FleetID)
{
    SItem array_att[T_END], array_def[T_END];
    if (Attacker && m_NumSetShipsAtt.size())
    {
        Attacker->clear();
        // get array version and convert into vector
        GetSetFleet(array_att, NULL, FleetID);
        for (int i = 0; i < T_SHIPEND; i++)
        {
            if(array_att[i].Num && array_att[i].Type != T_NONE)
                Attacker->push_back(array_att[i]);
        }
    }
    if (Defender && m_NumShipsDef.size())
    {
        Defender->clear();
        // get array version and convert into vector
        GetSetFleet(NULL, array_def, FleetID);
        for (int i = 0; i < T_END; i++)
        {
            if(array_def[i].Num && array_def[i].Type != T_NONE)
                Defender->push_back(array_def[i]);
        }
    }
    return true;
}

// writes combat report into file
bool CSpeedKernel::GenerateCR(const char* file)
{
	string out = GenerateCR();
    // write data to file
    FILE *f = fopen(file, "w+b");
    if(f) {
        fwrite(out.data(), out.length() * sizeof(char) + 1, 1, f);
        fclose(f);
        return true;
    }
    return false;
}

string CSpeedKernel::GenerateCR()
{
    TCHAR c[1024];
    size_t i;
    genstrstream out;
    string utf8_str;
	int j;
	out << _T("<html><head>") << _T("\n");
    out << _T("<link rel=stylesheet type=text/css href=\"") << m_CR_CSS << _T("\">\n");
    out << _T("<meta http-equiv=\"content-type\" content=\"text/html; charset=") << m_HTML_Charset << _T("\">") << _T("\n");
    out << _T("<title>") << m_KBTitle << KERNEL_VERSION << _T("</title></head>") << _T("\n");
	out << _T("<body align=center>") << _T("\n");
	out << _T("<br><br><br><br>") << _T("\n");
    if(!m_NumSetShipsDef.size() && !m_NumSetShipsAtt.size()) {
        out << m_KBResult[3] << _T("\n");
        out << _T("</body></html>") << _T("\n");
#ifdef UNICODE
        utf8_str = wchar_to_utf8(out.str());
#else
        utf8_str = out.str();
#endif
        return utf8_str;
    }

    // first round
    // create table for every fleet
    // attacker
    out << _T("<table width=\"100%\"><tr>") << _T("\n");
    for(j = 0; j < m_NumPlayersPerTeam[ATTER]; j++) {
        vector<SItem> vi;
        TCHAR title[128];
        _stprintf(title, _T("&nbsp;SpeedSim&nbsp;(%d)<br>"), j+1);
        for(i = 0; i < m_NumSetShipsAtt.size(); i++) {
            if(m_NumSetShipsAtt[i].OwnerID == j)
                vi.push_back(m_NumSetShipsAtt[i]);
        }
        if(vi.size() == 0)
            continue;
        out << _T("<td>") << _T("\n");
        out << _T("<center>");
        GenerateCRTable(out, vi, ATTER, j, title);
        out << _T("</center>");
        out << _T("</td>") << _T("\n");
    }
    if(!m_NumSetShipsAtt.size())
        out << _T("<td>") << m_Attacker << _T(" SpeedSim <br><b>") << m_KBResult[8] << _T("</b><br><br></td>") << _T("\n");
    out << _T("</tr></table><br><br>") << _T("\n");

    // defender's fleet
    out << _T("<table width=\"100%\"><tr>") << _T("\n");
    for(j = 0; j < m_NumPlayersPerTeam[DEFFER]; j++) {
        vector<SItem> vi;
        TCHAR title[128];
        _stprintf(title, _T("&nbsp;SpeedSim&nbsp;(%d)<br>"), j+1);
        for(i = 0; i < m_NumSetShipsDef.size(); i++) {
            if(m_NumSetShipsDef[i].OwnerID == j)
                vi.push_back(m_NumSetShipsDef[i]);
        }
        if(vi.size() == 0)
            continue;
        out << _T("<td>") << _T("\n");
        out << _T("<center>");
        GenerateCRTable(out, vi, DEFFER, j, title);
        out << _T("</center>");
        out << _T("</td>") << _T("\n");
    }
    if(!m_NumSetShipsDef.size())
        out << _T("<td>") << m_Defender << _T(" SpeedSim <br><b>") << m_KBResult[8] << _T("</b><br><br></td>") << _T("\n");
    out << _T("</tr></table>") << _T("\n");

    if(m_NumSetShipsAtt.size() > 0 && m_NumSetShipsDef.size() > 0) {
        _stprintf(c, m_KBRoundStr[0].c_str(), m_NumShotsPerRoundAtt[0], m_ShotStrengthAtt[0]);
        out << c;
        out << _T("<br>");
        _stprintf(c, m_KBRoundStr[1].c_str(), m_AbsorbedDef[0]);
        out << c;
        out << _T("<br><br>");
        _stprintf(c, m_KBRoundStr[2].c_str(), m_NumShotsPerRoundDef[0], m_ShotStrengthDef[0]);
        out << c;
        out << _T("<br>");
        _stprintf(c, m_KBRoundStr[3].c_str(), m_AbsorbedAtt[0]);
        out << c;
        out << _T("<br><br>");
    }

	for(int round = 1; round < m_Result.NumRounds; round++)
	{
        out << _T("<center>");
        out << _T("\n");
        out << _T("<table><tr><td>");
        out << _T("<center>");
        GenerateCRTable(out, m_NumShipsInKBAtt[round], ATTER, -1, _T("&nbsp;SpeedSim&nbsp;[1:1:1]"));
        out << _T("</center>");
        out << _T("</td></tr></table>");
        out << _T("\n");
        out << _T("<table><tr><td>");
        out << _T("<center>");
        GenerateCRTable(out, m_NumShipsInKBDef[round], DEFFER, -1, _T("&nbsp;SpeedSim&nbsp;[1:1:1]"));
        out << _T("</center>");
        out << _T("</td></tr></table>");
        out << _T("</center>");
        out << _T("\n");

		if(!m_NumShotsPerRoundAtt[round])
			break;
		if(!m_NumShotsPerRoundDef[round])
			break;

        _stprintf(c, m_KBRoundStr[0].c_str(), m_NumShotsPerRoundAtt[round], m_ShotStrengthAtt[round]);
		out << c << _T("<br>");
		_stprintf(c, m_KBRoundStr[1].c_str(), m_AbsorbedDef[round]);
		out << c << _T("<br><br>");
        _stprintf(c, m_KBRoundStr[2].c_str(), m_NumShotsPerRoundDef[round], m_ShotStrengthDef[round]);
		out << c << _T("<br>");
		_stprintf(c, m_KBRoundStr[3].c_str(), m_AbsorbedAtt[round]);
		out << c << _T("<br><br>");
	}

    // last round
    // attacker
    if(m_NumSetShipsAtt.size() && m_NumSetShipsDef.size()) {
        out << _T("<table width=\"100%\"><tr>") << _T("\n");
        for(j = 0; j < m_NumPlayersPerTeam[ATTER]; j++) {
            vector<SItem> vi;
            TCHAR title[128];
            _stprintf(title, _T("&nbsp;SpeedSim&nbsp;(%d)<br>"), j+1);
            for(i = 0; i < m_NumShipsAtt.size(); i++) {
                if(m_NumShipsAtt[i].OwnerID == j) {
                    vi.push_back(m_NumShipsAtt[i]);
                }
            }
            if(vi.size() == 0) {
                out << _T("<td>") << m_Attacker << title << _T("<br><b>") << m_KBResult[8] << _T("</b></td><br><br>") << _T("\n");
                continue;
            }
            out << _T("<td>") << _T("\n");
            out << _T("<center>");
            GenerateCRTable(out, vi, ATTER, j, title);
            out << _T("</center>");
            out << _T("</td>") << _T("\n");
        }
        out << _T("</tr></table><br><br>") << _T("\n");

        // fleet of defender
        out << _T("<table width=\"100%\"><tr>") << _T("\n");
        for(j = 0; j < m_NumPlayersPerTeam[DEFFER]; j++) {
            vector<SItem> vi;
            TCHAR title[128];
            _stprintf(title, _T("&nbsp;SpeedSim&nbsp;(%d)<br>"), j+1);
            for(i = 0; i < m_NumShipsDef.size(); i++) {
                if(m_NumShipsDef[i].OwnerID == j) {
                    vi.push_back(m_NumShipsDef[i]);
                }
            }
            if(vi.size() == 0) {
                out << _T("<td>") << m_Defender << title << _T("<br><b>") << m_KBResult[8] << _T("</b><br><br></td>") << _T("\n");
                continue;
            }
            out << _T("<td>") << _T("\n");
            out << _T("<center>");
            GenerateCRTable(out, vi, DEFFER, j, title);
            out << _T("</center>");
            out << _T("</td>") << _T("\n");
        }
        out << _T("</tr></table><br><br>") << _T("\n");
    }

    out << _T("<p>");
    if(m_Result.AttWon != 0 && m_Result.DefWon == 0 && m_Result.Draw == 0)
	{
		out << m_KBResult[0] << _T("<br>\n");
        _stprintf(c, m_KBResult[5].c_str(), (int)m_Result.Beute.met, (int)m_Result.Beute.kris, (int)m_Result.Beute.deut);
		out << c;
	}
	else if(m_Result.DefWon != 0 && m_Result.AttWon == 0 && m_Result.Draw == 0)
	{
		out << m_KBResult[1];
	}
	else if(m_Result.Draw != 0 && m_Result.AttWon == 0 && m_Result.DefWon == 0)
	{
		out << m_KBResult[2];
	}
	else if(m_Result.Draw == 0 && m_Result.AttWon == 0 && m_Result.DefWon == 0)
	{
		out << m_KBResult[3] << _T("\n");
	}
	else
	{
		float num = m_Result.AttWon + m_Result.DefWon + m_Result.Draw;
		out << m_KBResult[4] << _T("<br>");
		_stprintf(c, m_KBResult[6].c_str(), 100 * (m_Result.AttWon / num), 100 * m_Result.DefWon / num );
		out << c;
        out << _T("<br>");
        _stprintf(c, m_KBResult[7].c_str(), 100 * m_Result.Draw / num);
		out << c;
	}

	TCHAR tmpchar[512];
	out << _T("<br><br>");
    _stprintf(c, m_KBLoss[0].c_str(), AddPointsToNumber(m_Result.VerlusteAngreifer.met + m_Result.VerlusteAngreifer.kris, tmpchar), AddPointsToNumber(m_Result.VerlusteAngreifer.deut, &tmpchar[256]));
	out << c;
    out << _T("<br>");
	_stprintf(c, m_KBLoss[1].c_str(), AddPointsToNumber(m_Result.VerlusteVerteidiger.met + m_Result.VerlusteVerteidiger.kris, tmpchar), AddPointsToNumber(m_Result.VerlusteVerteidiger.deut, &tmpchar[256]));
	out << c;
    out << _T("<br>");
	_stprintf(c, m_KBLoss[2].c_str(), AddPointsToNumber(m_Result.TF.met, tmpchar), AddPointsToNumber(m_Result.TF.kris, &tmpchar[32]));
	out << c;
    out << _T("<br><br>");
	int chance = (m_Result.TF.met + m_Result.TF.kris) / 100000;
	if(chance > 20)
		chance = 20;
	_stprintf(c, m_KBMoon.c_str(), chance);
	out << c;
    out << _T("</p>");
    out << _T("<br><br><br><br>");
    out << _T("<b>Automatically generated by <a href=\"http://www.speedsim.net/eng\" target=\"_blank\">SpeedSim</a>");
    out << _T(" Kernel v") << KERNEL_VERSION << _T("</b>") << _T("\n");
	out << _T("</body></html>") << _T("\n");
#ifdef UNICODE
    utf8_str = wchar_to_utf8(out.str());
#else
    utf8_str = out.str();
#endif
    return utf8_str;
}

void CSpeedKernel::GenerateCRTable(genstrstream &out, const vector<SItem> &Items, int Team, int Player, TCHAR* Title) {
    size_t k;
    TCHAR c[1024];
    out << (Team == ATTER ? m_Attacker : m_Defender) << Title << _T("\n");
    if(Player != -1) {
        ShipTechs techs = (Team == ATTER ? m_TechsAtt[Player] : m_TechsDef[Player]);
        out << m_KBTechs[0] << _T(":+") << techs.Weapon * 10 << _T("%&nbsp;") << m_KBTechs[1]
            << _T(":+") << techs.Shield * 10 << _T("%&nbsp;") << m_KBTechs[1] << _T(":+")
            << techs.Armour * 10 << _T("%\n");
    }
    else
        Player = 0;

    // create table
    out << _T("<table><tr>") << _T("\n");
    out << _T("<td><b>") << m_KBTable[0] << _T("</b></td>");
    // item names
    for(k = 0; k < Items.size(); k++)
    {
        if(Items[k].Num)
            out << _T("<td><b>") << CRItemName((ITEM_TYPE)Items[k].Type) << _T("</b></td>");
    }
    out << _T("</tr>") << _T("\n") << _T("<tr><td><b>") << m_KBTable[1] << _T("</b></td>");
    // number of ships
    for(k = 0; k < Items.size(); k++)
    {
        if(Items[k].Num)
        {
            int n = ceil(Items[k].Num - 0.5f);
			out << _T("<td>") << n << _T("</td>");
        }
    }

    out << _T("</tr>") << _T("\n") << _T("<tr><td><b>") << m_KBTable[2] << _T("</b></td>");
    // damage
    for(k = 0; k < Items.size(); k++)
    {
        if(Items[k].Num)
        {
            double dam = Dams[Player][Team][Items[k].Type];
            if(ceil(dam) == dam)
                _stprintf(c, _T("<td>%.0f</td>"), dam);
            else
                _stprintf(c, _T("<td>%.1f</td>"), dam);
            out << c;
        }
    }
    // shield
    out << _T("</tr>") << _T("\n") << _T("<tr><td><b>") << m_KBTable[3] << _T("</b></td>");
    for(k = 0; k < Items.size(); k++)
    {
        if(Items[k].Num)
        {
            double shield = MaxShields[Player][Team][Items[k].Type];
            if(ceil(shield) == shield)
                _stprintf(c, _T("<td>%.0f</td>"), shield);
            else
                _stprintf(c, _T("<td>%.1f</td>"), shield);
            out << c;
        }
    }
    // hull
    out << _T("</tr>") << _T("\n") << _T("<tr><td><b>") << m_KBTable[4] << _T("</b></td>");
    for(k = 0; k < Items.size(); k++)
    {
        if(Items[k].Num)
        {
            double hull = MaxLifes[Player][Team][Items[k].Type];
            if(ceil(hull) == hull)
                _stprintf(c, _T("<td>%.0f</td>"), hull);
            else
                _stprintf(c, _T("<td>%.1f</td>"), hull);
            out << c;
        }
    }
    out << _T("</tr></table><br><br>") << _T("\n");
}

// possible loot
void CSpeedKernel::ComputeLoot()
{
	Res Beute;
    Res mgl = m_Result.RessDa;
    mgl /= 2;
    Res th = mgl;
	unsigned long LadeKap = 0, tmplade;
	for(size_t i = 0; i < m_NumShipsAtt.size(); i++)
		LadeKap += floor(m_NumShipsAtt[i].Num + 0.5f) * LadeKaps[m_NumShipsAtt[i].Type];
	tmplade = LadeKap;

    // needed capacity
    unsigned long ulNeededCap = mgl.met + mgl.kris + mgl.deut;
    double fPercLoot = 0;

    // Kelder's new algorithm for needed capacity
    m_Result.iNeededCapacity = MAX(th.met + th.kris + th.deut, MIN((2 * th.met + th.kris + th.deut) * 3 / 4, (2 * th.met + th.deut)));

    //m_Result.iNeededCapacity = ulNeededCap;
    m_Result.Beute = Res();

    if(m_Result.AttWon > 0) {
        mgl = th;
        // real plunder
        LadeKap = tmplade;
        // get metal
        if(mgl.met < LadeKap / 3) {
            Beute.met = mgl.met;
            mgl.met = 0;
        }
        else {
            Beute.met = (__int64)ceil((float)LadeKap / 3.f);
            mgl.met -= (__int64)ceil((float)LadeKap / 3.f);
        }
        LadeKap -= Beute.met;
        // crystal
        if(mgl.kris < LadeKap / 2) {
            Beute.kris = mgl.kris;
            mgl.kris = 0;
        }
        else {
            Beute.kris = (__int64)ceil((float)LadeKap / 2.f);
            mgl.kris -= (__int64)ceil((float)LadeKap / 2.f);
        }
        LadeKap -= Beute.kris;
        // deut
        if(mgl.deut < LadeKap) {
            Beute.deut = mgl.deut;
            mgl.deut = 0;
        }
        else {
            Beute.deut = LadeKap;
            mgl.deut -= LadeKap;
        }
        LadeKap -= Beute.deut;

        // again metal
        if(mgl.met < LadeKap / 2) {
            Beute.met += mgl.met;
            LadeKap -= mgl.met;
        }
        else {
            Beute.met += (__int64)ceil((float)LadeKap / 2.f);
            LadeKap -= (__int64)ceil((float)LadeKap / 2.f);
        }
        // again crystal
        if(mgl.kris < LadeKap) {
            Beute.kris += mgl.kris;
            LadeKap -= mgl.kris;
        }
        else {
            Beute.kris += LadeKap;
            LadeKap = 0;
        }
   	    m_Result.Beute.met += Beute.met;
        m_Result.Beute.kris += Beute.kris;
        m_Result.Beute.deut += Beute.deut;

        mgl = th;

        if(mgl.met + mgl.kris + mgl.deut > 0)
            m_Result.Ausbeute += 100 * (Beute.met + Beute.kris + Beute.deut) / (mgl.met + mgl.kris + mgl.deut);
        else
            m_Result.Ausbeute += 100;
    }
    m_Result.GesamtBeute = m_Waves.TotalPlunder + m_Result.Beute;
    m_Result.NumAtts = m_Waves.NumAtts + 1;
    m_Result.GesamtRecs = m_Waves.TotalRecs + m_Result.MaxNumRecs;
    m_Result.GesVerlust = m_Waves.TotalLosses + m_Result.VerlusteAngreifer;
    m_Result.GesTF = m_Waves.TotalDebris + m_Result.TF;
}

// computes losses/debris and battle result
void CSpeedKernel::ComputeBattleResult()
{
	ComputeLosses();
    ComputeLoot();
	size_t i;
    // remove empty ships
    for(i = 0; i < m_NumShipsAtt.size(); i++) {
		vector<SItem>::iterator it = m_NumShipsAtt.begin() + i;
        if(!it->Num || it->Type == T_NONE) {
            m_NumShipsAtt.erase(it);
            i--;
        }
    }
    for(i = 0; i < m_NumShipsDef.size(); i++) {
		vector<SItem>::iterator it = m_NumShipsDef.begin() + i;
        if(!it->Num || it->Type == T_NONE) {
            m_NumShipsDef.erase(it);
            i--;
        }
    }

	if(m_Result.VerlusteAngreifer.met != 0)
		m_Result.PercentInTFMet = 100 * ((float)m_Result.TF.met / m_Result.VerlusteAngreifer.met);
	else
		m_Result.PercentInTFMet = 0;

	if(m_Result.VerlusteAngreifer.kris != 0)
		m_Result.PercentInTFKris = 100 * ((float)m_Result.TF.kris / m_Result.VerlusteAngreifer.kris);
	else
		m_Result.PercentInTFKris = 0;

	if(m_Result.Ausbeute > 100)
		m_Result.Ausbeute = 100;

	// fuel calculation
    m_Result.FlyTime = 0;
	if(m_Result.Position.Gala)
    {
        m_Result.SpritVerbrauch = 0;
        m_Result.FlyTime = 0;
        float minSpeed = 9999999.0f;
        unsigned int uiTime = 0;
        // get minimum speed of all fleets
        for(int i = 0; i < MAX_PLAYERS_PER_TEAM; i++) {
            float sp = GetFleetSpeed(i, m_NumSetShipsAtt);
            if(sp < minSpeed)
                minSpeed = sp;
        }
        for(int pl = 0; pl < MAX_PLAYERS_PER_TEAM; pl++) {
            int dist = GetDistance(m_OwnPos[pl], m_Result.Position);

            // fly time for recyclers
            vector<SItem> vRec;
            vRec.push_back(SItem(T_REC, 1, pl));
            m_Result.RecFlyTime[pl] = ComputeFlyTime(m_OwnPos[pl], m_Result.Position, pl, vRec);
            // fuel for recyclers
            float rec_bsp = BaseSpeed[T_REC] * (1 + (float)m_TechsTW[pl][Triebwerke[T_REC]] * (Triebwerke[T_REC] + 1) / 10.0f);
            float rec_sp = 35000.f / (m_Result.RecFlyTime[pl] - 10) * sqrt(dist * 10 / rec_bsp);
            m_Result.RecFuel[pl] = m_Result.NumRecs * Verbrauch[T_REC] * (rec_sp / 10.0f + 1) * (rec_sp / 10.0f + 1) * dist / 35000.0f + 1;

            // compute separate fuel need for every fleet
            uiTime = ComputeFlyTime(m_OwnPos[pl], m_Result.Position, pl, m_NumSetShipsAtt);
            if(m_OwnPos[pl].Gala == 0 && m_OwnPos[pl].Sys == 0 && m_OwnPos[pl].Pos == 0)
                continue;
            __int64 gesverb = 0;
            // sum consumptions
            if(m_NewFuel) {
                // OGame >= v0.68a
                for(size_t i = 0; i < m_NumSetShipsAtt.size(); i++) {
                    if(m_NumSetShipsAtt[i].OwnerID == pl) {
                        int type = m_NumSetShipsAtt[i].Type;
                        float basesp = BaseSpeed[type];
                        int engine = Triebwerke[type];
                        int iConsumpt = Verbrauch[type];
                        if(type == T_KT && m_TechsTW[pl][engine + 1] >= 5) {
                            basesp *= 2;
                            engine += 1;
                            iConsumpt *= 2;
                        }
                        if(type == T_BOMBER && m_TechsTW[pl][engine + 1] >= 8) {
                            basesp = 5000;
                            engine += 1;
                        }
                        basesp *= (1 + (float)m_TechsTW[pl][engine] * (engine + 1) / 10.0f);
                        float sp = 35000.f / (uiTime - 10) * sqrt(dist * 10 / basesp);
                        gesverb += m_NumSetShipsAtt[i].Num * iConsumpt * (sp / 10.0f + 1) * (sp / 10.0f + 1);
                    }
                }
                m_Result.SpritVerbrauch += gesverb * dist / 35000.0f + 1;
            }
            else {
                // OGame < v0.68a
                for(size_t i = 0; i < m_NumSetShipsAtt.size(); i++) {
                    if(m_NumSetShipsAtt[i].OwnerID == pl)
                        gesverb += m_NumSetShipsAtt[i].Num * Verbrauch[m_NumSetShipsAtt[i].Type];
                }
                m_Result.SpritVerbrauch += gesverb * dist / 35000.0f * (m_Speed[pl]/10.0f+1) * (m_Speed[pl]/10.0f+1)+1;
            }
            // get slowest fleet
            if(uiTime > m_Result.FlyTime)
                m_Result.FlyTime = uiTime;
        }
	}
	else
		m_Result.SpritVerbrauch = m_Result.FlyTime = 0;

	m_Result.GewinnOhneTF = m_Result.Beute - m_Result.VerlusteAngreifer;
	m_Result.GewinnMitHalfTF = m_Result.Beute + m_Result.TF * 0.5f - m_Result.VerlusteAngreifer;
	m_Result.GewinnMitTF = m_Result.Beute + m_Result.TF - m_Result.VerlusteAngreifer;
	m_Result.GewinnOhneTF.deut -= m_Result.SpritVerbrauch;
	m_Result.GewinnMitHalfTF.deut -= m_Result.SpritVerbrauch;
	m_Result.GewinnMitTF.deut -= m_Result.SpritVerbrauch;

    m_Result.GewinnOhneTF_def = m_Result.Beute * -1 - m_Result.VerlVertmitAufbau;
    m_Result.GewinnMitHalfTF_def = m_Result.TF * 0.5 - m_Result.Beute - m_Result.VerlVertmitAufbau;
	m_Result.GewinnMitHalfTF_def.deut = -m_Result.Beute.deut - m_Result.VerlVertmitAufbau.deut;
    m_Result.GewinnMitTF_def = m_Result.TF - m_Result.Beute - m_Result.VerlVertmitAufbau;
	m_Result.GewinnMitTF_def.deut = -m_Result.Beute.deut - m_Result.VerlVertmitAufbau.deut;
}

void CSpeedKernel::ComputeLosses()
{
	size_t i = 0;
	Res verl;
	vector<SItem> anf, uebrig;

	// count all ships - attacker
	anf.resize(T_END); uebrig.resize(T_END);
	for(i = 0; i < T_END; i++)
	{
		anf[i].Type = (ITEM_TYPE)i;
		uebrig[i].Type = (ITEM_TYPE)i;
		anf[i].Num = 0;
		uebrig[i].Num = 0;
	}

	for(i = 0; i < m_NumSetShipsAtt.size(); i++)
		anf[m_NumSetShipsAtt[i].Type].Num += m_NumSetShipsAtt[i].Num;
	for(i = 0; i < m_NumShipsAtt.size(); i++)
		uebrig[m_NumShipsAtt[i].Type].Num += m_NumShipsAtt[i].Num;

	for(i = 0; i < anf.size(); i++)
    {
        DWORD Type = anf[i].Type;
        DWORD SetShips = anf[i].Num;
        verl = Kosten[Type] * (SetShips - ceil(uebrig[i].Num - 0.5));
		m_Result.VerlusteAngreifer += verl;

		if(i < T_SHIPEND)
			m_Result.TF += verl /10*3;
		else if(m_DefInTF)
			m_Result.TF += verl /10*3;

        // Worst-Case
		verl = Kosten[Type] * (anf[i].Num - m_WorstCaseAtt[i]);
		m_Result.MaxVerlAtt += verl;
		if(i < T_SHIPEND)
			m_Result.MaxTF += verl/10*3;
		else if(m_DefInTF)
			m_Result.MaxTF += verl /10*3;

        // Best-Case
		verl = Kosten[Type] * (anf[i].Num - m_BestCaseAtt[i]);
		m_Result.MinVerlAtt += verl;
		if(i < T_SHIPEND)
			m_Result.MinTF += verl /10*3;
		else if(m_DefInTF)
			m_Result.MinTF += verl /10*3;

   		m_Result.WertAtt += Kosten[Type] * anf[i].Num;
    }
#ifdef CREATE_ADV_STATS
    for (i = 0; i < m_CombatResultsAtt.size(); i++)
    {
        // losses information
        Res losses = Res();
        for (j = 0; j < T_END; j++)
        {
            losses += Kosten[j] * (anf[j].Num - m_CombatResultsAtt[i][j]);
        }
        m_LossAtt[i] = losses;
        m_DebrisFields[i] = losses * 3 / 10;
    }
#endif

	// count all ships - defender
	anf.resize(T_END); uebrig.resize(T_END);
	for(i = 0; i < T_END; i++)
	{
		anf[i].Type = (ITEM_TYPE)i;
		uebrig[i].Type = (ITEM_TYPE)i;
		anf[i].Num = 0;
		uebrig[i].Num = 0;
	}

	for(i = 0; i < m_NumSetShipsDef.size(); i++)
		anf[m_NumSetShipsDef[i].Type].Num += m_NumSetShipsDef[i].Num;
	for(i = 0; i < m_NumShipsDef.size(); i++)
		uebrig[m_NumShipsDef[i].Type].Num += m_NumShipsDef[i].Num;

    for(i = 0; i < anf.size(); i++)
    {
		verl = Kosten[i] * (anf[i].Num - ceil(uebrig[i].Num - 0.5));
		m_Result.VerlusteVerteidiger += verl;
		if(i < T_SHIPEND)
			m_Result.TF += verl /10*3;
		else if(m_DefInTF)
			m_Result.TF += verl /10*3;
        if(i < T_SHIPEND)
            m_Result.VerlVertmitAufbau += verl;
        else
            m_Result.VerlVertmitAufbau += verl * (1-DEF_AUFBAU_FAKTOR);

		verl = Kosten[i] * (anf[i].Num - m_WorstCaseDef[i]);
		m_Result.MaxVerlDef += verl;
		if(i < T_SHIPEND)
			m_Result.MaxTF += verl /10*3;
		else if(m_DefInTF)
			m_Result.MaxTF += verl /10*3;


		verl = Kosten[i] * (anf[i].Num - m_BestCaseDef[i]);
		m_Result.MinVerlDef += verl;
		if(i < T_SHIPEND)
			m_Result.MinTF += verl /10*3;
		else if(m_DefInTF)
			m_Result.MinTF += verl /10*3;

   		m_Result.WertDef += Kosten[i] * anf[i].Num;
	}
#ifdef CREATE_ADV_STATS
    for (i = 0; i < m_CombatResultsDef.size(); i++)
    {
        // losses information
        Res losses = Res();
        for (j = 0; j < T_END; j++)
        {
            losses += Kosten[j] * (anf[j].Num - m_CombatResultsDef[i][j]);
        }
        m_LossDef[i] = losses;
        m_DebrisFields[i] += losses * 3 / 10;
    }
#endif

	m_Result.NumRecs = ceil((m_Result.TF.met + m_Result.TF.kris) / 20000.0f);
	m_Result.MaxNumRecs = ceil((m_Result.MaxTF.met + m_Result.MaxTF.kris) / 20000.0f);
	m_Result.MinNumRecs = ceil((m_Result.MinTF.met + m_Result.MinTF.kris) / 20000.0f);
    m_Result.TF.deut = 0;
	m_Result.MaxTF.deut = 0;
}

bool CSpeedKernel::GenerateBWC(const char* file)
{
    if(!m_CompBestWorstCase)
        return false;
    string out = GenerateBWC();

    // write data to file
    FILE *f = fopen(file, "wb");
    if(f) {
        fwrite(out.data(), out.length() * sizeof(char) + 1, 1, f);
        fclose(f);
        return true;
    }
    return false;
}

string CSpeedKernel::GenerateBWC()
{
    int i;
	TCHAR tmp[512];
    genstrstream out;
    genstring tmp2;
#ifdef CREATE_ADV_STATS
    CreateAdvShipStats();
#endif

	out << _T("<html><head>") << _T("\n");
    out << _T("<link rel=stylesheet type=text/css href=\"") << m_BWC_CSS << _T("\">\n");
    out << _T("<meta http-equiv=\"content-type\" content=\"text/html; charset=") << m_HTML_Charset << _T("\">") << _T("\n");
    out << _T("<title>") << m_BWTitle[0] << _T("</title></head>") << _T("\n");
	out << _T("<body>") << _T("\n");
    out << _T("<center>");
	out << _T("<h3 align=\"center\">") << m_BWTitle[0] << _T("</h3>") << _T("\n");
	out << _T("<table border=\"0\" cellpadding=\"20\">") << _T("\n");
	out << _T("  <tr><td width=\"400\">") << _T("\n");
	out << _T("    <h4 align=\"center\">") << m_BWTitle[1] << _T("</h4>") << _T("\n");
	out << _T("    <table border=\"2\" cellpadding=\"2\" cellspacing=\"2\" align=\"center\">") << _T("\n");
	out << _T("      <tr>") << _T("\n");
	out << _T("        <td width=\"170\"><b>") << m_BWTable[5] << _T("</b></td>") << _T("\n");
	out << _T("        <td width=\"60\" align=\"center\"><b>") << m_BWTable[7] << _T("</b></td>") << _T("\n");
	out << _T("        <td width=\"60\" align=\"center\"><b>") << m_BWTable[8] << _T("</b></td>") << _T("\n");
	out << _T("      </tr>") << _T("\n");
	for(i = 0; i < T_SHIPEND; i++)
	{
		if(i == T_SAT)
			continue;
		out << _T("      <tr><td>") << ItemName((ITEM_TYPE)i) << _T("</td>") << _T("\n");
		out << _T("        <td>") << AddPointsToNumber(m_BestCaseAtt[i], tmp) << _T("</td>") << _T("\n");
		out << _T("        <td align=\"right\">") << AddPointsToNumber(m_WorstCaseAtt[i], tmp) << _T("</td>") << _T("\n");
		out << _T("      </tr>") << _T("\n");
	}

	out << _T("     </table>") << _T("\n");
	out << _T("  </td>") << _T("\n");
	out << _T("  <td>") << _T("\n");
	out << _T("  <h4 align=\"center\">") << m_BWLoss[0] << _T("</h4>") << _T("\n");
    out << _T("  ") << m_BWLoss[1] << _T(" <b>") << AddPointsToNumber(m_Result.WertAtt.met + m_Result.WertAtt.kris, tmp) << _T("</b> ") << m_BWLoss[3] << _T(" (+ ") << AddPointsToNumber(m_Result.WertAtt.deut, &tmp[64]) << _T(" ") << m_Ress[2] << _T(")<br>") << _T("\n");
    out << _T("  ") << m_BWLoss[2] << _T(" <b>") << AddPointsToNumber(m_Result.WertDef.met + m_Result.WertDef.kris, tmp) << _T("</b> ") << m_BWLoss[3] << _T(" (+ ") << AddPointsToNumber(m_Result.WertDef.deut, &tmp[64]) << _T(" ") << m_Ress[2] << _T(")<br>") << _T("\n");
	out << _T("  <table border=\"2\" cellpadding=\"3\">") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td><b>") << m_Attacker << _T("</b></td>") << _T("\n");
	out << _T("	<td align=\"center\"><b>") << m_Ress[0] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_Ress[1] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_Ress[2] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_BWTable[3] << _T("</b></td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[0] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlAtt.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlAtt.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlAtt.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlAtt.met + m_Result.MinVerlAtt.kris + m_Result.MinVerlAtt.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[1] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteAngreifer.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteAngreifer.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteAngreifer.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteAngreifer.met + m_Result.VerlusteAngreifer.kris + m_Result.VerlusteAngreifer.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[2] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlAtt.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlAtt.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlAtt.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlAtt.met + m_Result.MaxVerlAtt.kris + m_Result.MaxVerlAtt.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	/////////////////// Defender
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td><b>") << m_Defender << _T("</b></td>") << _T("\n");
	out << _T("	<td align=\"center\"><b>") << m_Ress[0] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_Ress[1] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_Ress[2] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_BWTable[3] << _T("</b></td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[0] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlDef.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlDef.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlDef.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinVerlDef.met + m_Result.MinVerlDef.kris + m_Result.MinVerlDef.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[1] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteVerteidiger.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteVerteidiger.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteVerteidiger.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlusteVerteidiger.met + m_Result.VerlusteVerteidiger.kris + m_Result.VerlusteVerteidiger.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[2] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlDef.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlDef.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlDef.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxVerlDef.met + m_Result.MaxVerlDef.kris + m_Result.MaxVerlDef.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[6] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlVertmitAufbau.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlVertmitAufbau.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlVertmitAufbau.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.VerlVertmitAufbau.met + m_Result.VerlVertmitAufbau.kris + m_Result.VerlVertmitAufbau.deut, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	///////// Debris
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td><b>") << m_BWTable[4] << _T("</b></td>") << _T("\n");
	out << _T("	<td align=\"center\"><b>") << m_Ress[0] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_Ress[1] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_BWTable[3] << _T("</b></td>") << _T("\n");
	out << _T("    <td align=\"center\"><b>") << m_FleetNames[T_REC] << _T("</b></td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[0] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinTF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinTF.met + m_Result.MinTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MinNumRecs, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[1] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.TF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.TF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.TF.met + m_Result.TF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.NumRecs, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("    <td>") << m_BWTable[2] << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxTF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxTF.met + m_Result.MaxTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("    <td align=\"right\">") << AddPointsToNumber(m_Result.MaxNumRecs, tmp) << _T("</td>") << _T("\n");
	out << _T("   </tr>") << _T("\n");
	out << _T("   <tr>") << _T("\n");
	out << _T("  </table>") << _T("\n");

	out << _T("  </td>") << _T("\n");
	out << _T("  </tr>") << _T("\n");

	////////////// Defender
	out << _T("  <tr valign=top><td width=\"400\">") << _T("\n");
	out << _T("    <h4 align=\"center\">") << m_BWTitle[2] << _T("</h4>") << _T("\n");
	out << _T("    <table border=\"2\" cellpadding=\"2\" cellspacing=\"2\" align=\"center\">") << _T("\n");
	out << _T("      <tr>") << _T("\n");
	out << _T("        <td width=\"170\"><b>") << m_BWTable[5] << _T("</b></td>") << _T("\n");
	out << _T("        <td width=\"60\" align=\"center\"><b>") << m_BWTable[7] <<  _T("</b></td>") << _T("\n");
	out << _T("        <td width=\"60\" align=\"center\"><b>") << m_BWTable[8] << _T("</b></td>") << _T("\n");
	out << _T("      </tr>") << _T("\n");
	for(i = 0; i < T_END; i++)
	{
		if(i == T_RAK)
			out << _T("<tr><td colspan=\"3\">&nbsp;</td></tr>") << _T("\n");

		out << _T("      <tr><td>") << ItemName((ITEM_TYPE)i) << _T("</td>") << _T("\n");
		out << _T("        <td align=\"right\">") << AddPointsToNumber(m_BestCaseDef[i], tmp) << _T("</td>") << _T("\n");
		out << _T("        <td align=\"right\">") << AddPointsToNumber(m_WorstCaseDef[i], tmp) << _T("</td>") << _T("\n");
		out << _T("      </tr>") << _T("\n");
	}

	out << _T("     </table></td>") << _T("\n");
	out << _T("<td valign=\"top\"><h4>") << m_Bilanzstrings[0] << _T("</h4>") << _T("\n");
	out << _T("\n");
	out << _T("<table border=\"2\" cellpadding=\"2\" cellspacing=\"2\" align=\"center\">");
    out << _T("<tr><td><b>") << m_Attacker << _T("</b></td><td>");
	out << m_Ress[0] << _T("</td><td>") << m_Ress[1] << _T("</td><td>") << m_Ress[2] << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[1] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[2] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[3] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("</table><br><br>");

	out << _T("\n");
	out << _T("<table border=\"2\" cellpadding=\"2\" cellspacing=\"2\" align=\"center\"><tr>");
    out << _T("<td><b>") << m_Defender << _T("</b></td><td>");
	out << m_Ress[0] << _T("</td><td>") << m_Ress[1] << _T("</td><td>") << m_Ress[2] << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[1] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF_def.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF_def.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnOhneTF_def.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[2] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF_def.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF_def.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitHalfTF_def.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("<tr><td>") << m_Bilanzstrings[3] << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF_def.met, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF_def.kris, tmp) << _T("</td>") << _T("\n");
	out << _T("<td align=right>") << AddPointsToNumber(m_Result.GewinnMitTF_def.deut, tmp) << _T("</td></tr>") << _T("\n");
	out << _T("</table>");

	out << _T("</td>");
	out << _T("</tr>") << _T("\n");
	out << _T("</table>") << _T("\n");
    out << _T("<b>Automatically generated by <a href=\"http://www.speedsim.net/eng\" target=\"_blank\">SpeedSim</a>");
    out << _T(" Kernel v") << KERNEL_VERSION << _T("</b>") << _T("\n");
    out << _T("</center>");
	out << _T("</body></html>") << _T("\n");
    string utf8_str;
#ifdef UNICODE
    utf8_str = wchar_to_utf8(out.str());
#else
    utf8_str = out.str();
#endif
    return utf8_str;
}

#ifdef CREATE_ADV_STATS
void CSpeedKernel::CreateAdvShipStats()
{
    CPngGraph graph(500, 500);
    int i;
    // ship graph
    for(i = 0; i < T_END; i++)
    {
        float xMin = 99999999, xMax = 0;
        size_t j;
        bool shipAvail = false;
        for(j = 0; j < m_NumSetShipsAtt.size(); j++)
        {
            if(m_NumSetShipsAtt[j].Type == i && m_NumSetShipsAtt[j].Num > 0) {
                shipAvail = true;
                break;
            }
        }
        if(!shipAvail)
            continue;
        // fill data
        float yMin = 99999999, yMax = 0;
        vector<sPoint> datapts;
        map<int, int> numships;
        map<int, int>::iterator it;
        size_t numcases = 0;
        for (j = 0; j < m_CombatResultsAtt.size(); j++)
        {
            int res = m_CombatResultsAtt[j][i];
            it = numships.find(res);
            if(it != numships.end())
                it->second++;
            else
                numships[res] = 1;
            numcases++;
            if(res > xMax) xMax = res;
            if(res < xMin) xMin = res;
        }
        if (numships.size() > 100)
        {
            int fac = (xMax - xMin) / 50;
            map<int, int> newships;
            // calculate down to 100
            it = numships.begin();
            for (; it != numships.end(); it++)
            {
                size_t n = it->first - it->first % fac;
                newships[n] += it->second;
            }
            numships = newships;
        }
        it = numships.begin();
        for(j = 0; j < numships.size(); j++)
        {
            if(!it->second)
                continue;
            sPoint pt;
            pt.x = it->first;
            if(pt.x > xMax) xMax = pt.x;
            if(pt.x < xMin) xMin = pt.x;

            pt.y = it->second * 100.f / numcases;
            if(pt.y > yMax) yMax = pt.y;
            if(pt.y < yMin) yMin = pt.y;

            datapts.push_back(pt);
            it++;
        }
        if(xMin == xMax)
        {
            xMin -= 5;
            xMax += 5;
        }
        if(yMin == yMax)
        {
            yMin -= 5;
            yMax += 5;
        }
        if(yMin < 0)
            yMin = 0;
        if(xMin < 0)
            xMin = 0;

        if(xMin == 99999999 && xMax == 0 || datapts.size() == 0)
            continue;
        graph.CreateAxes(xMin, xMax, yMin, yMax, int(abs(xMax - xMin) / 10.f), abs(yMax - yMin) / 10.f);
        graph.PlotData(datapts, PLOT_CIRCLE|PLOT_CONNECT_POINTS);
        char fname[50];
        sprintf(fname, "%s.png", m_IniFleetNames[i].c_str());
        graph.WriteToFile(fname);
    }

    // generate losses graph
}
#endif
