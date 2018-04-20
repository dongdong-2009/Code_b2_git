/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioDirectLines.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioDirectLines.cpp: implementation of the CRadioDirectLine class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/Resource.h"
#include "RadioDirectLines.h"

#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"

#include "bus/radio/RadioAgentCorbaDef_4669/IDL/src/IRadioCorbaDef.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"



const std::string CRadioDirectLines::USER      = "User";
const std::string CRadioDirectLines::STATION   = "Stations";
const std::string CRadioDirectLines::OCR       = "OCR";
const std::string CRadioDirectLines::DCR       = "DCR";
const std::string CRadioDirectLines::OTHER     = "Others";
const std::string CRadioDirectLines::HOTLINE   = "HotLines";

using TA_Base_Core::DebugUtil;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRadioDirectLines::CRadioDirectLines()
{
	//InitEntries();
}

CRadioDirectLines::~CRadioDirectLines()
{
}


void CRadioDirectLines::AddEntry(DirectLineVector& lines,std::string name,std::string label,unsigned int position)
{

    try
    {      
        int length = lines.size();
	    DirectLineEntry& entry = lines.at(position);
        entry.label = name;
        entry.number = label;
    }
    catch(std::out_of_range&)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Speed dial entry %s set for invalid position %d",
            name.c_str(),position);
    }
}

void CRadioDirectLines::InitVectors()
{
	int i;
	DirectLineEntry	entry;
    entry.label = "";
    entry.number = "";

	// User entries
	m_userEntries.clear();
	// Station entries
	m_stationsEntries.clear();
	// OCR entries
	m_OCREntries.clear();
	// DCR entries
	m_DCREntries.clear();
	// Other OCR / DCR entries
	m_otherEntries.clear();
	for (i = 0; i < 36; i++)
	{
		m_userEntries.push_back(entry);
		m_stationsEntries.push_back(entry);
		m_OCREntries.push_back(entry);
		m_DCREntries.push_back(entry);
		m_otherEntries.push_back(entry);
        m_hotLineEntries.push_back(entry);
	}

    m_hotLineEntries.resize(8);
}

void CRadioDirectLines::InitEntries()
{
    InitVectors();

    // Get the parent class and retrieve the reference to the readio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    // before preparing anything, need to get the name of the speed dial set for this user
//     std::string userSetName("");
//     try
//     {
// 		try{
// 			userSetName = manager->getRadioCorbaDef()->getUserSetName(
//             TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str(),TA_Base_Bus::IRadioCorbaDef::PROFILE);
// 		}//TD18706,jianghp, handle the operation mode
// 		catch (TA_Base_Core::OperationModeException &) 
// 		{
// 			manager->getRadioCorbaDef().clear();
// 			userSetName = manager->getRadioCorbaDef()->getUserSetName(
// 				TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str(),TA_Base_Bus::IRadioCorbaDef::PROFILE);
// 		}
//     }
//     SILENT_RM_CATCH_HANDLER("getUserSetName");
	std::string userSetName("User");
    std::string sSessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	
	//libo++
	char sProfileID[9] = {0};
	TA_Base_Bus::AuthenticationLibrary authLibrary;
    TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sSessionID,sSessionID);
	int nProfileId = sessionInfo.ProfileId[0];
	sprintf( sProfileID, "_%d", nProfileId );
	userSetName  +=sProfileID;
	//++libo


    std::vector<NamedVector> directLineVectors;
    directLineVectors.push_back(NamedVector(m_userEntries,userSetName));
    directLineVectors.push_back(NamedVector(m_stationsEntries,STATION));
    directLineVectors.push_back(NamedVector(m_OCREntries,OCR));
    directLineVectors.push_back(NamedVector(m_DCREntries,DCR));
    directLineVectors.push_back(NamedVector(m_otherEntries,OTHER));
    directLineVectors.push_back(NamedVector(m_hotLineEntries,HOTLINE));


    for (unsigned int vectorCount = 0;vectorCount < directLineVectors.size();vectorCount++)
    {

        TA_Base_Bus::IRadioCorbaDef::SpeedDialList_var directDialUsers;
        try
        {
            CORBA_CALL_RETURN( directDialUsers, manager->getRadioCorbaDef(), getSpeedDialSet, ( directLineVectors[vectorCount].name.c_str() ) );//limin
        }
        catch(CORBA::Exception&)
        {
            directDialUsers = new TA_Base_Bus::IRadioCorbaDef::SpeedDialList();
            directDialUsers->length(0);
        }
        catch(...)
        {
            directDialUsers = new TA_Base_Bus::IRadioCorbaDef::SpeedDialList();
            directDialUsers->length(0);
        }

        for (unsigned int i = 0;i < directDialUsers->length();i++)
        {
            AddEntry(directLineVectors[vectorCount].vector,std::string(directDialUsers[i].label),std::string(directDialUsers[i].TSI),directDialUsers[i].position);        
        }        
    }
}

bool CRadioDirectLines::GetDirectLines(std::string name, DirectLineVector& lines)
{
	if (name == "User" /* "UserAndyParker" */)
	{
		lines = m_userEntries;
		return true;
	}
	else if (name == "Stations")
	{
		lines = m_stationsEntries;
		return true;
	}
	else if (name == "OCR")
	{
		lines = m_OCREntries;
		return true;
	}
	else if (name == "DCR")
	{
		lines = m_DCREntries;
		return true;
	}
	else if (name == "Others")
	{
		lines = m_otherEntries;
		return true;
	}

	// Not found
	return false;
}

bool CRadioDirectLines::GetDirectHotLines(DirectLineVector& lines)
{
	// initialise the incoming array
	lines.clear();
    
	/*AddEntry(m_hotLineEntries,"Emerg_1","9999-123-111",0);
	AddEntry(m_hotLineEntries,"Emerg_2","9999-123-111",1);
	AddEntry(m_hotLineEntries,"Emerg_3","9999-123-111",2);
	AddEntry(m_hotLineEntries,"Emerg_4","9999-123-111",3);
	AddEntry(m_hotLineEntries,"Emerg_5","9999-123-111",4);
	AddEntry(m_hotLineEntries, "All Trains", "9999",5);
	AddEntry(m_hotLineEntries, "Inbound Trains", "9999",6);
	AddEntry(m_hotLineEntries, "Outbound Trains", "9999",7);*/

    lines = m_hotLineEntries;

	return true;
}

