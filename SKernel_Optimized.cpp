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

#include "SpeedKernel.h"

// This file contains only for speed optimized functions with less features and (hopefully) more speed
#ifdef INCL_OPTIMIZED_FUNCTIONS

const BattleResult& CSpeedKernel::OptimizedSimulate(int count, const vector<SItem>* Attacker, const vector<SItem>* Defender, int* techAtt, int* techDef)
{
	int def_won = 0, att_won = 0, draw = 0;

	// init internal values, count ships etc.
    size_t i;
    
    float DamFak_a = (10 + techAtt[0]) / 10.0f;
    float ShFak_a = (10 + techAtt[1]) / 10.0f;
    float LifeFak_a = (10 + techAtt[2]) / 10.0f;
    
    float DamFak_v = (10 + techDef[0]) / 10.0f;
    float ShFak_v = (10 + techDef[1]) / 10.0f;
    float LifeFak_v = (10 + techDef[2]) / 10.0f;
    
    for(i = 0; i < T_END; i++)
    {
        MaxLifes[0][DEFFER][i] = MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][i];
        MaxShields[0][DEFFER][i] = MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][i];
        Dams[0][DEFFER][i] = Dams[MAX_PLAYERS_PER_TEAM][ATTER][i];
        
        MaxLifes[0][ATTER][i] = MaxLifes[MAX_PLAYERS_PER_TEAM][ATTER][i];
        MaxShields[0][ATTER][i] = MaxShields[MAX_PLAYERS_PER_TEAM][ATTER][i];
        Dams[0][ATTER][i] = Dams[MAX_PLAYERS_PER_TEAM][ATTER][i];
        
        
        MaxLifes[0][DEFFER][i] *= LifeFak_v;
        MaxShields[0][DEFFER][i] *= ShFak_v;
        Dams[0][DEFFER][i] *= DamFak_v;
        
        MaxLifes[0][ATTER][i] *= LifeFak_a;
        MaxShields[0][ATTER][i] *= ShFak_a;
        Dams[0][ATTER][i] *= DamFak_a;
    }
    
    Obj obj;
    size_t item = 0;

    vector<Obj> Att, Def, *CurrAtt, *CurrDef;
    CurrAtt = new vector<Obj>;
    CurrDef = new vector<Obj>;
    
    CurrAtt->reserve(200 * T_RAK);
    CurrDef->reserve(200 * T_END);

	for(item = 0; item < Attacker->size(); item++)
	{
        // create attacking objects; for every ship 1 object
		obj = FillObj((*Attacker)[item].Type, ATTER, 0);
        for(size_t o = 0; o < (*Attacker)[item].Num; o++)
			CurrAtt->push_back(obj);
	}

	for(item = 0; item < Defender->size(); item++)
	{
        // defending units
		obj = FillObj((*Defender)[item].Type, DEFFER, 0);
		for(size_t o = 0; o < (*Defender)[item].Num; o++)
			CurrDef->push_back(obj);
	}
    m_NumSetShipsAtt = *Attacker;
    m_NumSetShipsDef = *Defender;
    
    m_NumShipsAtt.clear();
    m_NumShipsDef.clear();

    m_NumShipsDef.resize(T_END);
    m_NumShipsAtt.resize(T_END);
    for(i = 0; i < T_END; i++) {
        m_NumShipsAtt[i].Type = (ITEM_TYPE)i;
        m_NumShipsDef[i].Type = (ITEM_TYPE)i;
        m_NumShipsAtt[i].Num = 0;
        m_NumShipsDef[i].Num = 0;
    }

	m_Result.NumRounds = 0;
	m_Result.TF = Res();
	m_Result.MaxTF = Res();
	m_Result.VerlusteAngreifer = Res();
	m_Result.VerlusteVerteidiger = Res();
    m_Result.VerlVertmitAufbau = Res();
	m_Result.MaxVerlAtt = Res();
	m_Result.MaxVerlDef = Res();
	m_Result.MinTF = Res();
	m_Result.MinVerlAtt = Res();
	m_Result.MinVerlDef = Res();
	m_Result.Beute = Res();
	m_Result.AttWon = 0;
	m_Result.DefWon = 0;
	m_Result.Draw = 0;
	m_Result.SpritVerbrauch = 0;
    m_Result.Ausbeute = 0;
	m_Result.WertAtt = Res();
	m_Result.WertDef = Res();
    if(count == 0)
		return m_Result;

	// backup set fleet
	Att = *CurrAtt;
	Def = *CurrDef;

	int num, round;

    for(num = 0; num < count; num++)
	{
        round = 1;
        *CurrAtt = Att;
		*CurrDef = Def;
		for(round = 1; round < 7; round++)
		{
            // maximize all shields
            DWORD id;
            for(i = 0; i < CurrAtt->size(); i++) {
                id = (*CurrAtt)[i].PlayerID;
                (*CurrAtt)[i].Shield = MaxShields[0][ATTER][(*CurrAtt)[i].Type];
            }
            
            for(i = 0; i < CurrDef->size(); i++) {
                id = (*CurrDef)[i].PlayerID;
                (*CurrDef)[i].Shield = MaxShields[0][DEFFER][(*CurrDef)[i].Type];
            }
            
            // let every ship shoot
            for(i = 0; i < CurrAtt->size(); i++)
            {
                OptimizedShipShoots((*CurrAtt)[i], ATTER);
			}

			for(i = 0; i < CurrDef->size(); i++)
			{
				OptimizedShipShoots((*CurrDef)[i], DEFFER);
			}

			// remove destroyed ships, calculate loss and debris
                
            vector<Obj>* tmpAtt = new vector<Obj>;
            vector<Obj>* tmpDef = new vector<Obj>;
            
            DWORD Num = 0;
            size_t i;
            for(i = 0; i < CurrAtt->size(); i++)
            {
                if((*CurrAtt)[i].Explodes == false)
                    Num++;
            }
            
            DWORD cnt = 0;
            
            tmpAtt->resize(Num);
            for(i = 0; i < CurrAtt->size(); i++)
            {
                if(!(*CurrAtt)[i].Explodes)
                    (*tmpAtt)[cnt++] = (*CurrAtt)[i];
            }
            
            Num = 0;
            cnt = 0;
            for(i = 0; i < CurrDef->size(); i++)
            {
                if((*CurrDef)[i].Explodes == false)
                    Num++;
            }
            
            tmpDef->resize(Num);
            for(i = 0; i < CurrDef->size(); i++)
            {
                if(!(*CurrDef)[i].Explodes)
                    (*tmpDef)[cnt++] = (*CurrDef)[i];
            }
            
            delete CurrAtt;
            delete CurrDef;
            CurrAtt = tmpAtt;
            CurrDef = tmpDef;


			// battle ends
			if(round == 6 || CurrAtt->size() == 0 || CurrDef->size() == 0)
			{
				// count left ships
				for(i = 0; i < CurrAtt->size(); i++)
				{
					ITEM_TYPE it = (*CurrAtt)[i].Type;
					m_NumShipsAtt[it].Num++;
				}

				for(i = 0; i < CurrDef->size(); i++)
				{
					ITEM_TYPE it = (*CurrDef)[i].Type;
					m_NumShipsDef[it].Num++;
				}
                
                // => attacker won
				if(CurrAtt->size() > 0 && CurrDef->size() == 0)
					m_Result.AttWon++;
				// => defender won
                else if(CurrDef->size() > 0 && CurrAtt->size() == 0)
                    m_Result.DefWon++;
                else
                    // => draw
                    m_Result.Draw++;

				break;
			}
		}
		m_Result.NumRounds += (round > 6 ? 6 : round);
	}
    delete CurrAtt;
    delete CurrDef;
	m_Result /= num;
    for(i = 0; i < m_NumShipsDef.size(); i++)
	{
        m_NumShipsAtt[i].Num /= num;
	}
	for(i = 0; i < m_NumShipsDef.size(); i++)
	{
		m_NumShipsDef[i].Num /= num;
	}
    OptimizedComputeLosses();
    return m_Result;
}

void CSpeedKernel::OptimizedShipShoots(Obj& o, int Team)
{
    bool ShootsAgain = true;
	ULONG Ziel = 0;
	int ZielTeam = Team == ATTER ? DEFFER : ATTER;
    const DWORD DefferID = 0, AtterID = 0;
	
	float Dam = Dams[AtterID][Team][o.Type];
	float Dam2 = Dam;	

	Obj* obj;
	
	vector<Obj>* treffer = (Team == ATTER ? m_DefObj : m_AttObj);

	unsigned int ListSize = treffer->size();
	if(ListSize == 0)
		return;

	// shoot until RF stops 
	while(ShootsAgain)
	{
		Dam = Dams[AtterID][Team][o.Type];
		Dam2 = Dam;
        {
            // get random target from enemy
            Ziel = RandomNumber(ListSize);
        }

        obj = &(*treffer)[Ziel];
        if(obj->Shield) {
            // new shield usage
            if(100 * Dam / MaxShields[DefferID][ZielTeam][obj->Type] < 1) {
                // if damage < 1% of shield, set damage to 0
                Dam = 0;
                Dam2 = Dam;
            }
        }
		if(obj->Shield <= 0 || Dam > 0)
		{
            // reduce shield by damage
			Dam -= obj->Shield;
			obj->Shield -= Dam2;
			if(Dam < 0)
				Dam = 0;
		}
		else
		{
			Dam = 0;
		}
		if(obj->Shield < 0)
			obj->Shield = 0;
		if(Dam > 0)
		{
            // if damage left, destroy hull
			obj->Life -= Dam;
			if(obj->Life < 0)
				obj->Life = 0;

			if(obj->Life <= 0.7f * MaxLifes[DefferID][ZielTeam][obj->Type])
			{
				// ship probably explodes, when hull damage >= 30 %
				if(rand() % 100 >= 100 * obj->Life / MaxLifes[DefferID][ZielTeam][obj->Type])
					obj->Explodes = true;
			}
		}
		// can shoot this at ship again?
		ShootsAgain = CanShootAgain_Optimized(o.Type, obj->Type);
	}
	return;

}

bool CSpeedKernel::CanShootAgain_Optimized(ITEM_TYPE AttType, ITEM_TYPE TargetType)
{
    // uses RF-v065a
    if(AttType == T_TS)
	{
		if(TargetType == T_TS || TargetType == T_PLASMA || TargetType == T_KS || TargetType == T_GS)
			return false;
        if(TargetType == T_RAK || TargetType == T_LL || TargetType == T_LJ)
            return RandomNumber(1000) >= 5; // RF (200)
        if(TargetType == T_SL || TargetType == T_IONEN || TargetType == T_SJ)
            return rand() % 100 >= 1;       // RF(100)
        if(TargetType == T_GAUSS)
            return rand() % 100 >= 2;       // RF(50)
        if(TargetType == T_KREUZER)
            return RandomNumber(10000) >= 303;  // RF(33)
        if(TargetType == T_SS)
            return RandomNumber(10000) >= 333;  // RF(30)
        if(TargetType == T_BOMBER)
            return rand() % 100 >= 4;       // RF(25)
        if(TargetType == T_ZER)
            return rand() % 100 >= 20;      // RF(5)
        // other ships
        return RandomNumber(1000) >= 4;     // RF(250)
	}
    if((TargetType == T_SPIO || TargetType == T_SAT) && AttType != T_SPIO && AttType < T_SHIPEND)
		return rand() % 100 >= 20;		    // RF(5)

	if(AttType == T_KREUZER)
	{
		if(TargetType == T_LJ)
			return rand() % 100 >= 33;	    // RF(3)
		
		if(TargetType == T_RAK)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	if(AttType == T_ZER)
	{
		if(TargetType == T_LL)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	if(AttType == T_BOMBER)
	{
		if(TargetType == T_LL || TargetType == T_RAK)
			return rand() % 100 >= 5;	    // RF(20)
		if(TargetType == T_SL || TargetType == T_IONEN)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	return false;
}

void CSpeedKernel::InitOptimized()
{
    ComputeShipData();
    InitRand();
    srand(time(NULL)); 
}

void CSpeedKernel::OptimizedComputeLosses()
{
	size_t i = 0;
	Res verl;
  
    for(i = 0; i < m_NumSetShipsAtt.size(); i++)
    {
        verl = Kosten[m_NumSetShipsAtt[i].Type] * (m_NumSetShipsAtt[i].Num - ceil(m_NumShipsAtt[i].Num - 0.5));
		m_Result.VerlusteAngreifer += verl;
        if(i < T_SHIPEND)
			m_Result.TF += verl / 10 * 3;
    }
    for(i = 0; i < m_NumSetShipsDef.size(); i++)
    {
        verl = Kosten[m_NumSetShipsDef[i].Type] * (m_NumSetShipsDef[i].Num - ceil(m_NumShipsDef[i].Num - 0.5));
		m_Result.VerlusteVerteidiger += verl;
        if(i < T_SHIPEND)
			m_Result.TF += verl / 10 * 3;
    }
}

#endif
