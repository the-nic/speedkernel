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

#include "SpeedKernel.h"

/*
	SKernel_Misc.cpp

	-General Functions-

	Function listing:

		Numbers and names of units:
			void ItemName(ITEM_TYPE t, TCHAR* out);
			genstring ItemName(ITEM_TYPE t);
            genstring ItemName2(ITEM_TYPE t);
			ITEM_TYPE GetItemType(genstring& name);
            genstring CRItemName(ITEM_TYPE t);


		Math functions:
            void InitRand();
			ULONG RandomNumber(ULONG Max);
            bool ItemCompare(const SItem& a, const SItem& b)


        Reading Functios:
            bool LoadLangFile(TCHAR *langfile);
            bool LoadRFFile(TCHAR *RFFile);
            genstring ReadStringFromIniFile(TCHAR *inifile, TCHAR *Section, TCHAR *KeyName);


		Misc:
			UINT GetDistance(PlaniPos& a, PlaniPos& b);
            DWORD ComputeFlyTime(const PlaniPos& b, const PlaniPos& e);

  			void ComputeShipData();
			void SetRemainingItemsInDef();

            void AddPointsToNumber(__int64& value, TCHAR* out);
*/

#ifndef WIN32
#undef ASMRAND
unsigned int _lrotl( unsigned int num, int shift ) {
	register unsigned int value = num;
	register unsigned int bit;

#if UINT_MAX == 0xFFFFFFFF
    // 32bit machine
	shift &= 0x1f;
    value = (value>>(0x20 - shift)) | (value << shift);
#elif UINT_MAX == 0xFFFFFFFFFFFFFFFF
    // not 32bit.. amd64??
	shift &= 0x3f;
    value = (value>>(0x40 - shift)) | (value << shift);
#else
#error Your machine is not supported!
#endif
	return value;
}
#endif


#ifdef ASMRAND
    DWORD randbuf[34];
    DWORD p1, p2;
#else
    DWORD randbuf[17][2];
    int p1, p2;
#endif

// compares 2 items
bool CSpeedKernel::ItemCompare(const SItem& a, const SItem& b)
{
    if(a.OwnerID < b.OwnerID)
        return true;
    if(a.OwnerID > b.OwnerID)
        return false;

    return a.Type < b.Type;
}

// copies name of unit into TCHAR-pointer
void CSpeedKernel::ItemName(ITEM_TYPE t, TCHAR* out)
{
    if(t >= T_KT && t <= T_END)
        _tcscpy(out, m_FleetNames[t].c_str());
    else
        _tcscpy(out, m_FleetNames[T_END + 1].c_str());//"Unknown"
}

void CSpeedKernel::ItemIniName(ITEM_TYPE Type, TCHAR* out) {
    if(Type >= T_KT && Type <= T_GS)
        _tcscpy(out, m_IniFleetNames[Type].c_str());
}

// returns name as genstring
genstring CSpeedKernel::ItemName(ITEM_TYPE t)
{
	if(t >= T_KT && t <= T_END)
        return m_FleetNames[t];
    else
        return m_FleetNames[T_END + 1]; //'Unknown'
}

genstring CSpeedKernel::ItemName2(ITEM_TYPE t) {
    if(t >= T_KT && t <= T_GS)
        return m_altFleetNames[t];
    else
        return m_FleetNames[T_END + 1]; //'Unknown'
}

genstring CSpeedKernel::CRItemName(ITEM_TYPE t) {
	if(t >= T_KT && t <= T_GS)
        return m_KBNames[t];
    else
        return m_FleetNames[T_END + 1]; //'Unknown'
}

// returns item type from name
ITEM_TYPE CSpeedKernel::GetItemType(genstring& name)
{
    for(int i = 0; i <= T_END; i++) {
        if(name == m_FleetNames[i] || name == m_altFleetNames[i] || name == m_KBNames[i] )
            return (ITEM_TYPE)i;
    }
	return T_NONE;
}

unsigned long CSpeedKernel::GetShipCapacity(ITEM_TYPE ship)
{
    if(ship > T_NONE && ship < T_SHIPEND)
        return LadeKaps[ship];
    else
        return 0;
}

// sets values for every unit (shield, life, hull etc.)
void CSpeedKernel::ComputeShipData()
{
    if(!m_ShipDataFromFile) {
        Kosten[T_KT] = Res(2000, 2000);
        Kosten[T_GT] = Res(6000, 6000);
        Kosten[T_LJ] = Res(3000, 1000);
        Kosten[T_SJ] = Res(6000, 4000);
        Kosten[T_KREUZER] = Res(20000, 7000, 2000);
        if(!m_UseOldBS)
            Kosten[T_SS] = Res(45000, 15000);
        else
            Kosten[T_SS] = Res(40000, 20000);
        Kosten[T_KOLO] = Res(10000, 20000, 10000);
        Kosten[T_REC] = Res(10000, 6000, 2000);
        Kosten[T_SPIO] = Res(0, 1000);
        Kosten[T_BOMBER] = Res(50000, 25000, 15000);
        Kosten[T_SAT] = Res(0, 2000, 500);
        Kosten[T_ZER] = Res(60000, 50000, 15000);
        Kosten[T_TS] = Res(5000000, 4000000, 1000000);
        Kosten[T_IC] = Res(30000, 40000, 15000);
        Kosten[T_RAK] = Res(2000, 0);
        Kosten[T_LL] = Res(1500, 500);
        Kosten[T_SL] = Res(6000, 2000);
        Kosten[T_GAUSS] = Res(20000, 15000, 2000);
        Kosten[T_IONEN] = Res(2000, 6000);
        Kosten[T_PLASMA] = Res(50000, 50000, 30000);
        Kosten[T_KS] = Res(10000, 10000);
        Kosten[T_GS] = Res(50000, 50000);

        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_KT] = 10;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_GT] = 25;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_LJ] = 10;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_SJ] = 25;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_KREUZER] = 50;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_SS] = 200;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_KOLO] = 100;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_REC] = 10;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_SPIO] = 0.01f;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_BOMBER] = 500;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_SAT] = 1;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_ZER] = 500;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_TS] = 50000;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_IC] = 400;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_RAK] = 20;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_LL] = 25;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_SL] = 100;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_GAUSS] = 200;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_IONEN] = 500;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_PLASMA] = 300;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_KS] = 2000;
        MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][T_GS] = 10000;

        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_KT] = 5;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_GT] = 5;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_LJ] = 50;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_SJ] = 150;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_KREUZER] = 400;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_SS] = 1000;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_KOLO] = 50;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_REC] = 1;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_SPIO] = 0.01f;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_BOMBER] = 1000;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_SAT] = 1;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_ZER] = 2000;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_TS] = 200000;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_IC] = 700;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_RAK] = 80;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_LL] = 100;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_SL] = 250;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_GAUSS] = 1100;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_IONEN] = 150;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_PLASMA] = 3000;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_KS] = 1;
        Dams[MAX_PLAYERS_PER_TEAM][ATTER][T_GS] = 1;
    }
    int i;
    for(i = 0; i < T_END; i++)
    {
        MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][i] = (Kosten[i].kris + Kosten[i].met) / 10.0f;
    }

    for(DWORD ID = 0; ID < MAX_PLAYERS_PER_TEAM; ID++)
    {
        double DamFak_a = (10 + m_TechsAtt[ID].Weapon) / 10.0f;
        double ShFak_a = (10 + m_TechsAtt[ID].Shield) / 10.0f;
        double LifeFak_a = (10 + m_TechsAtt[ID].Armour) / 10.0f;

        double DamFak_v = (10 + m_TechsDef[ID].Weapon) / 10.0f;
        double ShFak_v = (10 + m_TechsDef[ID].Shield) / 10.0f;
        double LifeFak_v = (10 + m_TechsDef[ID].Armour) / 10.0f;

        for(i = 0; i < T_END; i++)
        {
            MaxLifes[ID][DEFFER][i] = MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][i];
            MaxShields[ID][DEFFER][i] = MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][i];
            Dams[ID][DEFFER][i] = Dams[MAX_PLAYERS_PER_TEAM][ATTER][i];

            MaxLifes[ID][ATTER][i] = MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][i];
            MaxShields[ID][ATTER][i] = MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][i];
            Dams[ID][ATTER][i] = Dams[MAX_PLAYERS_PER_TEAM][ATTER][i];


            MaxLifes[ID][DEFFER][i] = floor(MaxLifes[ID][DEFFER][i] * LifeFak_v);
            MaxShields[ID][DEFFER][i] = floor(MaxShields[ID][DEFFER][i] * ShFak_v);
            Dams[ID][DEFFER][i] = floor(Dams[ID][DEFFER][i] * DamFak_v);

            MaxLifes[ID][ATTER][i] = floor(MaxLifes[ID][ATTER][i] * LifeFak_a);
            MaxShields[ID][ATTER][i] = floor(MaxShields[ID][ATTER][i] * ShFak_a);
            Dams[ID][ATTER][i] = floor(Dams[ID][ATTER][i] * DamFak_a);
        }
    }
    if(!m_ShipDataFromFile)
    {
        for(i = 0; i < T_END; i++)
	    {
		    LadeKaps[i] = 0;
		    Verbrauch[i] = 0;
            BaseSpeed[i] = 0;
	    }
        // ship capacity
        LadeKaps[T_KT] = 5000;
	    LadeKaps[T_GT] = 25000;
	    LadeKaps[T_LJ] = 50;
	    LadeKaps[T_SJ] = 100;
	    LadeKaps[T_KREUZER] = 800;
	    LadeKaps[T_SS] = 1500;
	    LadeKaps[T_KOLO] = 7500;
	    LadeKaps[T_REC] = 20000;
	    // changed since OGame 0.74a
        LadeKaps[T_SPIO] = 0;
	    LadeKaps[T_BOMBER] = 500;
	    LadeKaps[T_ZER] = 2000;
	    LadeKaps[T_TS] = 1000000;
        LadeKaps[T_IC] = 750;

        // consumption
        Verbrauch[T_KT] = 10;
        Verbrauch[T_GT] = 50;
        Verbrauch[T_LJ] = 20;
        Verbrauch[T_SJ] = 75;
        Verbrauch[T_KREUZER] = 300;
        Verbrauch[T_SS] = 500;
        Verbrauch[T_KOLO] = 1000;
        Verbrauch[T_REC] = 300;
        Verbrauch[T_SPIO] = 1;
        Verbrauch[T_BOMBER] = 1000;
        Verbrauch[T_ZER] = 1000;
        Verbrauch[T_TS] = 1;
        Verbrauch[T_IC] = 250;

        // speed
        BaseSpeed[T_KT] = 5000;
        BaseSpeed[T_GT] = 7500;
        BaseSpeed[T_LJ] = 12500;
        BaseSpeed[T_SJ] = 10000;
        BaseSpeed[T_KREUZER] = 15000;
        BaseSpeed[T_SS] = 10000;
        BaseSpeed[T_KOLO] = 2500;
        BaseSpeed[T_REC] = 2000;
        BaseSpeed[T_SPIO] = 100000000;
        BaseSpeed[T_BOMBER] = 4000;
        BaseSpeed[T_ZER] = 5000;
        BaseSpeed[T_TS] = 100;
        BaseSpeed[T_IC] = 10000;
    }

    Triebwerke[T_KT] = TW_VERBRENNUNG;
    Triebwerke[T_GT] = TW_VERBRENNUNG;
    Triebwerke[T_LJ] = TW_VERBRENNUNG;
    Triebwerke[T_SJ] = TW_IMPULS;
    Triebwerke[T_KREUZER] = TW_IMPULS;
    Triebwerke[T_SS] = TW_HYPERRAUM;
    Triebwerke[T_KOLO] = TW_IMPULS;
    Triebwerke[T_REC] = TW_VERBRENNUNG;
    Triebwerke[T_SPIO] = TW_VERBRENNUNG;
    Triebwerke[T_BOMBER] = TW_IMPULS;
    Triebwerke[T_ZER] = TW_HYPERRAUM;
    Triebwerke[T_TS] = TW_HYPERRAUM;
    Triebwerke[T_IC] = TW_HYPERRAUM;
}

// inits random number generator
void CSpeedKernel::InitRand() {
    int seed = time(NULL);
#ifdef ASMRAND
    __asm {
        mov eax, seed
        xor ecx, ecx
R80:
        imul eax, 2891336453
        inc eax
        mov randbuf[ecx*4], eax
        inc ecx
        cmp ecx, 34
        jb R80
        mov p1, 0         // initialize buffer pointers
        mov p2, 80
    }
#else
    int i, j;

    for(i = 0; i < 17; i++) {
        for(j = 0; j < 2; j++) {
            seed = seed * 2891336453UL + 1;
            randbuf[i][j] = seed;
        }
    }
    // initialize pointers to circular buffer
    p1 = 0;
    p2 = 10;
#endif
}

#pragma warning(disable: 4035)
ULONG CSpeedKernel::RandomNumber(ULONG Max)
{
#ifdef ASMRAND
    __asm {
        mov ebx, p1       // ring buffer pointers
        mov ecx, p2       // ring buffer pointer
        mov edx, randbuf[ebx]
        mov eax, randbuf[ebx]
        rol edx, 19       // rotate bits
        rol eax, 27
        add edx, randbuf[ecx]   // add two dwords
        add eax, randbuf[ecx+4]
        mov randbuf[ebx], eax   // save in swapped order
        mov randbuf[ebx+4], edx
        sub ebx, 8        // decrement p1
        jnc R30
        mov ebx, 128      // wrap around p1
R30:
        sub ecx, 8        // decrement p2
        jnc R40
        mov ecx, 128      // wrap around p2
R40:
        mov p1, ebx       // save updated pointers
        mov p2, ecx

        mov ebx, Max
        mov ecx, edx      // high bits of random number
        mul ebx           // multiply low 32 bits
        mov eax, ecx
        mov ecx, edx
        mul ebx           // multiply high 32 bits
        mov eax, edx
    }
#else
    DWORD y, z;
    // generate next number
    z = _lrotl(randbuf[p1][0], 19) + randbuf[p2][0];
    y = _lrotl(randbuf[p1][1], 27) + randbuf[p2][1];
    randbuf[p1][0] = y;
    randbuf[p1][1] = z;
    // rotate list pointers
    if (--p1 < 0)
        p1 = 16;
    if (--p2 < 0)
        p2 = 16;
    return y % Max;
#endif
}
#pragma warning(default: 4035)

int CSpeedKernel::GetDistance(const PlaniPos& b, const PlaniPos& e)
{
	if(b.Gala != e.Gala)
		return abs(int(b.Gala - e.Gala) * 20000);

	if(b.Sys != e.Sys)
		return 2700 + 5 * abs(int(b.Sys-e.Sys) * 19);

	if(b.Pos != e.Pos)
		return 1000 + abs(int(b.Pos - e.Pos) * 5);

	return 0;
}

DWORD CSpeedKernel::ComputeFlyTime(const PlaniPos& b, const PlaniPos& e, const vector<SItem>& vFleet, int Speed, int EngineTechs[])
{
    if(!Speed || !EngineTechs)
        return 0;
    if(b.Gala != e.Gala)
        return 10 + (3500.0f / Speed * sqrt((abs((int)(b.Gala-e.Gala)) * 20000000.0f) / GetFleetSpeed(vFleet, EngineTechs)));

    if(b.Sys != e.Sys)
        return 10 + (3500.0f / Speed * sqrt((2700000.0f + abs((int)(b.Sys-e.Sys)) * 95000.0f) / GetFleetSpeed(vFleet, EngineTechs)));

    if(b.Pos != e.Pos)
        return 10 + (3500.0f / Speed * sqrt((1000000.0f + abs((int)(b.Pos-e.Pos)) * 5000.0f) / GetFleetSpeed(vFleet, EngineTechs)));
}

DWORD CSpeedKernel::ComputeFlyTime(const PlaniPos& b, const PlaniPos& e, int FleetID, const vector<SItem>& vFleet /* = m_NumSetShipsAtt */)
{
    if(!m_Speed[FleetID])
            return 0;
    if(b.Gala != e.Gala)
        return 10 + (3500.0f / m_Speed[FleetID] * sqrt((abs((int)(b.Gala-e.Gala)) * 20000000.0f) / GetFleetSpeed(FleetID, vFleet)));

    if(b.Sys != e.Sys)
        return 10 + (3500.0f / m_Speed[FleetID] * sqrt((2700000.0f + abs((int)(b.Sys-e.Sys)) * 95000.0f) / GetFleetSpeed(FleetID, vFleet)));

    if(b.Pos != e.Pos)
        return 10 + (3500.0f / m_Speed[FleetID] * sqrt((1000000.0f + abs((int)(b.Pos-e.Pos)) * 5000.0f) / GetFleetSpeed(FleetID, vFleet)));
    return 0;
}

int CSpeedKernel::GetShipSpeed(const ITEM_TYPE Ship, const int EngineTechs[])
{
    int basesp = BaseSpeed[Ship];
    int engine = Triebwerke[Ship];
    if(Ship == T_KT)
    {
        if(EngineTechs[engine + 1] >= 5)
        {
            basesp *= 2;
            engine += 1;
        }
    }
    else if(Ship == T_BOMBER)
    {   
        if(EngineTechs[engine + 1] >= 8)
        {
            basesp = 5000;
            engine += 1;
        }
    }
    return basesp * (1 + (float)EngineTechs[engine] * (engine + 1) / 10.0f);
}

int CSpeedKernel::GetShipSpeed(ITEM_TYPE Ship, int FleetID)
{
    return GetShipSpeed(Ship, m_TechsTW[FleetID]);
}

int CSpeedKernel::GetFleetSpeed(const vector<SItem>& vFleet, const int EngineTechs[])
{
    int min = INT_MAX;
    for(size_t i = 0; i < vFleet.size(); i++) {
        if(vFleet[i].Num == 0 || vFleet[i].Type == T_NONE)
            continue;
        int speed = GetShipSpeed(vFleet[i].Type, EngineTechs);
        if(speed < min)
            min = speed;
    }
    return min;
}

int CSpeedKernel::GetFleetSpeed(int FleetID, const vector<SItem>& vFleet)
{
    return GetFleetSpeed(vFleet, m_TechsTW[FleetID]);
}

void CSpeedKernel::SetRemainingItemsInDef(REBUILD_OPTION opt)
{
	vector<SItem> items;
	int i;
    size_t j;
    
    double rem_units = 0;

	items.resize(T_END);
	for(i = 0; i < T_END; i++)
	{
        rem_units = 0;
        // count remaining ships based on the option
        switch(opt)
        {
        case REBUILD_BESTCASE:
            rem_units = m_BestCaseDef[i];
            break;
        case REBUILD_WORSTCASE:
            rem_units = m_WorstCaseDef[i];
            break;
        case REBUILD_AVG:
            for (j = 0; j < m_NumShipsDef.size(); j++)
            {
                if(m_NumShipsDef[j].Type == (ITEM_TYPE)i)
                    rem_units += m_NumShipsDef[j].Num;
            }
            break;
        default:
            return;
        }

		items[i].Type = (ITEM_TYPE)i;
        items[i].OwnerID = 0;
        items[i].Num = 0;
        if(i < T_SHIPEND)
            items[i].Num = rem_units;
        else
        {
            double NumUnits = 0;
            // get the number of set def units
            for(j = 0; j < m_NumSetShipsDef.size(); j++) 
            {
                if(m_NumSetShipsDef[j].Type == (ITEM_TYPE)i)
                    NumUnits += m_NumSetShipsDef[j].Num;
            }
            if(m_RebuildSmallDef && NumUnits <= 10)
                items[i].Num = NumUnits;
            else
            {
                items[i].Num = floor(rem_units + m_DefRebuildFac * (NumUnits - rem_units) + 0.5);
            }
         }
	}

	SetFleet(NULL, &items);
	m_Result.RessDa -= m_Result.Beute;

    m_Waves.TotalPlunder = m_Result.GesamtBeute;
	m_Waves.NumAtts = m_Result.NumAtts;
    m_Waves.TotalRecs = m_Result.GesamtRecs;
    m_Waves.TotalDebris = m_Result.GesTF;
    m_Waves.TotalLosses = m_Result.GesVerlust;
    m_Waves.TotalFuel = m_Result.TotalFuel;
}

TCHAR* CSpeedKernel::AddPointsToNumber(__int64 value, TCHAR* out)
{
    int i;
    PR_PROF_FUNC(F_ADDPTONUMBER);
    genstrstream dat;

    bool neg = false;
#if _MSC_VER < 1400 && defined(_MSC_VER)
    TCHAR tmp[512];
    _stprintf(tmp, _T("%I64d"), value);
    dat << tmp;
#else
    dat << value;
#endif

    genstring c = dat.str();

    if(value < 0) {
        c.erase(0, 1);
        neg = true;
    }

    int l = _tcslen(c.c_str());
    int quot = (int)l / 3;
    int rem = l % 3;
    if(rem == 0)
        quot--;
    for(i = 1; i <= quot; i++)
    {
        c.insert(l - i * 4 + 1, m_ThousandSep);
        l++;
    }
    if(neg)
        c.insert(0, _T("-"));
    _tcscpy(out, c.c_str());

    return out;
}

void CSpeedKernel::GetFleetWorth(Res &att, Res &def) {
    att = def = Res(0, 0, 0);
    size_t j;
    for(j = 0; j < m_NumSetShipsAtt.size(); j++) {
        att += Kosten[m_NumSetShipsAtt[j].Type] * m_NumSetShipsAtt[j].Num;
    }
    for(j = 0; j < m_NumSetShipsDef.size(); j++) {
        def += Kosten[m_NumSetShipsDef[j].Type] * m_NumSetShipsDef[j].Num;
    }
    return;
}

Res CSpeedKernel::GetFleetWorth(vector<SItem> *Fleet) {
    Res worth;
    size_t j;
    for(j = 0; j < Fleet->size(); j++) {
        worth += Kosten[(*Fleet)[j].Type] * (*Fleet)[j].Num;
    }
    return worth;
}

bool CSpeedKernel::LoadLangFile(const char *langfile) {
    if(!langfile) {
        // init german strings
        m_FleetNames[T_KT] = _T("Kleiner Transporter");
        m_FleetNames[T_GT] = _T("Gro\xDF")_T("er Transporter");
        m_FleetNames[T_LJ] = _T("Leichter J\xE4ger");
        m_FleetNames[T_SJ] = _T("Schwerer J\xE4ger");
        m_FleetNames[T_KREUZER] = _T("Kreuzer");
        m_FleetNames[T_SS] = _T("Schlachtschiff");
        m_FleetNames[T_SPIO] = _T("Spionagesonde");
        m_FleetNames[T_KOLO] = _T("Kolonieschiff");
        m_FleetNames[T_REC] = _T("Recycler");
        m_FleetNames[T_BOMBER] = _T("Bomber");
        m_FleetNames[T_SAT] = _T("Solarsatellit");
        m_FleetNames[T_ZER] = _T("Zerst\xF6rer");
        m_FleetNames[T_TS] = _T("Todesstern");
        m_FleetNames[T_IC] = _T("Schlachtkreuzer");
        m_FleetNames[T_RAK] = _T("Raketenwerfer");
        m_FleetNames[T_LL] = _T("Leichtes Lasergesch\xFCtz");
        m_FleetNames[T_SL] = _T("Schweres Lasergesch\xFCtz");
        m_FleetNames[T_IONEN] = _T("Ionengesch\xFCtz");
        m_FleetNames[T_GAUSS] = _T("Gau\xDF")_T("kanone");
        m_FleetNames[T_PLASMA] = _T("Plasmawerfer");
        m_FleetNames[T_KS] = _T("Kleine Schildkuppel");
        m_FleetNames[T_GS] = _T("Gro\xDF")_T("e Schildkuppel");
        m_FleetNames[T_END] = _T("Abfangrakete");
        m_FleetNames[T_END + 1] = _T("Unbekannt");
        m_altFleetNames[T_GT] = _T("Grosser Transporter");
        m_altFleetNames[T_LJ] = _T("Leichter Jaeger");
        m_altFleetNames[T_SJ] = _T("Schwerer Jaeger");
        m_altFleetNames[T_SAT] = _T("Solar Satellit");
        m_altFleetNames[T_ZER] = _T("Zerstoerer");
        m_altFleetNames[T_LL] = _T("Leichtes Lasergeschuetz");
        m_altFleetNames[T_SL] = _T("Schweres Lasergeschuetz");
        m_altFleetNames[T_IONEN] = _T("Ionengeschuetz");
        m_altFleetNames[T_GAUSS] = _T("Gausskanone");
        m_altFleetNames[T_GS] = _T("Grosse Schildkuppel");
        m_KBNames[T_KT] = _T("Kl.Trans");
        m_KBNames[T_GT] = _T("Gr.Trans");
        m_KBNames[T_LJ] = _T("L.J\xE4ger");
        m_KBNames[T_SJ] = _T("S.J\xE4ger");
        m_KBNames[T_KREUZER] = _T("Kreuzer");
        m_KBNames[T_SS] = _T("Schlachts.");
        m_KBNames[T_REC] = _T("Recy.");
        m_KBNames[T_KOLO] = _T("Kol. Schiff.");
        m_KBNames[T_SPIO] = _T("Spio.Sonde");
        m_KBNames[T_BOMBER] = _T("Bomber");
        m_KBNames[T_SAT] = _T("Sol. Sat");
        m_KBNames[T_ZER] = _T("Zerst.");
        m_KBNames[T_TS] = _T("Rip");
        m_KBNames[T_IC] = _T("Schlachtkr.");
        m_KBNames[T_RAK] = _T("Rak.");
        m_KBNames[T_LL] = _T("L.Laser");
        m_KBNames[T_SL] = _T("S.Laser");
        m_KBNames[T_IONEN] = _T("Ion.W");
        m_KBNames[T_GAUSS] = _T("Gau\xDF");
        m_KBNames[T_PLASMA] = _T("Plasma");
        m_KBNames[T_KS] = _T("S.Kuppel");
        m_KBNames[T_GS] = _T("GS.Kuppel");
        m_KBTitle = _T("Kampfbericht - SpeedSim v");

        m_TechNames[0] = _T("Waffentechnik");
        m_TechNames[1] = _T("Schildtechnik");
        m_TechNames[2] = _T("Raumschiffpanzerung");
        m_TechNames[3] = _T("Verbrennungstriebwerk");
        m_TechNames[4] = _T("Impulstriebwerk");
        m_TechNames[5] = _T("Hyperraumantrieb");
        m_Ress[0] = _T("Metall");
        m_Ress[1] = _T("Kristall");
        m_Ress[2] = _T("Deuterium");
        m_wrongRess[0] = _T("Metallmine");
        m_wrongRess[1] = _T("Kristallmine");
        m_wrongRess[2] = _T("Deuteriumsynthetisierer");
        m_wrongRess2[0] = _T("Metallspeicher");
        m_wrongRess2[1] = _T("Kristallspeicher");
        m_wrongRess2[2] = _T("Deuteriumtank");
        m_Spiostrings[0] = _T("auf ");
        m_Spiostrings[1] = _T("] (");
        m_Spiostrings[3] = _T("Mondbasis");
        m_EspLimiter = _T("Spionageabwehr:");
        m_EspTitles[0] = _T("Flotten");
        m_EspTitles[1] = _T("Verteidigung");
        m_EspTitles[2] = _T("Forschung");
        m_Ovr[0] = _T("dem Planeten ");
        m_Ovr[1] = _T("] Die Flotte");
        m_TechPreString = _T("Stufe");
        m_Attacker = _T("Angreifer");
        m_Defender = _T("Verteidiger");

        m_KBTable[0] = _T("Typ");
        m_KBTable[1] = _T("Anz.");
        m_KBTable[2] = _T("Bewaff.");
        m_KBTable[3] = _T("Schilde");
        m_KBTable[4] = _T("H&uuml;lle");

        m_KBTechs[0] = _T("Waffen");
        m_KBTechs[1] = _T("Schilde");
        m_KBTechs[2] = _T("H\xFClle");
        m_KBRoundStr[0] = _T("Die angreifende Flotte schie&szlig;t insgesamt %d mal mit Gesamtst&auml;rke %.0f auf den Verteidiger");
        m_KBRoundStr[1] = _T("Die Schilde des Verteidigers absorbieren %.0f Schadenspunkte");
        m_KBRoundStr[2] = _T("Die verteidigende Flotte schie&szlig;t insgesamt %d mal mit Gesamtst&auml;rke %.0f auf den Angreifer");
        m_KBRoundStr[3] = _T("Die Schilde des Angreifers absorbieren %.0f Schadenspunkte");
        m_KBResult[0] = _T("Der Angreifer hat die Schlacht gewonnen!");
        m_KBResult[1] = _T("Der Verteidiger hat die Schlacht gewonnen!");
        m_KBResult[2] = _T("Die Schlacht endet unentschieden. Beide Flotten ziehen sich auf ihre Heimatplaneten zur&uuml;ck.");
        m_KBResult[3] = _T("Es kann kein Kampfergebnis ausgegeben werden, weil der Kampf noch gar nicht simuliert worden ist.");
        m_KBResult[4] = _T("Das Ergebnis des Kampfes ist nicht exakt vorrauszusehen.");
        m_KBResult[5] = _T("Er erbeutet %d Metall, %d Kristall und %d Deuterium");
        m_KBResult[6] = _T("Der Angreifer gewinnt mit %.0f%% Wahrscheinlichkeit, der Verteidiger gewinnt zu %.0f%% Wahrscheinlichkeit.");
        m_KBResult[7] = _T("Der Kampf geht zu %.0f%% Unentschieden aus.");
        m_KBResult[8] = _T("Vernichtet");
        m_KBLoss[0] = _T("Der Angreifer hat insgesamt %s Units + %s Deuterium verloren.");
        m_KBLoss[1] = _T("Der Verteidiger hat insgesamt %s Units + %s Deuterium verloren.");
        m_KBLoss[2] = _T("Auf diesen Raumkoordinaten liegen nun %s Metall und %s Kristall");
        m_KBMoon = _T("Die Chance einer Mondentstehung betr&auml;gt %d%%");
        // western european charset
#ifdef UNICODE
        m_HTML_Charset = _T("utf-8");
#else
        m_HTML_Charset = _T("ISO-8859-1");
#endif // UNICODE

        m_BWTable[0] = _T("Minimal");
        m_BWTable[1] = _T("Durchschnittlich");
        m_BWTable[2] = _T("Maximal");
        m_BWTable[3] = _T("Summe");
        m_BWTable[4] = _T("Tr&uuml;mmerfeld");
        m_BWTable[5] = _T("Schiff");
        m_BWTable[6] = _T("Nach Wiederaufbau");
        m_BWTable[7] = _T("Best");
        m_BWTable[8] = _T("Worst");

        m_BWLoss[0] = _T("Maximale und Minimale Verluste");
        m_BWLoss[1] = _T("Wert der angreifenden Flotte:");
        m_BWLoss[2] = _T("Wert der verteidigenden Flotte:");
        m_BWLoss[3] = _T("Units");

        m_BWTitle[0] = _T("Best/Worst Case Berechnung");
        m_BWTitle[1] = _T("Best/Worst Case f&uuml;r Angreifer");
        m_BWTitle[2] = _T("Best/Worst Case f&uuml;r Verteidiger");

		m_Bilanzstrings[0] = _T("Bilanz f&uuml;r Angreifer und Verteidiger nach Kampf");
		m_Bilanzstrings[1] = _T("Ohne Einsammeln des TF");
		m_Bilanzstrings[2] = _T("Bei Einsammeln des halben TF");
		m_Bilanzstrings[3] = _T("Bei Einsammeln des gesamten TF");
        m_BracketNames = false;
        SetParseOrder();
        return true;
    }
    CIniFile iniFile(langfile);

    // normal ship names
    iniFile.GetStr(m_FleetNames[T_KT], _T("Fleet"), _T("KT"));
    iniFile.GetStr(m_FleetNames[T_GT], _T("Fleet"), _T("GT"));
    iniFile.GetStr(m_FleetNames[T_LJ], _T("Fleet"), _T("LJ"));
    iniFile.GetStr(m_FleetNames[T_SJ], _T("Fleet"), _T("SJ"));
    iniFile.GetStr(m_FleetNames[T_KREUZER], _T("Fleet"), _T("KR"));
    iniFile.GetStr(m_FleetNames[T_SS], _T("Fleet"), _T("SS"));
    iniFile.GetStr(m_FleetNames[T_SPIO], _T("Fleet"), _T("SPIO"));
    iniFile.GetStr(m_FleetNames[T_KOLO], _T("Fleet"), _T("KOL"));
    iniFile.GetStr(m_FleetNames[T_REC], _T("Fleet"), _T("REC"));
    iniFile.GetStr(m_FleetNames[T_BOMBER], _T("Fleet"), _T("BOM"));
    iniFile.GetStr(m_FleetNames[T_SAT], _T("Fleet"), _T("SAT"));
    iniFile.GetStr(m_FleetNames[T_ZER], _T("Fleet"), _T("ZER"));
    iniFile.GetStr(m_FleetNames[T_TS], _T("Fleet"), _T("TS"));
    iniFile.GetStr(m_FleetNames[T_IC], _T("Fleet"), _T("IC"));
    iniFile.GetStr(m_FleetNames[T_RAK], _T("Fleet"), _T("RAK"));
    iniFile.GetStr(m_FleetNames[T_LL], _T("Fleet"), _T("LL"));
    iniFile.GetStr(m_FleetNames[T_SL], _T("Fleet"), _T("SL"));
    iniFile.GetStr(m_FleetNames[T_IONEN], _T("Fleet"), _T("IO"));
    iniFile.GetStr(m_FleetNames[T_GAUSS], _T("Fleet"), _T("GAUSS"));
    iniFile.GetStr(m_FleetNames[T_PLASMA], _T("Fleet"), _T("PLAS"));
    iniFile.GetStr(m_FleetNames[T_KS], _T("Fleet"), _T("KS"));
    iniFile.GetStr(m_FleetNames[T_GS], _T("Fleet"), _T("GS"));
    iniFile.GetStr(m_FleetNames[T_END], _T("Fleet"), _T("ABM"));
    iniFile.GetStr(m_FleetNames[T_END + 1], _T("Fleet"), _T("UNB"));

    // alternative names
    iniFile.GetStr(m_altFleetNames[T_KT], _T("AltFleet"), _T("KT2"));
    iniFile.GetStr(m_altFleetNames[T_GT], _T("AltFleet"), _T("GT2"));
    iniFile.GetStr(m_altFleetNames[T_LJ], _T("AltFleet"), _T("LJ2"));
    iniFile.GetStr(m_altFleetNames[T_SJ], _T("AltFleet"), _T("SJ2"));
    iniFile.GetStr(m_altFleetNames[T_KREUZER], _T("AltFleet"), _T("KR2"));
    iniFile.GetStr(m_altFleetNames[T_SS], _T("AltFleet"), _T("SS2"));
    iniFile.GetStr(m_altFleetNames[T_SPIO], _T("AltFleet"), _T("SPIO2"));
    iniFile.GetStr(m_altFleetNames[T_KOLO], _T("AltFleet"), _T("KOL2"));
    iniFile.GetStr(m_altFleetNames[T_REC], _T("AltFleet"), _T("REC2"));
    iniFile.GetStr(m_altFleetNames[T_BOMBER], _T("AltFleet"), _T("BOM2"));
    iniFile.GetStr(m_altFleetNames[T_SAT], _T("AltFleet"), _T("SAT2"));
    iniFile.GetStr(m_altFleetNames[T_ZER], _T("AltFleet"), _T("ZER2"));
    iniFile.GetStr(m_altFleetNames[T_TS], _T("AltFleet"), _T("TS2"));
    iniFile.GetStr(m_altFleetNames[T_IC], _T("AltFleet"), _T("IC2"));
    iniFile.GetStr(m_altFleetNames[T_RAK], _T("AltFleet"), _T("RAK2"));
    iniFile.GetStr(m_altFleetNames[T_LL], _T("AltFleet"), _T("LL2"));
    iniFile.GetStr(m_altFleetNames[T_SL], _T("AltFleet"), _T("SL2"));
    iniFile.GetStr(m_altFleetNames[T_IONEN], _T("AltFleet"), _T("IO2"));
    iniFile.GetStr(m_altFleetNames[T_GAUSS], _T("AltFleet"), _T("GAUSS2"));
    iniFile.GetStr(m_altFleetNames[T_PLASMA], _T("AltFleet"), _T("PLAS2"));
    iniFile.GetStr(m_altFleetNames[T_KS], _T("AltFleet"), _T("KS2"));
    iniFile.GetStr(m_altFleetNames[T_GS], _T("AltFleet"), _T("GS2"));
    iniFile.GetStr(m_altFleetNames[T_END], _T("Fleet"), _T("ABM2"));

    // cr names
    iniFile.GetStr(m_KBNames[T_KT], _T("KBNames"), _T("KB_KT"));
    iniFile.GetStr(m_KBNames[T_GT], _T("KBNames"), _T("KB_GT"));
    iniFile.GetStr(m_KBNames[T_LJ], _T("KBNames"), _T("KB_LJ"));
    iniFile.GetStr(m_KBNames[T_SJ], _T("KBNames"), _T("KB_SJ"));
    iniFile.GetStr(m_KBNames[T_KREUZER], _T("KBNames"), _T("KB_KR"));
    iniFile.GetStr(m_KBNames[T_SS], _T("KBNames"), _T("KB_SS"));
    iniFile.GetStr(m_KBNames[T_SPIO], _T("KBNames"), _T("KB_SPIO"));
    iniFile.GetStr(m_KBNames[T_KOLO], _T("KBNames"), _T("KB_KOL"));
    iniFile.GetStr(m_KBNames[T_REC], _T("KBNames"), _T("KB_REC"));
    iniFile.GetStr(m_KBNames[T_BOMBER], _T("KBNames"), _T("KB_BOM"));
    iniFile.GetStr(m_KBNames[T_SAT], _T("KBNames"), _T("KB_SAT"));
    iniFile.GetStr(m_KBNames[T_ZER], _T("KBNames"), _T("KB_ZER"));
    iniFile.GetStr(m_KBNames[T_TS], _T("KBNames"), _T("KB_TS"));
    iniFile.GetStr(m_KBNames[T_IC], _T("KBNames"), _T("KB_IC"));
    iniFile.GetStr(m_KBNames[T_RAK], _T("KBNames"), _T("KB_RAK"));
    iniFile.GetStr(m_KBNames[T_LL], _T("KBNames"), _T("KB_LL"));
    iniFile.GetStr(m_KBNames[T_SL], _T("KBNames"), _T("KB_SL"));
    iniFile.GetStr(m_KBNames[T_IONEN], _T("KBNames"), _T("KB_IO"));
    iniFile.GetStr(m_KBNames[T_GAUSS], _T("KBNames"), _T("KB_GAUSS"));
    iniFile.GetStr(m_KBNames[T_PLASMA], _T("KBNames"), _T("KB_PLAS"));
    iniFile.GetStr(m_KBNames[T_KS], _T("KBNames"), _T("KB_KS"));
    iniFile.GetStr(m_KBNames[T_GS], _T("KBNames"), _T("KB_GS"));

    // ress-stuff
    iniFile.GetStr(m_Ress[0], _T("Ress"), _T("MET"));
    iniFile.GetStr(m_Ress[1], _T("Ress"), _T("KRIS"));
    iniFile.GetStr(m_Ress[2], _T("Ress"), _T("DEUT"));
    iniFile.GetStr(m_wrongRess[0], _T("Ress"), _T("MET_M"));
    iniFile.GetStr(m_wrongRess[1], _T("Ress"), _T("KRIS_M"));
    iniFile.GetStr(m_wrongRess[2], _T("Ress"), _T("DEUT_M"));
    iniFile.GetStr(m_wrongRess2[0], _T("Ress"), _T("MET_S"));
    iniFile.GetStr(m_wrongRess2[1], _T("Ress"), _T("KRIS_S"));
    iniFile.GetStr(m_wrongRess2[2], _T("Ress"), _T("DEUT_S"));

    // Techs
    iniFile.GetStr(m_TechNames[0], _T("Techs"), _T("WAFF"));
    iniFile.GetStr(m_TechNames[1], _T("Techs"), _T("SCHILD"));
    iniFile.GetStr(m_TechNames[2], _T("Techs"), _T("PANZ"));

    // esp reading
    iniFile.GetStr(m_Spiostrings[0], _T("ReadStrings"), _T("SPIO1"));
    iniFile.GetStr(m_Spiostrings[1], _T("ReadStrings"), _T("SPIO2"));
    // stop string
    iniFile.GetStr(m_EspLimiter, _T("ReadStrings"), _T("ESP_END"));
    // titles
    iniFile.GetStr(m_EspTitles[0], _T("ReadStrings"), _T("ESP_FLEETS"));
    iniFile.GetStr(m_EspTitles[1], _T("ReadStrings"), _T("ESP_DEF"));
    iniFile.GetStr(m_EspTitles[2], _T("ReadStrings"), _T("ESP_RESEARCH"));
    // lunar base
    iniFile.GetStr(m_Spiostrings[3], _T("ReadStrings"), _T("LUNARBASE"));

    // overview
    iniFile.GetStr(m_Ovr[0], _T("ReadStrings"), _T("OVR1"));
    iniFile.GetStr(m_Ovr[1], _T("ReadStrings"), _T("OVR2"));
    iniFile.GetStr(m_Defense, _T("ReadStrings"), _T("DEFSTR"));

    // Defense/Techs additional strings
    iniFile.GetStr(m_TechPreString, _T("ReadStrings"), _T("TECHSTR"));
    iniFile.GetStr(m_TechNames[3], _T("ReadStrings"), _T("ENGINE_COMB"));
    iniFile.GetStr(m_TechNames[4], _T("ReadStrings"), _T("ENGINE_IMP"));
    iniFile.GetStr(m_TechNames[5], _T("ReadStrings"), _T("ENGINE_HYP"));

    iniFile.GetStr(m_Attacker, _T("General"), _T("ATT"));
    iniFile.GetStr(m_Defender, _T("General"), _T("DEF"));

    // combat report stuff
    iniFile.GetStr(m_KBTable[0], _T("KBStuff"), _T("TYP"));
    iniFile.GetStr(m_KBTable[1], _T("KBStuff"), _T("NUM"));
    iniFile.GetStr(m_KBTable[2], _T("KBStuff"), _T("WEAP"));
    iniFile.GetStr(m_KBTable[3], _T("KBStuff"), _T("SHIELD"));
    iniFile.GetStr(m_KBTable[4], _T("KBStuff"), _T("HULL"));
    iniFile.GetStr(m_KBTechs[0], _T("KBStuff"), _T("WEAP2"));
    iniFile.GetStr(m_KBTechs[1], _T("KBStuff"), _T("SHIELD2"));
    iniFile.GetStr(m_KBTechs[2], _T("KBStuff"), _T("HULL2"));
    iniFile.GetStr(m_KBRoundStr[0], _T("KBStuff"), _T("ATT_SHOOTS"));
    iniFile.GetStr(m_KBRoundStr[1], _T("KBStuff"), _T("DEF_ABSORBS"));
    iniFile.GetStr(m_KBRoundStr[2], _T("KBStuff"), _T("DEF_SHOOTS"));
    iniFile.GetStr(m_KBRoundStr[3], _T("KBStuff"), _T("ATT_ABSORBS"));
    iniFile.GetStr(m_KBResult[0], _T("KBStuff"), _T("ATT_WON"));
    iniFile.GetStr(m_KBResult[1], _T("KBStuff"), _T("DEF_WON"));
    iniFile.GetStr(m_KBResult[2], _T("KBStuff"), _T("DRAW"));
    iniFile.GetStr(m_KBResult[3], _T("KBStuff"), _T("NOSIM"));
    iniFile.GetStr(m_KBResult[4], _T("KBStuff"), _T("NO_EXACT_RES"));
    iniFile.GetStr(m_KBResult[5], _T("KBStuff"), _T("ATT_GET"));
    iniFile.GetStr(m_KBResult[6], _T("KBStuff"), _T("LIKELIHOOD"));
    iniFile.GetStr(m_KBResult[7], _T("KBStuff"), _T("LLH_DRAW"));
    iniFile.GetStr(m_KBResult[8], _T("KBStuff"), _T("DESTROYED"));
    iniFile.GetStr(m_KBLoss[0], _T("KBStuff"), _T("ATT_LOSS"));
    iniFile.GetStr(m_KBLoss[1], _T("KBStuff"), _T("DEF_LOSS"));
    iniFile.GetStr(m_KBLoss[2], _T("KBStuff"), _T("KB_TF"));
    iniFile.GetStr(m_KBMoon, _T("KBStuff"), _T("MOON"));
    iniFile.GetStr(m_KBTitle, _T("KBStuff"), _T("KBTITLE"));
#ifndef UNICODE
    iniFile.GetStr(m_HTML_Charset, _T("KBStuff"), _T("CHARSET"));
    if(m_HTML_Charset == _T(""))
        m_HTML_Charset = _T("ISO-8859-1");
#endif

    // BW-Case
    iniFile.GetStr(m_BWTable[0], _T("BWCFile"), _T("MIN"));
    iniFile.GetStr(m_BWTable[1], _T("BWCFile"), _T("AVRG"));
    iniFile.GetStr(m_BWTable[2], _T("BWCFile"), _T("MAX"));
    iniFile.GetStr(m_BWTable[3], _T("BWCFile"), _T("SUM"));
    iniFile.GetStr(m_BWTable[4], _T("BWCFile"), _T("BW_TF"));
    iniFile.GetStr(m_BWTable[5], _T("BWCFile"), _T("SHIP"));
    iniFile.GetStr(m_BWTable[6], _T("BWCFile"), _T("AFTERREBUILD"));
    iniFile.GetStr(m_BWTable[7], _T("BWCFile"), _T("BESTCASE"));
    iniFile.GetStr(m_BWTable[8], _T("BWCFile"), _T("WORSTCASE"));

    iniFile.GetStr(m_BWLoss[0], _T("BWCFile"), _T("MAXMIN_LOSS"));
    iniFile.GetStr(m_BWLoss[1], _T("BWCFile"), _T("WORTH_ATT"));
    iniFile.GetStr(m_BWLoss[2], _T("BWCFile"), _T("WORTH_DEF"));
    iniFile.GetStr(m_BWLoss[3], _T("BWCFile"), _T("UNITS"));

    iniFile.GetStr(m_BWTitle[0], _T("BWCFile"), _T("BW_TITLE"));
    iniFile.GetStr(m_BWTitle[1], _T("BWCFile"), _T("BW_TITLEATT"));
    iniFile.GetStr(m_BWTitle[2], _T("BWCFile"), _T("BW_TITLEDEF"));

    // balance
    iniFile.GetStr(m_Bilanzstrings[0], _T("BAL"), _T("TITLE2"));
    iniFile.GetStr(m_Bilanzstrings[1], _T("BAL"), _T("T1"));
    iniFile.GetStr(m_Bilanzstrings[2], _T("BAL"), _T("T2"));
    iniFile.GetStr(m_Bilanzstrings[3], _T("BAL"), _T("T3"));

    long i = 0;
    iniFile.GetLong(i, _T("ReadStrings"), _T("BRACKET_NAMES"));
    if(i)
        m_BracketNames = true;
    
    SetParseOrder();
    return true;
}

bool CSpeedKernel::LoadRFFile(char *RFFile)
{
    genstring s;
    int i, j;
    CIniFile iniFile(RFFile);

    for(i = 0; i < T_END; i++)
    {
        for(j = 0; j < T_END; j++)
            m_RF[i][j] = 10000;
    }
    for(i = 0; i < T_END; i++)
    {
        for(j = 0; j < T_END; j++)
        {
            long n;
            if(iniFile.GetLong(n, m_IniFleetNames[i], m_IniFleetNames[j]))
                m_RF[i][j] = 9999 - RFPERC(n);
        }
    }
    for(i = 0; i < T_END; i++)
    {
        for(j = 0; j < T_END; j++)
        {
            if(m_RF[i][j] > 10000 || m_RF[i][j] <= 0)
                m_RF[i][j] = 10000;
        }
    }

    return true;
}

bool CSpeedKernel::LoadShipData(char *SDFile) {
    // load shield, damage, cost (life)
    genstring s;
    long i, num;
    if(!SDFile)
        return false;
    CIniFile iniFile(SDFile);
    
    for(i = 0; i < T_END; i++) {
        // shield
        if(iniFile.GetLong(num, _T("Shield"), m_IniFleetNames[i]))
            MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][i] = num;
        // damage
        if(iniFile.GetLong(num, _T("Damage"), m_IniFleetNames[i]))
            Dams[MAX_PLAYERS_PER_TEAM][ATTER][i] = num;
        // cost
        if(iniFile.GetStr(s, _T("Cost"), m_IniFleetNames[i]))
            Kosten[i] = StringToRes(s);
        if(i < T_SHIPEND)
        {
            // deuterium consumption
            if(iniFile.GetLong(num, _T("Consumption"), m_IniFleetNames[i]))
                Verbrauch[i] = num;
            // capacity
            if(iniFile.GetLong(num, _T("Capacity"), m_IniFleetNames[i]))
                LadeKaps[i] = num;
            // base speed
            if(iniFile.GetLong(num, _T("BaseSpeed"), m_IniFleetNames[i]))
                BaseSpeed[i] = num;
        }
    }
    
    return true;
}

Res CSpeedKernel::StringToRes(const genstring &val) {
    Res res;
    genstring num;
    genstring::size_type f = 0, f2 = 0;

    f = val.find(_T(","));
    num = val.substr(0);
    res.met = _ttoi(num.c_str());
    f2 = val.find(_T(","), f+1) + 1;
    num = val.substr(f+1);
    res.kris = _ttoi(num.c_str());
    f = f2;
    f2 = val.length();
    num = val.substr(f);
    res.deut = _ttoi(num.c_str());
    return res;
}

void CSpeedKernel::FillRFTable(RFTYPE rfType)
{
    int i;
    for(i = 0; i < T_END; i++)
    {
        for(int j = 0; j < T_END; j++)
            m_RF[i][j] = 10000;
    }

    switch(rfType)
    {
    case RF_065:
        {
            for(i = 0; i < T_SHIPEND; i++)
            {
                m_RF[i][T_SPIO] = 2000;
                m_RF[i][T_SAT] = 2000;
            }
            m_RF[T_SPIO][T_SPIO] = 10000;
            m_RF[T_SPIO][T_SAT] = 10000;
            m_RF[T_SAT][T_SAT] = 10000;

            m_RF[T_KREUZER][T_LJ] = 3333;
            m_RF[T_KREUZER][T_RAK] = 1000;

            m_RF[T_ZER][T_LL] = 1000;

            m_RF[T_BOMBER][T_LL] = 500;
            m_RF[T_BOMBER][T_RAK] = 500;
            m_RF[T_BOMBER][T_SL] = 1000;
            m_RF[T_BOMBER][T_IONEN] = 1000;

            m_RF[T_TS][T_SPIO] = 8;
            m_RF[T_TS][T_SAT] = 8;
            m_RF[T_TS][T_RAK] = 50;
            m_RF[T_TS][T_LL] = 50;
            m_RF[T_TS][T_LJ] = 50;
            m_RF[T_TS][T_SS] = 333;
            m_RF[T_TS][T_SL] = 100;
            m_RF[T_TS][T_IONEN] = 100;
            m_RF[T_TS][T_SJ] = 100;
            m_RF[T_TS][T_GAUSS] = 200;
            m_RF[T_TS][T_KREUZER] = 303;
            m_RF[T_TS][T_BOMBER] = 400;
            m_RF[T_TS][T_ZER] = 2000;
            m_RF[T_TS][T_KT] = 40;
            m_RF[T_TS][T_GT] = 40;
            m_RF[T_TS][T_REC] = 40;
            m_RF[T_TS][T_KOLO] = 40;

        }
        break;
    case RF_075:
        {
            for(i = 0; i < T_SHIPEND; i++)
            {
                m_RF[i][T_SPIO] = 2000;
                m_RF[i][T_SAT] = 2000;
            }
            m_RF[T_SPIO][T_SPIO] = 10000;
            m_RF[T_SPIO][T_SAT] = 10000;
            m_RF[T_SAT][T_SAT] = 10000;
            m_RF[T_SAT][T_SPIO] = 10000;

            m_RF[T_SJ][T_KT] = 3333;

            m_RF[T_KREUZER][T_LJ] = 1667;
            m_RF[T_KREUZER][T_RAK] = 1000;

            m_RF[T_ZER][T_LL] = 1000;
            m_RF[T_ZER][T_IC] = 5000;

            m_RF[T_BOMBER][T_LL] = 500;
            m_RF[T_BOMBER][T_RAK] = 500;
            m_RF[T_BOMBER][T_SL] = 1000;
            m_RF[T_BOMBER][T_IONEN] = 1000;

            m_RF[T_TS][T_SPIO] = 8;
            m_RF[T_TS][T_SAT] = 8;
            m_RF[T_TS][T_RAK] = 50;
            m_RF[T_TS][T_LL] = 50;
            m_RF[T_TS][T_LJ] = 50;
            m_RF[T_TS][T_SS] = 333;
            m_RF[T_TS][T_SL] = 100;
            m_RF[T_TS][T_IONEN] = 100;
            m_RF[T_TS][T_SJ] = 100;
            m_RF[T_TS][T_GAUSS] = 200;
            m_RF[T_TS][T_KREUZER] = 303;
            m_RF[T_TS][T_BOMBER] = 400;
            m_RF[T_TS][T_ZER] = 2000;
            m_RF[T_TS][T_KT] = 40;
            m_RF[T_TS][T_GT] = 40;
            m_RF[T_TS][T_REC] = 40;
            m_RF[T_TS][T_KOLO] = 40;
            m_RF[T_TS][T_IC] = 667;

            m_RF[T_IC][T_KT] = 3333;
            m_RF[T_IC][T_GT] = 3333;
            m_RF[T_IC][T_SJ] = 2500;
            m_RF[T_IC][T_KREUZER] = 2500;
            m_RF[T_IC][T_SS] = 1429;
        }
        break;
    }
}

void CSpeedKernel::SetParseOrder()
{
    vector<ShipString> tmp_list[2];
    int i = 0;

    // get all lengths of ship names
    for(; i < T_END+1; i++)
    {
        ShipString s;
        s.Item = (ITEM_TYPE)i;
        s.Length = m_FleetNames[i].length();
        tmp_list[0].push_back(s);
        s.Length = m_altFleetNames[i].length();
        tmp_list[1].push_back(s);
    }
    // sort them descending
    sort(tmp_list[0].begin(), tmp_list[0].end(), ShipString::IsLess);
    sort(tmp_list[1].begin(), tmp_list[1].end(), ShipString::IsLess);
    m_ParseOrder[0].resize(T_END+1);
    m_ParseOrder[1].resize(T_END+1);
    for(i = 0; i < T_END+1; i++)
    {
        m_ParseOrder[0][i] = tmp_list[0][i].Item;
        m_ParseOrder[1][i] = tmp_list[1][i].Item;
    }
}
