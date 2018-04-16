/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/PIDRightsManager.h $
* @author:  Wu Min Tao
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2009/12/15 19:31:45 $
* Last modified by:  $Author: grace.koh $
*
*/

#include "PidManagerDialog.h"
#include "PIDSelectionManager.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "core/data_access_interface/src/locationaccessfactory.h"
#include "core/data_access_interface/PidsManager/src/PidStationIdAccessFactory.h"
#include "boost/tokenizer.hpp"
#include "resource.h"


namespace
{
    std::vector<std::string> tokenizeString( std::string theString, const std::string& separatorList )
    {
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens( theString, sep );

        for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
        {
            parts.push_back( *tok_iter );
        }

        // if parts is empty, then this should return the entire string
        if ( parts.size() == 0 )
        {
            parts.push_back( theString );
        }

        return parts;
    }
}

namespace TA_IRS_App
{

const std::string PIDSelectionManager::UPTRACK = "UT01";
const std::string PIDSelectionManager::DOWNTRACK = "DT01";
const std::string PIDSelectionManager::CONC = "CONC01";
const std::string PIDSelectionManager::ALL = "???";
const std::string PIDSelectionManager::TRAIN = "TRN";
const std::string PIDSelectionManager::SELECT = "Select";
const std::string PIDSelectionManager::DESELECT = "Deselect";
const std::string PIDSelectionManager::TOKEN = ".";
std::map<std::string,unsigned long> PIDSelectionManager::m_locationNameStationIdMap;
std::vector<std::string> PIDSelectionManager::m_locationsName;

PIDSelectionManager* PIDSelectionManager::getInstance()
{
    static PIDSelectionManager* p = NULL;
    if ( NULL == p )
    {
        p = new PIDSelectionManager();
    }
    return p;
}

PIDSelectionManager::PIDSelectionManager(): m_isSelectTrain( false )
{
	// Get location information
	try
	{
		TA_IRS_Core::PidStationIdAccessFactory::StationIdToLocationKeyMap stationIdToLocationKeyMap =
			TA_IRS_Core::PidStationIdAccessFactory::getInstance().getStationIdToLocationKeyMap();
		for(TA_IRS_Core::PidStationIdAccessFactory::StationIdToLocationKeyMap::iterator it = stationIdToLocationKeyMap.begin();
			it != stationIdToLocationKeyMap.end();
			++it)
		{
			TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(it->second);
			m_locationsName.push_back(location->getName());
		    m_locationNameStationIdMap.insert(std::pair<std::string, unsigned long>(location->getName(), it->first));
			delete location;
		}
		m_pidsList.replace(m_locationsName.size(), 0, NULL);
	}
	catch(...)
	{
	    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Failed init station id location key map");
	}

}

TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& PIDSelectionManager::getPids()
{
    return m_pidsList;
}

unsigned long PIDSelectionManager::getStationIdFromName( std::string name )
{
	unsigned long stationId = 0;
	if(m_locationNameStationIdMap.find(name) != m_locationNameStationIdMap.end())
	{
        stationId = m_locationNameStationIdMap[name];
	}
	return stationId;
}


PIDSelectionManager::Euoperator PIDSelectionManager::parseString( std::string strOperate, std::string strValue )
{
    Euoperator ret = NONE;
    std::vector<std::string>& valueTokened = tokenizeString( strValue, TOKEN );
    if ( strOperate == SELECT )
    {
        if ( valueTokened.size() == 1 )
        {
            if ( valueTokened[0] == ALL )
                return SELECT_ALL;
            else
                return SELECT_STATION_ALL;
        }

		if (  valueTokened.size() == 2 )
		{
			if ( valueTokened[0] == TRAIN )
				return SELECT_TRAIN;
		}

        if ( valueTokened.size() == 3 )
        {
            if ( valueTokened[0] == ALL )
                return SELECT_TRACK_ALL;
            else
                return SELECT_STATION_ONE_TRACK;
        }
		return ret;
    }
    else if ( strOperate == DESELECT )
    {
        if ( valueTokened.size() == 1 )
        {
            if ( valueTokened[0] == ALL )
                return DESELECT_ALL;   
            else
                return DESELECT_STATION_ALL;
        }

		if (  valueTokened.size() == 2 )
		{
			if ( valueTokened[0] == TRAIN )
				return DESELECT_TRAIN;
		}

        if ( valueTokened.size() == 3 )
        {
            if ( valueTokened[0] == ALL )
                return DESELECT_TRACK_ALL;
            else
                return DESELECT_STATION_ONE_TRACK;
        }

		return ret;
    }
    else
        return ret;
}

void PIDSelectionManager::selectAll()
{
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestination tmpDest;
    tmpDest.concourse = true;
    tmpDest.downTrack = true;
    tmpDest.upTrack = true;

    m_pidsList.length( m_locationsName.size());
    for ( int i = 0; i < m_pidsList.length(); ++i )
    {
        tmpDest.station = getStationIdFromName( m_locationsName[i] );
        m_pidsList[i] = tmpDest;
    }
    m_isSelectTrain = true;

	setEnable();
}

void PIDSelectionManager::deselectAll()
{
    m_pidsList.length( 0 );
    m_isSelectTrain = false;

	setDisable();
}

void PIDSelectionManager::selectStationAll( std::string strStationName )
{
	if(m_locationNameStationIdMap.find(strStationName) == m_locationNameStationIdMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station %s is not configured", strStationName.c_str());
		return;
	}
	TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestination tmpDest;
	tmpDest.station = getStationIdFromName( strStationName );
	tmpDest.concourse = true;
	tmpDest.downTrack = true;
	tmpDest.upTrack = true;
    //if station is already in list
	for(int i = 0; i < m_pidsList.length(); ++i)
	{
		if(m_pidsList[i].station == tmpDest.station)
		{
			m_pidsList[i].concourse = true;
			m_pidsList[i].downTrack = true;
			m_pidsList[i].upTrack = true;
			return;
		}		
	}

	//else add new destination to list
	
    m_pidsList.length( m_pidsList.length() + 1 );
    m_pidsList[m_pidsList.length() - 1] = tmpDest;
	
	setEnable();
}

void PIDSelectionManager::deselectStationAll( std::string strStationName )
{
	if(m_locationNameStationIdMap.find(strStationName) == m_locationNameStationIdMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station %s is not configured", strStationName.c_str());
		return;
	}
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList tmpList(m_locationsName.size());
    unsigned long location = getStationIdFromName( strStationName );

    for ( int i = 0; i < m_pidsList.length(); ++i )
    {
        if ( m_pidsList[i].station != location )
        {
            tmpList.length( tmpList.length() + 1 );
            tmpList[tmpList.length() - 1] = m_pidsList[i];
        }
    }
    m_pidsList = tmpList;
	setDisable();
}

void PIDSelectionManager::selectStationOneTrack( std::string strStationName, std::string strTrack )
{
	if(m_locationNameStationIdMap.find(strStationName) == m_locationNameStationIdMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station %s is not configured", strStationName.c_str());
		return;
	}
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestination tmpDest;
    tmpDest.station = getStationIdFromName( strStationName );
    tmpDest.downTrack = false;
    tmpDest.upTrack = false;
    tmpDest.concourse = false;

    if ( strTrack == UPTRACK )
        tmpDest.upTrack = true;
    else if ( strTrack == DOWNTRACK )
        tmpDest.downTrack = true;
    else
        tmpDest.concourse = true;

    for ( int i = 0; i < m_pidsList.length(); ++i )
    {
        if ( m_pidsList[i].station == tmpDest.station )
        {
            m_pidsList[i].downTrack |= tmpDest.downTrack;
            m_pidsList[i].upTrack   |= tmpDest.upTrack;
            m_pidsList[i].concourse |= tmpDest.concourse;
            return;
        }
    }
    m_pidsList.length( m_pidsList.length() + 1 );
    m_pidsList[m_pidsList.length() - 1] = tmpDest;

	setEnable();
}

void PIDSelectionManager::deselectStationOneTrack( std::string strStationName, std::string strTrack )
{
	if(m_locationNameStationIdMap.find(strStationName) == m_locationNameStationIdMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station %s is not configured", strStationName.c_str());
		return;
	}
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestination tmpDest;
    tmpDest.station = getStationIdFromName( strStationName );
    tmpDest.downTrack = false;
    tmpDest.upTrack = false;
    tmpDest.concourse = false;

    if ( strTrack == UPTRACK )
        tmpDest.upTrack = true;
    else if ( strTrack == DOWNTRACK )
        tmpDest.downTrack = true;
    else
        tmpDest.concourse = true;

    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList tmpList(m_locationsName.size());

    for ( int i = 0; i < m_pidsList.length(); ++i )
    {
        if ( m_pidsList[i].station != tmpDest.station )
        {
            tmpList.length( tmpList.length() + 1 );
            tmpList[tmpList.length() - 1] = m_pidsList[i];
        }
        else
        {
            m_pidsList[i].downTrack ^= tmpDest.downTrack;
            m_pidsList[i].upTrack   ^= tmpDest.upTrack;
            m_pidsList[i].concourse ^= tmpDest.concourse;
            if ( m_pidsList[i].downTrack || m_pidsList[i].upTrack || m_pidsList[i].concourse )
            {
                tmpList.length( tmpList.length() + 1 );
                tmpList[tmpList.length() - 1] = m_pidsList[i];
            }
        }
    }
    m_pidsList = tmpList;
	setDisable();
}

void PIDSelectionManager::selectTrackAll( std::string strTrack )
{
    for ( int i = 0; i < m_locationsName.size(); ++i )
    {
        selectStationOneTrack( m_locationsName[i], strTrack );
    }
	setEnable();
}

void PIDSelectionManager::deselectTrackAll( std::string strTrack )
{
    for ( int i = 0; i < m_locationsName.size(); ++i )
    {
        deselectStationOneTrack( m_locationsName[i], strTrack );
    }
	setDisable();
}

void PIDSelectionManager::selectTrain()
{   
    m_isSelectTrain = true;
	setEnable();
}

void PIDSelectionManager::deselectTrain()
{	
    m_isSelectTrain = false;
	setDisable();
}

bool PIDSelectionManager::isTrainSelect()
{
    return m_isSelectTrain;
}

void PIDSelectionManager::setTrainSel( bool isSel )
{
    m_isSelectTrain = isSel;
}

void PIDSelectionManager::setEnable()
{
	CPidManagerDialog* pDlg = CPidManagerDialog::getPidDlgInstance();
	if(pDlg != NULL)
	{
		//if select any station or select train,and select msg
		if((m_pidsList.length()!= 0 || m_isSelectTrain) && pDlg->isSelectMsg())
		{			
				pDlg->GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(true);
		}
	}
}

void PIDSelectionManager::setDisable()
{
	CPidManagerDialog* pDlg = CPidManagerDialog::getPidDlgInstance();
	if(pDlg != NULL)
	{
		//if not select any station and not select train,or not select msg
		if(((m_pidsList.length() == 0) && (!m_isSelectTrain)) || (!pDlg->isSelectMsg()))
		{
				pDlg->GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(false);
		}
	}
}

TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList PIDSelectionManager::getAllStationsList()
{
	//Make sure a instance is constructed	
	TA_ASSERT(getInstance() != NULL, "None instance constructed");

	TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList tmpDestList(m_locationsName.size());
	TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestination tmpDest;
	tmpDest.concourse = true;
	tmpDest.downTrack = true;
	tmpDest.upTrack = true;

	tmpDestList.length(tmpDestList.maximum());

	for ( int i = 0; i < tmpDestList.length(); ++i )
	{
		tmpDest.station = getStationIdFromName(m_locationsName[i]);
		tmpDestList[i] = tmpDest;
	}
	return tmpDestList;
}

}