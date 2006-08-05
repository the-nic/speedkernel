/*
SpeedSim - a OGame (www.ogame.org) combat simulator
Copyright (C) 2004, 2005 Maximialian Matthé & Nicolas Höft

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
	SKernel_Sim.cpp

		-Function for battle simulation

		Simulation:
		    void AbortSim();
            bool InitSim();
            bool Simulate(int count);
        
        Ships during simulation:
            void ShipShoots(Obj& o, int Team, DWORD AtterID);
            bool CanShootAgain_User(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            bool CanShootAgain_V058(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            bool CanShootAgain_V059(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            bool CanShootAgain_V062(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            bool CanShootAgain_V065(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            bool CantShootAgain(ITEM_TYPE AttType, ITEM_TYPE ZielType);
            
            void DestroyExplodedShips();
            void ShipsDontExplode();
            void MaxAllShields();
        
        Fleet / Technologies:
            bool SetFleet(vector<SItem>* Attacker, vector<SItem>* Defender);
            void SetTechs(int* att, int* def, DWORD PlayerID);
            void GetTechs(int* att, int* def, DWORD PlayerID);            
        
        Functions for battle result:
            void ComputeCRArrays();
            void SaveShipsToCR(int round);
            void UpdateBestWorstCase()

        Misc:            
            Obj FillObj(ITEM_TYPE Type, int Team, DWORD PlayerID);
*/

// sets a new fleet
bool CSpeedKernel::SetFleet(vector<SItem>* Attacker, vector<SItem>* Defender)
{
	Obj obj;
	if(Attacker)
		m_AttObj->clear();
	if(Defender)
		m_DefObj->clear();
	
	srand((unsigned)time(NULL));

	// reduce shield domes to 1
	if(Defender)
	{
		for(size_t i = 0; i < Defender->size(); i++)
		{
			if((*Defender)[i].Type == T_KS && (*Defender)[i].Num > 1)
				(*Defender)[i].Num = 1;
			if((*Defender)[i].Type == T_GS && (*Defender)[i].Num > 1)
				(*Defender)[i].Num = 1;
		}
	}

    int StartPos=0, EndPos=999;
    int OwnersToSet[MAX_PLAYERS_PER_TEAM], tmp = 0;
    unsigned int NumOwnersToSet = 0;
    size_t i = 0, j = 0;
    if(Attacker) {
//	    CheckVector(m_NumSetShipsAtt);
//	    CheckVector(*Attacker);
    }
    if(Attacker && Attacker->size())
    {
        // check, which OwnerIDs should be set new
        for(i = 0; i < MAX_PLAYERS_PER_TEAM; i++)
            OwnersToSet[i] = -1;
        for(i = 0; i < Attacker->size(); i++)
        {
            tmp = (*Attacker)[i].OwnerID;
            for(j = 0; j < NumOwnersToSet; j++)
                if(OwnersToSet[j] == tmp)
                    break;
            if(j != NumOwnersToSet && NumOwnersToSet != 0)
                break;

            // owner is not in list of new owners
            OwnersToSet[NumOwnersToSet++] = tmp;
        }

        // Remove all fleets of owners to be reset
		SItem t;
		for(i = 0; i < m_NumSetShipsAtt.size(); i++)
        {
            for(j = 0; j < NumOwnersToSet; j++)
            {
                t = m_NumSetShipsAtt[i];
				if(m_NumSetShipsAtt[i].OwnerID == OwnersToSet[j])
                {
                    m_NumSetShipsAtt.erase(m_NumSetShipsAtt.begin() + i);
                    i--;
                    break;
                }
            }
        }
        // delete 'non-ships'
        for(i = 0; i < Attacker->size(); i++) {
            vector<SItem>::iterator it = Attacker->begin() + i;
            if(!it->Num || it->Type == T_NONE) {
                Attacker->erase(it);
                i--;
            }
        }
//		CheckVector(m_NumSetShipsAtt);
        // Insert new fleets into fleet vector
        m_NumSetShipsAtt.insert(m_NumSetShipsAtt.end(), Attacker->begin(), Attacker->end());
        sort(m_NumSetShipsAtt.begin(), m_NumSetShipsAtt.end(), ItemCompare);
//      CheckVector(m_NumSetShipsAtt);
    }

    // repeat algorithm for defender
    NumOwnersToSet = 0, tmp = 0;
    i = 0; j = 0;
    if(Defender && Defender->size())
    {
        for(i = 0; i < MAX_PLAYERS_PER_TEAM; i++)
            OwnersToSet[i] = -1;
        for(i = 0; i < Defender->size(); i++)
        {
            tmp = (*Defender)[i].OwnerID;
            for(j = 0; j < NumOwnersToSet; j++)
                if(OwnersToSet[j] == tmp)
                    break;
            if(j != NumOwnersToSet && NumOwnersToSet != 0)
                break;

            OwnersToSet[NumOwnersToSet++] = tmp;
        }
        for(i = 0; i < m_NumSetShipsDef.size(); i++)
        {
            for(j = 0; j < NumOwnersToSet; j++)
            {
                if(m_NumSetShipsDef[i].OwnerID == OwnersToSet[j])
                {
                    m_NumSetShipsDef.erase(m_NumSetShipsDef.begin() + i);
                    i--;
                    break;
                }
            }
        }
        for(i = 0; i < Defender->size(); i++) {
            vector<SItem>::iterator it = Defender->begin() + i;
            if(!it->Num || it->Type == T_NONE) {
                Defender->erase(it);
                i--;
            }
        }
        m_NumSetShipsDef.insert(m_NumSetShipsDef.end(), Defender->begin(), Defender->end());
        sort(m_NumSetShipsDef.begin(), m_NumSetShipsDef.end(), ItemCompare);
    }
	return true;
}

void CSpeedKernel::GetTechs(ShipTechs* att, ShipTechs* def, DWORD PlayerID)
{
    if(att) {
        memcpy(att, &m_TechsAtt[PlayerID], sizeof(ShipTechs));
    }		
	if(def)
		memcpy(def, &m_TechsDef[PlayerID], sizeof(ShipTechs));
}

void CSpeedKernel::SetTechs(ShipTechs* att, ShipTechs* def, DWORD PlayerID)
{
	if(att)
		m_TechsAtt[PlayerID] = *att;
	if(def)
		m_TechsDef[PlayerID] = *def;
}

// creates an object with the correct values
Obj CSpeedKernel::FillObj(ITEM_TYPE Type, int Team, DWORD PlayerID)
{
	Obj o;
	o.Type = Type;
	
	o.Life = MaxLifes[PlayerID][Team][Type];
	o.Shield = MaxShields[PlayerID][Team][Type];
	o.Explodes = false;
    o.PlayerID = PlayerID;
	return o;
}

/// simulate the battle
bool CSpeedKernel::Simulate(int count)
{
    m_InitSim = false;

	m_DataIsDeleted = false;
	m_SimulateFreedItsData = false;

	if(count == 0)
		return false;

    int def_won = 0, att_won = 0, draw = 0;

	// init internal values, count ships etc.
	InitSim();
#ifdef CREATE_ADV_STATS
    m_DebrisFields.resize(count);
    m_LossAtt.resize(count);
    m_LossDef.resize(count);
    m_CombatResultsAtt.resize(count);
    m_CombatResultsDef.resize(count);
    for(int i = 0; i < count; i++)
    {
        m_CombatResultsAtt[i].resize(T_END);
        m_CombatResultsDef[i].resize(T_END);
    }
    
#endif

	vector<Obj> Att, Def;
	// backup set fleet
	// possibly copy into file for less memory usage when using _very_ big fleets?
    Att = (*m_AttObj);
	Def = (*m_DefObj);

	bool aborted = false;

    m_InitSim = true;
	int num, round;
//	CheckVector(m_NumShipsAtt);

    for(num = 0; num < count; num++)
	{
        (*m_AttObj) = Att;
		(*m_DefObj) = Def;
		for(round = 1; round < 7; round++)
		{
			if(m_FuncPtr)
				m_FuncPtr(num + 1, round);

			if(m_DataIsDeleted)
			{
				aborted = true;
				break;
			}
			// Save number of ships for combat report
			SaveShipsToCR(round);

			m_CurrentRound = round - 1;

			if(m_DataIsDeleted)
			{
				aborted = true;
				break;
			}
			// maximize all shields
			MaxAllShields();
			if(m_DataIsDeleted)
			{
				aborted = true;
				break;
			}
			// make every ship shoot
			size_t i;
            for(i = 0; i < m_AttObj->size(); i++)
			{
				ShipShoots((*m_AttObj)[i], ATTER, (*m_AttObj)[i].PlayerID);
			}

			if(m_DataIsDeleted)
			{
				aborted = true;
				break;
			}
			for(i = 0; i < m_DefObj->size(); i++)
			{
				ShipShoots((*m_DefObj)[i], DEFFER, (*m_DefObj)[i].PlayerID);
			}

			if(m_DataIsDeleted)
			{
				aborted = true;
				break;
			}
			// remove destroyed ships, calculate loss and debris
			DestroyExplodedShips();

			// battle ends
			if(round == 6 || m_AttObj->size() == 0 || m_DefObj->size() == 0)
			{
				SaveShipsToCR(round+1);

				// count left ships
				if(m_DataIsDeleted)
				{
					aborted = true;
					break;
				}
				for(i = 0; i < m_AttObj->size(); i++)
				{
					ITEM_TYPE it = (*m_AttObj)[i].Type;
					DWORD Index = ((*m_AttObj)[i].PlayerID*T_RAK)+it;
					m_NumShipsAtt[Index].Num++;
				}

				if(m_DataIsDeleted)
				{
					aborted = true;
					break;
				}	

				for(i = 0; i < m_DefObj->size(); i++)
				{
					ITEM_TYPE it = (*m_DefObj)[i].Type;
					DWORD Index = ((*m_DefObj)[i].PlayerID*(T_END))+it;
					m_NumShipsDef[Index].Num++;
				}

				if(m_DataIsDeleted)
				{
					aborted = true;
					break;
				}
                
                // => attacker won
				if(m_AttObj->size() > 0 && m_DefObj->size() == 0)
					m_Result.AttWon++;
				// => defender won
                else if(m_DefObj->size() > 0 && m_AttObj->size() == 0)
                    m_Result.DefWon++;
                else
                    // => draw
                    m_Result.Draw++;

				// recalculate best/worst case (check for better/worse case)
				if(m_DataIsDeleted)
				{
					aborted = true;
					break;
				}
				UpdateBestWorstCase(num);
				
				break;
			}
		}
		m_Result.NumRounds += (round > 6 ? 6 : round);
		if(aborted)
			break;
	}
    Att.clear();
    Def.clear();
    m_SimulateFreedItsData = true;
    
    if(!num && aborted)
        return false;
    else if (aborted)
    {
#ifdef CREATE_ADV_STATS
        // trim unused data
        m_DebrisFields.resize(num);
        m_LossAtt.resize(num);
        m_LossDef.resize(num);
        m_CombatResultsAtt.resize(num);
        m_CombatResultsDef.resize(num);
#endif
    }

	// battle result is added permanently added during multiple simulation
    // -> calculate average result by dividing through number of simulations
	m_Result /= num;
    size_t t;
    for(t = 0; t < m_NumShipsAtt.size(); t++)
	{
        m_NumShipsAtt[t].Num /= num;
	}
	for(t = 0; t < m_NumShipsDef.size(); t++)
	{
		m_NumShipsDef[t].Num /= num;
	}
	
	// now get the average number of ships in every round for combat reports
	ComputeCRArrays();

    // calculation of combat report
	ComputeBattleResult();
#ifdef PROFILING
    ofstream aus("prof.txt");
    CProfiler::GetInst().GetResults(aus);
#endif
	return true;
}

void CSpeedKernel::MaxAllShields()
{
	PR_PROF_FUNC(F_MASHIELDS);
    // this function sets all shields to its maximum
	size_t i;
    DWORD id;
    for(i = 0; i < m_AttObj->size(); i++) {
        id = (*m_AttObj)[i].PlayerID;
		(*m_AttObj)[i].Shield = MaxShields[id][ATTER][(*m_AttObj)[i].Type];
    }

    for(i = 0; i < m_DefObj->size(); i++) {
        id = (*m_DefObj)[i].PlayerID;
		(*m_DefObj)[i].Shield = MaxShields[id][DEFFER][(*m_DefObj)[i].Type];
    }
}

void CSpeedKernel::ShipsDontExplode()
{
    PR_BEG_FUNC(F_SDEXPLODE);
	// set all explosion flags to 'false'
    size_t i;
    for(i = 0; i < m_AttObj->size(); i++)
		(*m_AttObj)[i].Explodes = false;

	for(i = 0; i < m_DefObj->size(); i++)
		(*m_DefObj)[i].Explodes = false;
}

// removes destroyed ships from the arrays
void CSpeedKernel::DestroyExplodedShips()
{
	PR_PROF_FUNC(F_DESTEXPLSHIPS);
    
    vector<Obj>* tmpAtt = new vector<Obj>;
	vector<Obj>* tmpDef = new vector<Obj>;

	DWORD Num = 0;
	size_t i;
    for(i = 0; i < m_AttObj->size(); i++)
	{
		if((*m_AttObj)[i].Explodes == false)
			Num++;
	}

	DWORD count = 0;

	tmpAtt->resize(Num);
	for(i = 0; i < m_AttObj->size(); i++)
	{
		if(!(*m_AttObj)[i].Explodes)
			(*tmpAtt)[count++] = (*m_AttObj)[i];
	}

	Num = 0;
	count = 0;
	for(i = 0; i < m_DefObj->size(); i++)
	{
        if((*m_DefObj)[i].Explodes == false)
			Num++;
	}

	tmpDef->resize(Num);
	for(i = 0; i < m_DefObj->size(); i++)
	{
		if(!(*m_DefObj)[i].Explodes)
			(*tmpDef)[count++] = (*m_DefObj)[i];
	}

	delete m_AttObj;
	delete m_DefObj;
	m_AttObj = tmpAtt;
	m_DefObj = tmpDef;
}

// lets shoot a ship
void CSpeedKernel::ShipShoots(Obj& o, int Team, DWORD AtterID)
{
	PR_BEG_FUNC(F_SSHOOTS);
    bool ShootsAgain = true;
	ULONG Ziel = 0;
	int ZielTeam = Team == ATTER ? DEFFER : ATTER;
    DWORD DefferID = 0;
	
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
        DefferID = 0;
        {
            //PR_PROF_FUNC(F_RANDNR);
            // get random target from enemy
            Ziel = RandomNumber(ListSize);
            DefferID = (*treffer)[Ziel].PlayerID;
        }

		if(Team == ATTER)
		{
			m_NumShotsPerRoundAtt[m_CurrentRound]++;
			m_ShotStrengthAtt[m_CurrentRound] += Dam;
		}
		else
		{
			m_NumShotsPerRoundDef[m_CurrentRound]++;
			m_ShotStrengthDef[m_CurrentRound] += Dam;
		}

        obj = &(*treffer)[Ziel];
        if(obj->Shield) {
            if(100.f * Dam / MaxShields[DefferID][ZielTeam][obj->Type] < 1) {
                // if damage < 1% of shield, set damage to 0
                Dam = 0;
                Dam2 = Dam;
            }
            else if(!m_NewShield)
            {
				// round damage
                // old shield calculation
                Dam = MaxShields[DefferID][ZielTeam][obj->Type] * floor(100.0f * Dam / MaxShields[DefferID][ZielTeam][obj->Type]);
		        Dam /= 100.0f;
		        Dam2 = Dam;
            }
            else
            {
                Dam = floor(Dam * 10.f) / 10.f;
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
		float absorbed = Dams[AtterID][Team][o.Type] - Dam;
		// absorbed damage (for combat reports)
        if(Team == ATTER)
			m_AbsorbedDef[m_CurrentRound] += absorbed;
		else
			m_AbsorbedAtt[m_CurrentRound] += absorbed;
        
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
				if(rand() % 100 >= 100.f * obj->Life / MaxLifes[DefferID][ZielTeam][obj->Type])
					obj->Explodes = true;
			}
		}
		// can shoot this at ship again?
		ShootsAgain = CanShootAgain(o.Type, obj->Type);
        //ShootsAgain = CanShootAgain_V059(o.Type, obj->Type);
	}
	return;
}


// lets shoot a ship again with a certain chance
bool CSpeedKernel::CanShootAgain_V059(ITEM_TYPE AttType, ITEM_TYPE ZielType)
{

    PR_PROF_FUNC(F_CSA_059);

	if(AttType == T_TS)
	{
		if(ZielType == T_SPIO || ZielType == T_SAT)
            // 99,92% RF against probes RF(1250)
            return CSpeedKernel::GetInstance().RandomNumber(10000) >= 8;
        if(ZielType == T_ZER)
			return rand() % 10 >= 2;	// 90% RF gegen Zerstörer RF(5)
		if(ZielType == T_TS || ZielType == T_PLASMA || ZielType == T_KS || ZielType == T_GS)
			return false;
        if(ZielType < T_SHIPEND)
			// Gegen Flotten
            // RF(33) (96,97%)
            return CSpeedKernel::GetInstance().RandomNumber(10000) >= 302;

		if(ZielType >= T_SHIPEND)
			// RF(250) (99,6%) gegen def
            return CSpeedKernel::GetInstance().RandomNumber(1000) >= 4;
            
	}
		
	if(ZielType == T_SPIO && AttType != T_SPIO)
		return rand() % 100 >= 10;		// Alle Einheiten (außer Spionagesonde) 80% Rf gegen Spionagesonden
	
	if(ZielType == T_SAT)
		return rand() % 100 >= 20;		// Alle Einheiten 80% Rf gegen Sats

	if(AttType == T_KREUZER)
	{
		if(ZielType == T_LJ)
			return rand() % 100 >= 33;	// 67% RF gegen Leichte Jäger
		
		if(ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% RF gegen Raks

		return false;
	}

	if(AttType == T_ZER)
	{
		if(ZielType == T_LL)
			return rand() % 100 >= 10;	// 90% RF gegen Leichte Laser

		return false;
	}

	if(AttType == T_BOMBER)
	{
		if(ZielType == T_LL || ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% Rf gegen Raketenwerfer und Leichte Laser
		if(ZielType == T_SL || ZielType == T_IONEN)
			return rand() % 100 >= 20;	// 90% Rf gegen IOnengeschütze und Schwere Laser

		return false;
	}

	return false;
}

bool CSpeedKernel::CanShootAgain_V058(ITEM_TYPE AttType, ITEM_TYPE ZielType)
{
	PR_PROF_FUNC(F_CSA_058);
    
    if(AttType == T_TS)
	{
		if(ZielType == T_TS || ZielType == T_PLASMA || ZielType == T_KS || ZielType == T_GS)
			return false;
		
		if(ZielType == T_ZER)
			return rand() % 100 >= 10;		// 90% RF gegen Zerris
        //return rand() % 10000 >= 16;
        return CSpeedKernel::GetInstance().RandomNumber(10000) >= 20;
	}
		
	if(ZielType == T_SPIO && AttType != T_SPIO)
		return rand() % 100 >= 10;		// Alle Einheiten (außer Spionagesonde) 80% Rf gegen Spionagesonden
	
	if(ZielType == T_SAT)
		return rand() % 100 >= 20;		// Alle Einheiten 80% Rf gegen Sats

	if(AttType == T_KREUZER)
	{
		if(ZielType == T_LJ)
			return rand() % 100 >= 33;	// 67% RF gegen Leichte Jäger
		
		if(ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% RF gegen Raks

		return false;
	}

	if(AttType == T_ZER)
	{
		if(ZielType == T_LL)
			return rand() % 100 >= 10;	// 90% RF gegen Leichte Laser

		return false;
	}

	if(AttType == T_BOMBER)
	{
		if(ZielType == T_LL || ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% Rf gegen Raketenwerfer und Leichte Laser
		if(ZielType == T_SL || ZielType == T_IONEN)
			return rand() % 100 >= 20;	// 90% Rf gegen IOnengeschütze und Schwere Laser

		return false;
	}

	return false;
}

bool CSpeedKernel::CanShootAgain_V062(ITEM_TYPE AttType, ITEM_TYPE ZielType)
{
    PR_PROF_FUNC(F_CSA_062);

	if(AttType == T_TS)
	{
        // aufgrund des Bugs RF halbieren
		if(ZielType == T_TS || ZielType == T_PLASMA || ZielType == T_KS || ZielType == T_GS)
			return false;
		
		if(ZielType == T_ZER)
			return rand() % 100 >= 10;
        return CSpeedKernel::GetInstance().RandomNumber(10000) >= 40;
	}
		
	if(ZielType == T_SPIO && AttType != T_SPIO)
		return rand() % 100 >= 10;		// Alle Einheiten (außer Spionagesonde) 80% Rf gegen Spionagesonden
	
	if(ZielType == T_SAT)
		return rand() % 100 >= 20;		// Alle Einheiten 80% Rf gegen Sats

	if(AttType == T_KREUZER)
	{
		if(ZielType == T_LJ)
			return rand() % 100 >= 33;	// 67% RF gegen Leichte Jäger
		
		if(ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% RF gegen Raks

		return false;
	}

	if(AttType == T_ZER)
	{
		if(ZielType == T_LL)
			return rand() % 100 >= 10;	// 90% RF gegen Leichte Laser

		return false;
	}

	if(AttType == T_BOMBER)
	{
		if(ZielType == T_LL || ZielType == T_RAK)
			return rand() % 100 >= 10;	// 90% Rf gegen Raketenwerfer und Leichte Laser
		if(ZielType == T_SL || ZielType == T_IONEN)
			return rand() % 100 >= 20;	// 90% Rf gegen Ionengeschütze und Schwere Laser

		return false;
	}

	return false;
}

bool CSpeedKernel::CantShootAgain(ITEM_TYPE AttType, ITEM_TYPE ZielType) {
    return false;
}

bool CSpeedKernel::CanShootAgain_User(ITEM_TYPE AttType, ITEM_TYPE ZielType) {
    CSpeedKernel &inst = CSpeedKernel::GetInstance();
    return inst.RandomNumber(10000) < inst.m_RF[AttType][ZielType];
}

// newest values
bool CSpeedKernel::CanShootAgain_V065(ITEM_TYPE AttType, ITEM_TYPE ZielType)
{
    if(AttType == T_TS)
	{
        // CHECK!
        if(ZielType == T_SPIO || ZielType == T_SAT)
            return CSpeedKernel::GetInstance().RandomNumber(10000) >= 8; // RF(1250)
        if(ZielType == T_RAK || ZielType == T_LL || ZielType == T_LJ)
            return CSpeedKernel::GetInstance().RandomNumber(1000) >= 5; //RF (200)
        if(ZielType == T_SS)
            return CSpeedKernel::GetInstance().RandomNumber(10000) >= 333;  // RF(30)
		if(ZielType == T_TS || ZielType == T_PLASMA || ZielType == T_KS || ZielType == T_GS)
			return false;        
        if(ZielType == T_SL || ZielType == T_IONEN || ZielType == T_SJ)
            return rand() % 100 >= 1;       // RF(100)
        if(ZielType == T_GAUSS)
            return rand() % 100 >= 2;       // RF(50)
        if(ZielType == T_KREUZER)
            return CSpeedKernel::GetInstance().RandomNumber(10000) >= 303;  // RF(33)        
        if(ZielType == T_BOMBER)
            return rand() % 100 >= 4;       // RF(25)
        if(ZielType == T_ZER)
            return rand() % 100 >= 20;      // RF(5)
        // other ships
        return CSpeedKernel::GetInstance().RandomNumber(1000) >= 4;         // RF(250)
	}
    if((ZielType == T_SPIO || ZielType == T_SAT) && AttType != T_SPIO && AttType < T_SHIPEND)
		return rand() % 100 >= 20;		    // RF(5)

	if(AttType == T_KREUZER)
	{
		if(ZielType == T_LJ)
			return rand() % 100 >= 33;	    // RF(3)
		
		if(ZielType == T_RAK)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	if(AttType == T_ZER)
	{
		if(ZielType == T_LL)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	if(AttType == T_BOMBER)
	{
		if(ZielType == T_LL || ZielType == T_RAK)
			return rand() % 100 >= 5;	    // RF(20)
		if(ZielType == T_SL || ZielType == T_IONEN)
			return rand() % 100 >= 10;	    // RF(10)

		return false;
	}

	return false;
}

bool CSpeedKernel::CanShootAgain_FromTable(ITEM_TYPE AttType, ITEM_TYPE TargetType)
{
    CSpeedKernel& inst = CSpeedKernel::GetInstance();
    return inst.RandomNumber(10000) >= inst.m_RF[AttType][TargetType];
}

// computes new best/worst case
void CSpeedKernel::UpdateBestWorstCase(int CurSim)
{
	PR_PROF_FUNC(F_ADDPTONUMBER);
    
    if(!m_CompBestWorstCase)
		return;

	int a[T_END], d[T_END];
	size_t i;
    for(i = 0; i < T_END; i++)
	{
		a[i] = 0;
		d[i] = 0;
	}

    // count ships
    for(i = 0; i < m_AttObj->size(); i++)
		a[(*m_AttObj)[i].Type]++;

    for(i = 0; i < m_DefObj->size(); i++)
		d[(*m_DefObj)[i].Type]++;

    // check if better / worse
	for(i = 0; i < T_END; i++)
	{
		if(m_BestCaseAtt[i] < a[i])
			m_BestCaseAtt[i] = a[i];
		if(m_WorstCaseAtt[i] > a[i])
			m_WorstCaseAtt[i] = a[i];

		if(m_BestCaseDef[i] < d[i])
			m_BestCaseDef[i] = d[i];
		if(m_WorstCaseDef[i] > d[i])
			m_WorstCaseDef[i] = d[i];
        
#ifdef CREATE_ADV_STATS
        // save result
        m_CombatResultsAtt[CurSim][i] = a[i];
        m_CombatResultsDef[CurSim][i] = d[i];
#endif
	}
}


// updates number of ships for each round (needed for combat reports)
void CSpeedKernel::SaveShipsToCR(int round)
{
	PR_PROF_FUNC(F_SSTOKB);
    
    round--;
	m_NumSimulatedRounds[round]++;
	size_t t, i;
    for(i = 0; i < m_AttObj->size(); i++)
	{
		t = (*m_AttObj)[i].Type;
		m_NumShipsInKBAtt[round][t].Num++;
	}

	for(i = 0; i < m_DefObj->size(); i++)
	{
		t = (*m_DefObj)[i].Type;
		m_NumShipsInKBDef[round][t].Num++;
	}
}

bool CSpeedKernel::InitSim()
{
    PR_PROF_FUNC(F_INITSIM);

    // more randomness
    InitRand();
    srand(time(NULL));

    // calculate cost, hull, shield etc. of ships
    ComputeShipData();

    Obj obj;
    DWORD CurrPl = 999;
    unsigned int item = 0;
    int i;
    // reset fleet
	m_AttObj->clear();
    m_DefObj->clear();
	m_NumShipsAtt.resize(T_RAK*MAX_PLAYERS_PER_TEAM);
    m_NumShipsDef.resize((T_END)*MAX_PLAYERS_PER_TEAM);
	for(i = 0; i < MAX_PLAYERS_PER_TEAM; i++)
	{
		for(int j = 0; j < T_SHIPEND; j++)
		{
			m_NumShipsAtt[i*T_SHIPEND+j].OwnerID = i;
			m_NumShipsAtt[i*T_SHIPEND+j].Type = (ITEM_TYPE)j;
			m_NumShipsAtt[i*T_SHIPEND+j].Num = 0;
		}
	}
	for(i = 0; i < MAX_PLAYERS_PER_TEAM; i++)
	{
		for(int j = 0; j < T_END; j++)
		{
			m_NumShipsDef[i*(T_END)+j].OwnerID = i;
			m_NumShipsDef[i*(T_END)+j].Type = (ITEM_TYPE)j;
			m_NumShipsDef[i*(T_END)+j].Num = 0;
		}
	}
	for(item = 0; item < m_NumSetShipsAtt.size(); item++)
	{
        // create attacking objects; for every ship 1 object
		obj = FillObj(m_NumSetShipsAtt[item].Type, ATTER, m_NumSetShipsAtt[item].OwnerID);
        if(m_NumSetShipsAtt[item].OwnerID != CurrPl)
            CurrPl = m_NumSetShipsAtt[item].OwnerID;
        for(size_t o = 0; o < m_NumSetShipsAtt[item].Num; o++)
			m_AttObj->push_back(obj);
	}
    if(CurrPl < MAX_PLAYERS_PER_TEAM)
        m_NumPlayersPerTeam[ATTER] = CurrPl+1;
    else
        m_NumPlayersPerTeam[ATTER] = 0;

    CurrPl = 999;
	for(item = 0; item < m_NumSetShipsDef.size(); item++)
	{
        // defending units
		obj = FillObj(m_NumSetShipsDef[item].Type, DEFFER, m_NumSetShipsDef[item].OwnerID);
        if(m_NumSetShipsDef[item].OwnerID != CurrPl)
            CurrPl = m_NumSetShipsDef[item].OwnerID;
		for(size_t o = 0; o < m_NumSetShipsDef[item].Num; o++)
			m_DefObj->push_back(obj);
	}
    if(CurrPl < MAX_PLAYERS_PER_TEAM)
        m_NumPlayersPerTeam[DEFFER] = CurrPl+1;
    else
        m_NumPlayersPerTeam[DEFFER] = 0;

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

    // reset best/worst case
	for(int u = 0; u < T_END; u++)
	{
		m_WorstCaseAtt[u] = 999999999;
		m_BestCaseAtt[u] = 0;
		m_WorstCaseDef[u] = 999999999;
		m_BestCaseDef[u] = 0;
	}

    // reset combat report data
	for(int r = 0; r < 7; r++)
	{
		for(int i = 0; i < T_END; i++)
		{
			m_NumShipsInKBAtt[r][i].Num = 0;
			m_NumShipsInKBAtt[r][i].Type = (ITEM_TYPE)i;
			m_NumShipsInKBDef[r][i].Num = 0;
			m_NumShipsInKBDef[r][i].Type = (ITEM_TYPE)i;
		}

		m_NumSimulatedRounds[r] = 0;

		if(r < 6)
		{
			m_NumShotsPerRoundAtt[r] = 0;
			m_NumShotsPerRoundDef[r] = 0;
			m_ShotStrengthAtt[r] = 0;
			m_ShotStrengthDef[r] = 0;
			m_AbsorbedAtt[r] = 0;
			m_AbsorbedDef[r] = 0;
		}
	}
	return true;
}

void CSpeedKernel::ComputeCRArrays()
{
    PR_PROF_FUNC(F_CKBARRAYS);

	for(int r = 0; r < 7; r++)
	{
		for(int t = 0; t < T_END; t++)
		{
			if(m_NumSimulatedRounds[r] > 0)
			{
				m_NumShipsInKBAtt[r][t].Num /= m_NumSimulatedRounds[r];
                if(m_NumShipsInKBAtt[r][t].Num > 0)
                    m_NumShipsInKBAtt[r][t].Num += 0.5f;
				m_NumShipsInKBDef[r][t].Num /= m_NumSimulatedRounds[r];
                if(m_NumShipsInKBDef[r][t].Num > 0)
                    m_NumShipsInKBDef[r][t].Num += 0.5f;
			}
			else
			{
				m_NumShipsInKBAtt[r][t].Num = 0;
				m_NumShipsInKBDef[r][t].Num = 0;
			}
		}
		if(r < 6)
		{
			if(m_NumSimulatedRounds[r] > 0)
			{
				m_NumShotsPerRoundAtt[r] /= m_NumSimulatedRounds[r];
				m_NumShotsPerRoundDef[r] /= m_NumSimulatedRounds[r];
				m_ShotStrengthAtt[r] /= m_NumSimulatedRounds[r];
				m_ShotStrengthDef[r] /= m_NumSimulatedRounds[r];
				m_AbsorbedAtt[r] /= m_NumSimulatedRounds[r];
				m_AbsorbedDef[r] /= m_NumSimulatedRounds[r];
			}
		}
	}
}

void CSpeedKernel::AbortSim()
{
	long u = 0;
    m_DataIsDeleted = true;
    
    if(m_SimulateFreedItsData)
        return;

    // wait until simulation is initialised
    while(!m_InitSim)
		u++;

	DWORD num = 0;
    size_t i;
    // count number of ships, to wait a certain time
	for(i = 0; i < m_NumSetShipsAtt.size(); i++)
		num += m_NumSetShipsAtt[i].Num;
	for(i = 0; i < m_NumSetShipsDef.size(); i++)
        num += m_NumSetShipsDef[i].Num;
	DWORD warten = ((float)num / WARTEN_PRO_100K) * CLOCKS_PER_SEC / 1000.0f;

	DWORD start = clock();
	while((clock() - start < warten) || !m_SimulateFreedItsData)
		u++;
}
