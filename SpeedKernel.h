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

#ifndef SPEEDKERNEL_H
#define SPEEDKERNEL_H

#pragma warning(disable: 4251 4244 4786 4996 4503)
#pragma once

#include <vector>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <stdarg.h>

using namespace std;

#include "SKernel_Defs.h"
#include "SKernel_Structs.h"
#include "CIniFile.h"
#include "utf8.h"

//! The SpeedKernel Class which contains all simulating/reading functions
/*!
    Because CSpeedKernel is a singleton class, create an object that way:
    \code
      CSpeedKernel& kernel = CSpeedKernel.GetInstance();
    \endcode
    \author Nicolas Höft
    \author Maximilian Matthé
    \date 2004-2006
*/

class SPEEDKERNEL_API CSpeedKernel
{
public:
    //! Function to get the current instance
    static CSpeedKernel& GetInstance();
    //! Force new Instance - breaks sigleton class but needed for
    CSpeedKernel& ForceNewInstance();

    ~CSpeedKernel();

    //! Simulate starts a simulation
    /*!
        \param[in] count sets, how many times the battle is simulated
        \sa SetFleet()
    */
    bool Simulate(int count = 1);

    //! Aborts the simulation - works only, if you simulate in a different thread
    void AbortSim();

    //! reset all options / SpeedSimData
    void Reset();

    //! simulates an ip missiles attack
    IPMBattleResult SimulateIPM(int NumIPM, int NumABM, int FleetID, ITEM_TYPE PrimaryItem = T_RAK);

    //! \name Fleet related functions
    //! Functions important for fleet information
    //@{
    //! Gets the worth of the currently set fleet
    void GetFleetWorth(Res &att, Res &def);

    //! Gets the worth of a fleet
    Res GetFleetWorth(vector<SItem> *Fleet);

    //! Sets a fleet - those fleets will fight each other when you simulate
    /*!
        Please only set different fleets (for Alliance Combat System) in the correct order
        or one by another.
        Fleets with the same ID will be overwritten
        \sa GetSetFleet(), GetFleetAfterSim()
    */
	bool SetFleet(vector<SItem>* Attacker, vector<SItem>* Defender);

    //! Sets a fleet - overloaded function of SetFleet for those who like arrays more
    /*!
        Please only set different fleets (for Alliance Combat System) in the correct order
        or one by another.
        Fleets with the same ID will be overwritten
        \sa SetFleet()
        \param[in] size_att Array size of 'Attacker'
        \param[in] size_def Array size of 'Defender'
    */
    bool SetFleet(SItem* Attacker, SItem* Defender, int size_att, int size_def);

    //! Gets the fleets, which survived with the used Fleet ID
    /*!
        Both SItem arrays must have the length T_END - or else there will be an access violation
        or similar.
        \param[out] Attacker, Defender can be NULL if you want to set only one of them
        \param[in] FleetID ID of the fleet of the attacker/defender you want to get
    */

    bool GetFleetAfterSim(SItem* Attacker, SItem* Defender, int FleetID);

    //! vector version of the other version
    bool GetFleetAfterSim(vector<SItem>* Attacker, vector<SItem>* Defender, int FleetID);

    //! Returns the current set fleet for a certain FleetID
    /*!
        \param[out] Attacker, Defender array size has to be T_END
        \param[in] FleetID ID of the Attacker/Defender you want the fleet from
        If Attacker or Defender are NULL the fleet won't be returned to that one
    */
    bool GetSetFleet(SItem* Attacker, SItem* Defender, int FleetID);
    bool GetSetFleet(vector<SItem>* Attacker, vector<SItem>* Defender, int FleetID);

    //! Sets the Technologies for a certain attacker and defender
    /*!
        \returns true if successful or false, if not
    */
    void SetTechs(ShipTechs* att, ShipTechs* def, DWORD PlayerID);

    //! Returns the technologies of attacker and defender
    /*!
        \param[out] att receives attackers technologies
        \param[out] def receives defenders technologies
        \param[in] PlayerID ID of the fleet where you want to get the technologies from
        Returns the ships technologies for the attacker and defender
    */
    void GetTechs(ShipTechs* att, ShipTechs* def, DWORD PlayerID);

    //! Sets the speed for a certain attacker
    /*!
        The size att and def must be both 3
        \param[in] v contains combustion engine level
        \param[in] i contains impulse engine level
        \param[in] h contains hyperspace engine level
        \param[in] FleetID ID of the fleet you want set the technologies/speed
        \param[in] perc_speed Velocity of the fleet (100% = 100)
    */
    void SetSpeed(int perc_speed, int v, int i, int h, int FleetID);

    //! Gets the speed of a certain fleet
    /*!
        \sa SetSpeed()
    */
    void GetSpeed(int &perc_speed, int &v, int &i, int &h, int FleetID);
    //@}
    //! Rebuilds defense, removes the robbered resources and sets the best case for the attacker for the fleets
    void SetRemainingItemsInDef();


    //! \name Reading Functions
    //!  Functions for reading in several texts from OGame
    //@{
    /** Reading functions for reading in several OGame texts */

    //! Reads in an espionage report
    //! \param[out] ti Contains information read in
    bool ReadEspReport(genstring& r, TargetInfo& ti);
    //! Reads in an espionage report
    //! \param[in] FleetID To which fleet slot is the data set to
    bool ReadEspReport(genstring& r, int FleetID);
    //! reads in more esp reports
    //! \return Number of espionage reports detected
    int MultiReadEspReport(genstring reports, TargetInfo* TIs, int nMaxTI);
	//! Reads in the overview if you're being attacked and have a espionage technology > 7
    //! \sa GeneralRead()
    bool ReadOverview(const genstring& s, int FleetID);
    //! Reads the fleet from the fleet menu
    //! \sa GeneralRead()
    bool ReadOwnFleet(const genstring &OwnFleet, int FleetID);
    //! Reads in a combat report
    //! \sa GeneralRead()
    bool ReadCR(const genstring &KB);
    //! Reads in the text from the research lab
    //! \sa GeneralRead()
    bool ReadTechs(const genstring &TechText, int FleetID);
    //! Tries to detect the type of the text and reads it in
    /*!
        \return Type of Text or TEXT_UNKNOWN
        \sa TEXTTYPE
    */
    TEXTTYPE GeneralRead(genstring& s, int FleetID = 0);
    //@}

    //! \name Kernel Options
    //! Functions for setting/getting some options for the kernel
    //@{
    //! Sets the callback function which is called after every round that you can update your output
    /*!
        \param[in,out] f Is a function pointer to the function which is called after every round. If set to NULL, the callback wont be called
    */
	void SetCallBack(void (*f)(int sim, int round));

    //! Sets whether destroyed defense units go into debris
	void SetDefInDebris(bool really);
    //! sets the RF used in the simulation
	void SetRF(RFTYPE ver);
    //! Use new shield calculation - NOT USED
    void SetNewShield(bool really);
    //! Use new fuel calculation (OGame > v0.68a
    void SetNewFuel(bool bNewFuel);
    //! Sets the factor how many defense units will be rebuilt in SetRemainingItemsInDef
    /*! \sa SetRemainingItemsInDef() */
	void SetDefRebuildFactor(float RebuildFac);
    //! Set if the ship data should be read from a file
    /*!
        The file set to be set before with LoadShipData
        \sa LoadShipData(), UsesShipDataFromFile()
    */
    void SetUseShipDataFromFile(bool bFileData);
    //! Sets if small defense should be rebuilt with 100%
    /*! Default value is false */
    void RebuildSmallDefense(bool bRebuild);
    //! Gets if the ship data is read from a file
    /*!
        The file set to be set before with LoadShipData
        \sa LoadShipData(), SetUseShipDataFromFile()
    */
    bool UsesShipDataFromFile();

    //! Sets if the best/worst case data should be collected and the html generated
    /*! \sa GenerateBestWorseCase */
	void SetComputeBestWorstCase(bool how);

    //! Sets the starting position for a attacker
    /*
        This is needed for correct fuel calculation
        \sa GetOwnPosition()
    */
    void SetOwnPosition(const PlaniPos& p, int FleetID);
    //! Gets the starting position for a attacker
    /*! \sa SetOwnPosition() */
    void GetOwnPosition(PlaniPos& p, int FleetID);
    //! Sets information for defender
    void SetTargetInfo(TargetInfo TI, int FleetID, bool ResetWavesState = true);
    //! Get the information of defender
    TargetInfo GetTargetInfo(int FleetID);

    //! Resets the state wave
    void ResetWaveState();

    //! Sets the css files used in best/wors-case & combat report
    /*
        Use NULL as param, to reset to default values
    */
    void SetCSSFiles(TCHAR* cr_css, TCHAR* bwc_css);
    //@}
    //! Generates the BWC html into file
    /*!
        \return false if file could not created or the data hasn't been collected. Else true will be returned.
        \sa SetComputeBestWorstCase
    */
    bool GenerateBWC(const char* file);
    //! \return The BestWorst-Case HTML data (is in UTF-8 format)
    string GenerateBWC();

    //! Generates the combat report
    //! \return false if file could not created or else true
	bool GenerateCR(const char* file);

    //! Generates combat report
    //! \return string containg the html data (is in UTF-8 format)
    string GenerateCR();

    //! returns the battle result information
    /*!
        \return The battle result structure
        \sa BattleResult
    */
    BattleResult& GetBattleResult();

    //! \name Fleet Item names and types
    //! Information about fleet names and types
    //@{

    //! Copies the name of a fleet item into out
    /*!
        \param[in] Type Item from which one you want to have the name
        \param[out] out Pointer to a string where the name is copied to. The size should be about 20 characters
    */
	void ItemName(ITEM_TYPE Type, TCHAR* out);
    //! Copies the name of an item to out as its used in the ini files
    //! \sa ItemName()
    void ItemIniName(ITEM_TYPE Type, TCHAR* out);
    //! Returns the ItemName
    /*!
        \sa ItemName2()
        \return Name of the ship / defense unit
    */
	genstring ItemName(ITEM_TYPE Type);
    //! Returns alternative name for ship / defense unit
    /*!
        \sa ItemName()
        \return Alternative name for the ship / defense unit
    */
    genstring ItemName2(ITEM_TYPE Type);
    //! Returns name of ship as used in combat reports
    /*!
        \sa ItemName(), ItemName2(), ItemIniName()
        \return Name for the ship / defense unit as used in combat reports
    */
    genstring CRItemName(ITEM_TYPE t);
    //! returns item type from item name
    /*!
        \sa ItemName(), ItemName2(), ItemIniName()
        \return ITEM_NONE if name could not be detected
    */
    ITEM_TYPE GetItemType(genstring& name);
    //! returns capacity of an item
    /*!
        \return Capacity the item has
    */
    unsigned long GetShipCapacity(ITEM_TYPE ship);
    //@}

    //! copies the current version into out
    /*! \param[out] out should have a size of 10 characters */
	void GetVersion(TCHAR* out);
    //! Loads language data from a ini file
    /*!
        \return false, if an error occurred
    */
    bool LoadLangFile(const char *langfile);

    //! loads RF values from a RF-ini file
    /*!
        \sa LoadShipData()
        \return This function should return true
    */

    bool LoadRFFile(char *RFFile);
    //! Loads ship data from a ini file
    /*!
        \sa LoadRFFile()
        \return This function should return true
    */
    bool LoadShipData(char *SDFile);

    // speed optimized functions - no ACS, only basic combat result
#ifdef INCL_OPTIMIZED_FUNCTIONS
    const BattleResult& OptimizedSimulate(int count, const vector<SItem>* Attacker, const vector<SItem>* Defender, int* techAtt, int* techDef);
    void InitOptimized();
#endif

private:
	CSpeedKernel();
    // copy constructor
	CSpeedKernel(CSpeedKernel& s);
    CSpeedKernel(CSpeedKernel* s);
    CSpeedKernel& operator=(CSpeedKernel& s){ return *((CSpeedKernel*)NULL); };

    bool InitSim();
    void ComputeShipData();
    void MaxAllShields();
	void EndRound();
	void ShipsDontExplode();
	void DestroyExplodedShips();
	void ShipShoots(Obj& o, int Team, DWORD AtterID);
#ifdef INCL_OPTIMIZED_FUNCTIONS
    void OptimizedShipShoots(Obj& o, int Team);
#endif

	bool ParseSpioLine(genstring& s, TargetInfo& ti);
    bool ReadCRTable(genstring Table, vector<SItem> &Fleet, ShipTechs &techs);
	void ComputeLoot();
	void ComputeBattleResult();
    void UpdateBestWorstCase(int CurSim);
    void ComputeLosses();
#ifdef INCL_OPTIMIZED_FUNCTIONS
    void OptimizedComputeLosses();
#endif
#ifdef CREATE_ADV_STATS
    void CreateAdvShipStats();
#endif

    void SaveShipsToCR(int round);
	void ComputeCRArrays();
    void GenerateCRTable(genstrstream &out, const vector<SItem> &Items, int Team, int Player, TCHAR* Title);

    int GetDistance(const PlaniPos& b, const PlaniPos& e);
    DWORD ComputeFlyTime(const PlaniPos& b, const PlaniPos& e, int FleetID, const vector<SItem>& vFleet);
    int GetShipSpeed(ITEM_TYPE Ship, int FleetID);
    int GetFleetSpeed(int FleetID, const vector<SItem>& vFleet);

    //genstring ReadStringFromIniFile(char *inifile, const TCHAR *Section, const TCHAR *KeyName);
    //bool ReadLine(FILE *file, TCHAR *out, int max_length);
    Res StringToRes(const genstring &val);

    void InitRand();
    ULONG RandomNumber(ULONG Max);
    TCHAR* AddPointsToNumber(__int64 value, TCHAR* out);

    Obj FillObj(ITEM_TYPE Type, int Team, DWORD PlayerID);
    static bool ItemCompare(const SItem& a, const SItem& b);

	//bool CanShootAgain(ITEM_TYPE AttType, ITEM_TYPE ZielType);
	// becomes function pointer
    bool (*CanShootAgain)(ITEM_TYPE AttType, ITEM_TYPE ZielType);
	static bool CanShootAgain_V059(ITEM_TYPE AttType, ITEM_TYPE ZielType);
	static bool CanShootAgain_V058(ITEM_TYPE AttType, ITEM_TYPE ZielType);
    static bool CanShootAgain_V062(ITEM_TYPE AttType, ITEM_TYPE ZielType);
    static bool CanShootAgain_V065(ITEM_TYPE AttType, ITEM_TYPE ZielType);
    static bool CanShootAgain_FromTable(ITEM_TYPE AttType, ITEM_TYPE TargetType);
    // own RF
    static bool CanShootAgain_User(ITEM_TYPE AttType, ITEM_TYPE ZielType);
    // deactivated RF
    static bool CantShootAgain(ITEM_TYPE AttType, ITEM_TYPE ZielType);

    void FillRFTable(RFTYPE rfType);

    // Rapid Fire only for OptShipShoots
#ifdef INCL_OPTIMIZED_FUNCTIONS
    bool CanShootAgain_Optimized(ITEM_TYPE AttType, ITEM_TYPE TargetType);
#endif

	// pointer to function which outputs number of rounds and simulations
    void (*m_FuncPtr)(int sim, int round);

	vector<SItem> m_NumShipsAtt;
	vector<SItem> m_NumShipsDef;

	vector<SItem> m_NumSetShipsAtt;
	vector<SItem> m_NumSetShipsDef;

	vector<SItem> m_NumShipsInKBAtt[7], m_NumShipsInKBDef[7];
	DWORD m_NumShotsPerRoundAtt[6], m_NumShotsPerRoundDef[6];
	double m_ShotStrengthAtt[6], m_ShotStrengthDef[6];
	double m_AbsorbedAtt[6], m_AbsorbedDef[6];
	DWORD m_NumSimulatedRounds[7];

#ifdef CREATE_ADV_STATS
    // number of ships after battle
    vector<vector<int> > m_CombatResultsAtt, m_CombatResultsDef;
    // losses / win
    vector<Res> m_LossAtt, m_LossDef;
    vector<Res> m_DebrisFields;
    //vector<Res> m_
#endif

	int m_CurrentRound;

	vector<Obj>* m_AttObj, *m_DefObj;
    int m_NumPlayersPerTeam[2];

	int m_BestCaseAtt[T_END];
	int m_BestCaseDef[T_END];
	int m_WorstCaseAtt[T_END];
	int m_WorstCaseDef[T_END];

	BattleResult m_Result;
    WaveInfo m_Waves;

	bool m_DefInTF;
    bool m_NewFuel;
	bool m_CompBestWorstCase;
    bool m_RebuildSmallDef;

	bool m_DataIsDeleted;
	bool m_SimulateFreedItsData;
    bool m_InitSim;
    bool m_LastScanHadTechs;
    bool m_BracketNames;

    bool m_NewShield;
    float m_DefRebuildFac;
    bool m_ShipDataFromFile;

    // translations strings
    genstring m_FleetNames[T_END + 2];
    genstring m_altFleetNames[T_END + 1];
    genstring m_KBNames[T_END + 1];
    // techs: 1=weap, 2=shield, 3=hull, 4=combustion, 5=Impuls, 6=hyperspace
    genstring m_TechNames[6];
    // stop string for esp. reports
    genstring m_EspLimiter;
    // espionage report titles 1=Fleets, 2=Defence, 3=Reseach
    genstring m_EspTitles[3];
    // resources: 1= Metal, 2= Crystal, 3 = Deuterium
    genstring m_Ress[3];
    // mines: 1=metal, 2=crystal, 3=deuterium
    genstring m_wrongRess[3];
    // storages: 1=metal 2=crystal, 3=deuerium
    genstring m_wrongRess2[3];

    // 1="auf ", 2="] am " 3 = Research 4 = Lunar Base
    genstring m_Spiostrings[4];
    // 1= "dem Planeten ", 2 = "] Die Flotte"
    genstring m_Ovr[2];
    // text before number of defense units
    genstring m_Defense;
    // text before the technology level
    genstring m_TechPreString;
    genstring m_Attacker, m_Defender;

    // CR-table down from top
    genstring m_KBTable[5];
    // 1-weapons, 2-shields, 3-hull
    genstring m_KBTechs[3];
    // texts between rounds
    genstring m_KBRoundStr[4];
    genstring m_KBResult[9];
    genstring m_KBLoss[3];
    genstring m_KBMoon;
	genstring m_KBTitle;
    // Charset for correct output
    genstring m_HTML_Charset;

    genstring m_BWC_CSS, m_CR_CSS;

	genstring m_Bilanzstrings[10];

    // 0-min, 1-Max, 2-average, 3-Sum, 4 -debris 5- ship, 6- after rebuild 7-best 8-worst
    genstring m_BWTable[9];
    // 1-title 2-Best/Worst Case for attacker, 3- ..for defender
    genstring m_BWTitle[3];
    // losses: 1:title, 2:attacker, 3:defender 4:units
    genstring m_BWLoss[4];

	PlaniPos m_OwnPos[MAX_PLAYERS_PER_TEAM];

    TargetInfo m_DefenderInfos[MAX_PLAYERS_PER_TEAM];
    //TargetInfo m_LastTarget;

	ShipTechs m_TechsAtt[MAX_PLAYERS_PER_TEAM];
	ShipTechs m_TechsDef[MAX_PLAYERS_PER_TEAM];
    int m_TechsTW[MAX_PLAYERS_PER_TEAM][3];

    // fleet speed (100% = 10)
    int m_Speed[MAX_PLAYERS_PER_TEAM];

    // RF-values read from file
    int m_RF[T_END][T_END];

    // names of ships in ini-files
    genstring m_IniFleetNames[T_END];
    // unit costs
    Res Kosten[T_END];
    // max. shields
    float MaxShields[MAX_PLAYERS_PER_TEAM+1][2][T_END];
    // max. life
    float MaxLifes[MAX_PLAYERS_PER_TEAM+1][2][T_END];
    // damage
    float Dams[MAX_PLAYERS_PER_TEAM+1][2][T_END];
    // capacities
    long LadeKaps[T_END];
    // fuel usage
    WORD Verbrauch[T_END];
    // ship engines
    TRIEBWERK Triebwerke[T_END];
    int BaseSpeed[T_END];
};

void CheckVector(const vector<SItem>& v);
void CheckVector(const vector<Obj>& v);

#endif
