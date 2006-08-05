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

// This file contains the functions for setting/getting the options in SpeedKernel


void CSpeedKernel::SetUseShipDataFromFile(bool bFileData) {
    m_ShipDataFromFile = bFileData;
}

bool CSpeedKernel::UsesShipDataFromFile() {
    return m_ShipDataFromFile;
}

void CSpeedKernel::RebuildSmallDefense(bool bRebuild) {
    m_RebuildSmallDef = bRebuild;
}

void CSpeedKernel::SetNewFuel(bool bNewFuel) {
    m_NewFuel = bNewFuel;
}

void CSpeedKernel::SetDefRebuildFactor(float RebuildFac) {
    m_DefRebuildFac = RebuildFac;
}

void CSpeedKernel::ResetWaveState()
{
    // ToDo
    SetTargetInfo(m_LastTarget, 0);
	/*m_Waves.TotalPlunder = m_Result.GesamtBeute = Res();
	m_Waves.NumAtts = m_Result.NumAtts = 0;
    m_Waves.TotalRecs = m_Result.GesamtRecs = 0;*/
}

void CSpeedKernel::SetCallBack(void (*f)(int sim, int round)) { 
    m_FuncPtr = f;
}

void CSpeedKernel::SetDefInDebris(bool really) { 
    m_DefInTF = really;
}

void CSpeedKernel::SetComputeBestWorstCase(bool how) { 
    m_CompBestWorstCase = how;
}

void CSpeedKernel::SetOwnPosition(const PlaniPos& p, int FleetID) { 
    if(FleetID >= 0 && FleetID <= MAX_PLAYERS_PER_TEAM)
        m_OwnPos[FleetID] = p;
}

void CSpeedKernel::GetOwnPosition(PlaniPos& p, int FleetID) {
    if(FleetID >= 0 && FleetID <= MAX_PLAYERS_PER_TEAM)
        p = m_OwnPos[FleetID];
}

void CSpeedKernel::SetSpeed(int perc_speed, int v, int i, int h, int FleetID) { 
    m_Speed[FleetID] = perc_speed / 10;
    m_TechsTW[FleetID][0] = v;
    m_TechsTW[FleetID][1] = i;
    m_TechsTW[FleetID][2] = h;
}

void CSpeedKernel::GetSpeed(int &perc_speed, int &v, int &i, int &h, int FleetID) {
    perc_speed = m_Speed[FleetID] * 10;
    v = m_TechsTW[FleetID][0];
    i = m_TechsTW[FleetID][1];
    h = m_TechsTW[FleetID][2];
}

void CSpeedKernel::SetNewShield(bool really) {
    m_NewShield = really;
}

void CSpeedKernel::SetRF(RFTYPE ver)
{
    switch(ver) {
    case RF_058:
        CanShootAgain = CanShootAgain_V058;
    	break;
    case RF_059:
        CanShootAgain = CanShootAgain_V059;
    	break;
    case RF_062:
        CanShootAgain = CanShootAgain_V062;
        break;
    case RF_065:
        CanShootAgain = CanShootAgain_V065;
        break;
    case RF_NONE:
        CanShootAgain = CantShootAgain;
        break;
    case RF_USER:
        CanShootAgain = CanShootAgain_User;
        return;
        break;
    default:
        CanShootAgain = CanShootAgain_V065;
        break;
    }
    CanShootAgain = CanShootAgain_FromTable;
    FillRFTable(ver);
}

void CSpeedKernel::SetTargetInfo(TargetInfo TI, int FleetID)
{
    size_t i;
    vector<SItem> items = TI.Fleet;
    // merge fleets
    for(i = 0; i < TI.Defence.size(); i++)
    {
        items.push_back(TI.Defence[i]);
    }
    
    // if there where no fleet unit, add "dummy unit" to delete defender
    if(items.size() == 0)
    {
        SItem sIt;
        sIt.Num = 0;
        sIt.Type = T_NONE;
        items.push_back(sIt);
    }
    // set correct fleet id
    for (i = 0; i < items.size(); i++)
    {
        items[i].OwnerID = FleetID;
    }
    // update kernel data
    SetFleet(NULL, &items);
    m_LastTarget = TI;
    m_NumShipsDef = items;
    m_TechsDef[FleetID] = TI.Techs;
    
    // reset wave state
    m_Waves.TotalPlunder = m_Result.GesamtBeute = Res();
	m_Waves.NumAtts = m_Result.NumAtts = 0;
    m_Waves.TotalRecs = m_Result.GesamtRecs = 0;
    m_Waves.TotalDebris = m_Result.GesTF = Res();
    m_Waves.TotalLosses = m_Result.GesVerlust = Res();

    _tcsncpy(m_Result.PlaniName, TI.Name, 63);
    m_Result.Position = TI.Pos;
    m_Result.RessDa = TI.Resources;
}

TargetInfo CSpeedKernel::GetTargetInfo()
{
    return m_LastTarget;
}

void CSpeedKernel::SetCSSFiles(TCHAR* cr_css, TCHAR* bwc_css)
{
    if(cr_css)
        m_CR_CSS = cr_css;
    else
        m_CR_CSS = _T("cr.css");
    if(bwc_css)
        m_BWC_CSS = bwc_css;
    else
        m_BWC_CSS = _T("bwc.css");
}
