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

#ifndef SKERNEL_STUCTS_INC
#define SKERNEL_STUCTS_INC

#include "SKernel_Defs.h"
#include <cstring>

/*!
    \file
    \brief This file contains all needed structs and enums
*/


// for different RF versions
/*!
    \enum RFTYPE
    \brief This enum is used to diff between RF versions
    \sa CSpeedKernel::SetRF()
*/
enum RFTYPE {
    RF_NONE = 0,    /*!< disabled RapidFire */
    RF_058,         /*!< for OGame >= v0.58 */
    RF_059,         /*!< for OGame >= v0.59 */
    RF_062,         /*!< for OGame >= v0.62 */
    RF_065,         /*!< for OGame >= v0.65 */
    RF_075,         /*!< for OGame >= v0.75 */
    RF_USER,        /*!< user set RapidFire */
};

/*!
    \enum TEXTTYPE
    \brief This is for the return value of GeneralRead
    \sa CSpeedKernel::GeneralRead()
*/
enum TEXTTYPE {
    TEXT_ESPIO = 0, /*!< Espionage Report*/
    TEXT_COMBAT,    /*!< Combat report */
    TEXT_OVERVIEW,  /*!< Overview, if you're being attacked */
    TEXT_OWNFLEET,  /*!< Own fleet */
    TEXT_TECH,      /*!< Text from research lab menu */
    TEXT_UNKNOWN,   /*!< Unknown Text */
    TEXT_MULTIESPIO /*!< More then 1 espionage report */
};

/*!
    \enum REBUILD_OPTION
    \brief Options for rebuilding defense
    \sa CSpeedKernel::SetRemainingItemsInDef()
*/
enum REBUILD_OPTION
{
    REBUILD_BESTCASE,
    REBUILD_WORSTCASE,
    REBUILD_AVG,
};

/*!
    \enum ITEM_TYPE
    \brief This enum describes the Type of the ship/defense unit
*/
enum ITEM_TYPE
{
    T_NONE = -1,    /*!< No fleet item */
	// ships
	T_KT = 0,       /*!< Small Cargo Ship */
	T_GT,           /*!< Large Cargo Ship */
	T_LJ,           /*!< Light Fighter */
	T_SJ,           /*!< Heavy Fighter */
	T_KREUZER,      /*!< Cruiser */
	T_SS,           /*!< Battle Ship */
	T_KOLO,         /*!< Colony Ship */
	T_REC,          /*!< Recycler */
	T_SPIO,         /*!< Espionage Probe */
	T_BOMBER,       /*!< Bomber */
	T_SAT,          /*!< Solar Satellite */
	T_ZER,          /*!< Destroyer */
	T_TS,           /*!< Death Star */
    T_IC,           /*!< Interceptor */

    // defense
	T_RAK,          /*!< Missile Launcher */
	T_LL,           /*!< Small Laser */
	T_SL,           /*!< Heavy Laser */
	T_GAUSS,        /*!< Gauss Cannon */
	T_IONEN,        /*!< Ion Cannon */
	T_PLASMA,       /*!< Plasma Cannon */
	T_KS,           /*!< Small Shield Dome */
	T_GS,           /*!< Large Shield Dome */

    T_END,              /*!< Number of Items (Ships + Def) - can be used for loop e.g. */
    T_SHIPEND = T_RAK,  /*!< Number of Ships (e.g. for loops) */
};

/*!
    \enum TRIEBWERK
    \brief Engine Types
*/
enum TRIEBWERK {
    TW_VERBRENNUNG = 0, /*!< Combustion Engine */
    TW_IMPULS,          /*!< Impulse Engine */
    TW_HYPERRAUM,       /*!< Hyperspace Engine */
};

/*!
    \struct SItem
    \brief Struct for all Fleet Items containing Type, Number of Items and Owner
*/
struct SPEEDKERNEL_API SItem
{
	//! Type of item \sa ITEM_TYPE
    ITEM_TYPE Type;
	//! Number of Items
    double Num;
	//! Owner of this Item
    BYTE OwnerID;
    SItem() { Type = ITEM_TYPE(0); Num = 0; OwnerID = 0; };
    SItem(ITEM_TYPE UnitType, double Number, BYTE FleetID)
    {
        Type = UnitType;
        Num = Number;
        OwnerID = FleetID;
    }
    static bool IsLess(const SItem& r1, const SItem& r2) { return (int)r1.Type < (int)r2.Type; }
};

/*!
    \struct Res
    \brief Struct containing the resources
*/
struct SPEEDKERNEL_API Res
{
	//! Metal
    __int64 met;
	//! Crystal
    __int64 kris;
	//! Deuterium
    __int64 deut;
	Res(__int64 a, __int64 b, __int64 c) { met = a; kris = b; deut = c; };
	Res(__int64 a, __int64 b) { met = a; kris = b; deut = 0; };
	Res() { met = 0; kris = 0; deut = 0; };
	/*void operator/=(float num)
	{
	    met /= num;
		kris /= num;
		deut /= num;
	};*/
    void operator/=(int num)
	{
	    met /= num;
		kris /= num;
		deut /= num;
	}

	void operator+=(const Res& r)
	{
		met += r.met;
		kris += r.kris;
		deut += r.deut;
	}

	void operator-=(const Res& r)
	{
		met -= r.met;
		kris -= r.kris;
		deut -= r.deut;
	};

	Res operator-(const Res& r)
	{
		return Res(met - r.met, kris - r.kris, deut - r.deut);
	}

	Res operator+(const Res& r)
	{
		return Res(met + r.met, kris + r.kris, deut + r.deut);
	}

	void operator *=(float num)
	{
		met *= num;
		kris *= num;
		deut *= num;
	}

	Res operator/(float num)
	{
		return Res(met / num, kris / num, deut / num);
	}

    Res operator/(int num)
    {
        return Res(met / num, kris / num, deut / num);
    }

	Res operator*(float num)
	{
		return Res(met * num, kris * num, deut * num);
	}

    Res operator*(int num)
    {
        return Res(met * num, kris * num, deut * num);
    }

    Res operator*(double num)
    {
        return Res(met * num, kris * num, deut * num);
    }

	bool operator!=(const Res& r)
	{
		if(met != r.met)
			return true;
		if(kris != r.kris)
			return true;
		if(deut != r.deut)
			return true;

		return false;
	};
};

struct SPEEDKERNEL_API Obj
{
	ITEM_TYPE Type;
	float Shield;
	float Life;
	bool Explodes;
    BYTE PlayerID;
	Obj() { Shield = 0;
			Life = 0;
			Explodes = false; };
};

/*!
    \struct PlaniPos
    \brief Planet position with galaxy, system and position in system
*/
struct SPEEDKERNEL_API PlaniPos
{
	//! Galaxy
    int Gala;
	//! System
    int Sys;
	//! Position in System
    int Pos;

    //! moon or not
    bool bMoon;

    //! Constructor
    //! Sets all positions to 0
	PlaniPos() { Gala = 0; Sys = 0; Pos = 0; bMoon = false;};
    //! Constructor to set positions immediatly
	PlaniPos(int g, int s, int p) { Gala = g; Sys = s; Pos = p; bMoon = false;};
    //! Constructor, where first param contains string with the position in format GG:SSS:PP
    PlaniPos(const TCHAR* c) { *this = PlaniPos(genstring(c)); };
	//! \sa PlaniPos(TCHAR* c)
    PlaniPos(const genstring p)
	{
        bMoon = false;
		size_t f, f2;
		f = GetNextNumber(p, Gala);
		if(f == p.npos)
		{
			*this = PlaniPos();
			return;
		}
		f2 = GetNextNumber(p, Sys, f + 1);
		if(f2 == p.npos)
		{
			*this = PlaniPos();
			return;
		}
		GetNextNumber(p, Pos, f2 + 1);
	};
    //! Returns position in format GG:SSS:PP
    genstring ToString() const
    {
        genstrstream c;
        c << Gala << _T(":") << Sys << _T(":") << Pos;
        return c.str();
    };
    //! compares two positions
    bool operator==(const PlaniPos p) const
    {
        return Gala == p.Gala && Sys == p.Sys && Pos == p.Pos && bMoon == p.bMoon;
    };
    bool operator!=(const PlaniPos p) const
    {
        return !(p == *this);
    }
};

/*!
    \struct BattleResult
    \brief This struct contains all information about the battle result
*/
struct SPEEDKERNEL_API BattleResult
{
    //! percentual chance to win / lose / draw
    float AttWon, DefWon, Draw;
	//! Name of target planet
    TCHAR PlaniName[64];
    //! Target planet position
	PlaniPos Position;
    //! Losses of attacker
	Res VerlusteAngreifer;
    //! Losses of defender
	Res VerlusteVerteidiger;
    //! Losses of defender after defense is rebuilt
    Res VerlVertmitAufbau;
    //! Losses in all waves together
    Res GesVerlust;
	//! Average debris field
    Res TF;
    //! Maximum debris
    Res MaxTF;
    //! Minimum debris
    Res MinTF;
    //! Debris field after wave attacks
    Res GesTF;
	//! Max. losses for attacker
    Res MaxVerlAtt;
    //! Min. losses for attacker
	Res MinVerlAtt;
	//! Max. losses for defender
    Res MaxVerlDef;
    //! Min. losses for defender
	Res MinVerlDef;
    //! Worth of attacking fleet
	Res WertAtt;
    //! Worth of defending fleet
    Res WertDef;
    //! Losses/Win of metal you make when you collect all metal from debris
	float PercentInTFMet;
    //! Losses/Win of crystal you make when you collect all crystal from debris
    float PercentInTFKris;
    //! Average number of recyclers needed to collect all resources from debris
	int NumRecs;
    //! Maximum number of recyclers needed to collect all resources from debris
	int MaxNumRecs;
    //! Minimum number of recyclers needed to collect all resources from debris
	int MinNumRecs;
    //! Recyclers you need to collect all  resources from debris after wave attacks
    int GesamtRecs;
    //! Number of average rounds a battles takes to end
	float NumRounds;
    //! Plunder
	Res Beute;
    //! Plunder from waves
	Res GesamtBeute;
    //! Number of wave attacks
	int NumAtts;
    //! Booty
	float Ausbeute;
    //! Resources on planet
	Res RessDa;
    //! Win of attacker when got all resources from debris
	Res GewinnMitTF;
    //! Win of attacker when got half of resources from debris
	Res GewinnMitHalfTF;
    //! Win of attacker when got no resources from debris
	Res GewinnOhneTF;
    //! Win of defender when got no resources from debris
	Res GewinnOhneTF_def;
    //! Win of defender when got half of resources from debris
	Res GewinnMitHalfTF_def;
    //! Win of defender when got all resources from debris
	Res GewinnMitTF_def;
    //! Fuel consumption
	__int64 SpritVerbrauch;
    //! Flying time
    DWORD FlyTime;
    //! real needed capacity for plunder
    int iNeededCapacity;
    //! recycler fly time
    int RecFlyTime[MAX_PLAYERS_PER_TEAM];
    //! recyler fuel usage
    int RecFuel[MAX_PLAYERS_PER_TEAM];
    int TotalFuel;

    BattleResult()
    {
        memset(PlaniName, 0, 64);
        VerlusteAngreifer = VerlusteVerteidiger = VerlVertmitAufbau = GesVerlust = TF = Res();
        MaxTF = MinTF = GesTF = MaxVerlAtt = MaxVerlDef = MinVerlAtt = MinVerlDef = WertAtt = WertDef = Res();
        Beute = GesamtBeute = RessDa = GewinnMitTF = GewinnOhneTF = GewinnMitHalfTF = Res();
        GewinnOhneTF_def = GewinnMitTF_def = GewinnMitHalfTF_def = Res();
        AttWon = DefWon = Draw = 0;
        PercentInTFKris = PercentInTFMet = 0;
        NumRecs = MaxNumRecs = MinNumRecs = GesamtRecs = 0;
        NumRounds = NumAtts = Ausbeute = SpritVerbrauch = FlyTime = iNeededCapacity = 0;
        TotalFuel = 0;
        for(int i = 0; i < MAX_PLAYERS_PER_TEAM; i++)
        {
            RecFlyTime[i] = 0;
            RecFuel[i] = 0;
        }
    }

	/*void operator/=(float num)
	{
		VerlusteAngreifer /= num;
		VerlusteVerteidiger /= num;
		MaxVerlAtt /= num;
		MaxVerlDef /= num;
		MinVerlAtt /= num;
		MinVerlDef /= num;
		MaxTF /= num;
		TF /= num;
		NumRecs /= num;
		MaxNumRecs /= num;
		MinNumRecs /= num;
		NumRounds /= num;
		Beute /= num;
        AttWon /= num;
        DefWon /= num;
        Draw /= num;
		Ausbeute /= num;
	};*/
    void operator/=(int num)
	{
		VerlusteAngreifer /= num;
		VerlusteVerteidiger /= num;
		MaxVerlAtt /= num;
		MaxVerlDef /= num;
		MinVerlAtt /= num;
		MinVerlDef /= num;
		MaxTF /= num;
		TF /= num;
		NumRecs /= num;
		MaxNumRecs /= num;
		MinNumRecs /= num;
		NumRounds /= num;
		Beute /= num;
        AttWon /= num;
        DefWon /= num;
        Draw /= num;
		Ausbeute /= num;
	};
};

/*!
    \struct IPMBattleResult
    \brief This struct contains information about the battle result after an IPM combat
*/
struct SPEEDKERNEL_API IPMBattleResult
{
    //! Losses of attacker
    Res LossesAttacker;
    //! Losses of defender
    Res LossesDefender;
    //! Needed missiles to destroy whole defense
    int NeededMissiles;
    //! Number of remaining interplanitary missiles
    int NumIPM;
    //! Number of remaining anti-ballistic missiles
    int NumABM;
    //! Name of target planet
    TCHAR PlaniName[64];
    //! Target planet position
    PlaniPos Position;

    IPMBattleResult()
    {
        NeededMissiles = 0;
        NumIPM = 0;
        NumABM = 0;
    }
};

/*!
    \struct ShipTechs
    \brief Structure containing the technologies.
*/
struct SPEEDKERNEL_API ShipTechs {
    //! Weapon Technology
    int Weapon;
    //! Shielding Technology
    int Shield;
    //! Armour Technology
    int Armour;
    ShipTechs() { Weapon = 0; Shield = 0; Armour = 0; };
    ShipTechs(int w, int s, int a) { Weapon = w; Shield = s; Armour = a; };
};

/*!
    \struct WaveInfo
    \brief Data for wave attacks.
*/
struct SPEEDKERNEL_API WaveInfo
{
    //! All plundered resources
    Res TotalPlunder;
    //! Number of attacks
    int NumAtts;
    //! Number of recyclers needed after the waves
    int TotalRecs;
    //! Size of debris after attacks
    Res TotalDebris;
    //! Total own losses
    Res TotalLosses;
    //! all needed fuel
    int TotalFuel;

    WaveInfo() {
        TotalFuel = NumAtts = TotalRecs = 0;
        TotalPlunder = TotalDebris = TotalLosses = Res();
    }
};

/*!
    \enum ReportData
    \brief This enum describes how much information could be obtained from an espionage report
*/
enum ReportData {
    REPORT_ALL,     /*< got all information */
    REPORT_DEFENCE,
    REPORT_FLEET,
    REPORT_RES
};

/*!
    \struct TargetInfo
    \brief This structure contains all information about a target planet.
*/
struct SPEEDKERNEL_API TargetInfo
{
    //! Position of the target
    PlaniPos Pos;
    //! Target name
    TCHAR Name[64];
    //! fleet deployed there
    vector<SItem> Fleet;
    //! built defence
    vector<SItem> Defence;
    //! amount of ABMs
    int NumABM;
    //! resources
    Res Resources;
    //! current ShipTechs
    ShipTechs Techs;
    //! information available from the espionage report
    ReportData State;
    //! engines
    int Engines[3];
    //! fleet velocity in percent
    int FleetVelocity;

    TargetInfo()
    {
        Pos = PlaniPos();
        memset(&Name, 0, 64);
        memset(Engines, 0, 3 * sizeof(int));
        Resources = Res();
        Techs = ShipTechs();
        State = REPORT_RES;
        NumABM = 0;
        FleetVelocity = 100;
    }
    const TargetInfo& operator=(const TargetInfo& ti)
    {
        // necessary because else Fleets would not be copied correctly
        if(this == &ti)
            return *this;
        memcpy(Name, ti.Name, 64);
        Fleet = ti.Fleet;
        Defence = ti.Defence;
        Resources = ti.Resources;
        Techs = ti.Techs;
        State = ti.State;
        Pos = ti.Pos;
        NumABM = ti.NumABM;
        FleetVelocity = ti.FleetVelocity;
        return *this;
    }
};

//! Help structure to get parse order
struct ShipString
{
    genstring::size_type Length;
    ITEM_TYPE Item;
    // needed to get right sort order
    static bool IsLess(ShipString const& s1, ShipString const& s2)
    {
        return s1.Length > s2.Length;
    }
};

#endif
