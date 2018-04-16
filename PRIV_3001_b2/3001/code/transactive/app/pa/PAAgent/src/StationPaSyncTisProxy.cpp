/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StationPaSyncTisProxy.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <vector>
#include <map>

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "bus/pa/cachedmaps/src/TisCachedMaps.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/StationPaSyncTisProxy.h"


namespace TA_IRS_App
{

StationPaSyncTisProxy* StationPaSyncTisProxy::s_pInstance = NULL;
ACE_Mutex StationPaSyncTisProxy::s_mtxInstanceLock;

StationPaSyncTisProxy::StationPaSyncTisProxy()
{
}

StationPaSyncTisProxy::~StationPaSyncTisProxy()
{
}

StationPaSyncTisProxy& StationPaSyncTisProxy::getInstance()
{
    if ( NULL == s_pInstance )
    {
        ACE_Guard<ACE_Mutex> gudInstance(s_mtxInstanceLock);
        if ( NULL == s_pInstance )
        {
            s_pInstance = new StationPaSyncTisProxy();

            if ( NULL == s_pInstance )
            {
                TA_THROW( TA_Base_Core::TransactiveException("Instance of StationPaSyncTisProxy is not created") );
            }
        }
    }

	return *s_pInstance;
}

void StationPaSyncTisProxy::destoryInstance()
{
	if ( NULL == s_pInstance )
	{
		return;
	}

	delete s_pInstance;
	s_pInstance = NULL;
}

void StationPaSyncTisProxy::verifyTisParamters( const std::vector<unsigned long>& vecZoneKeys, const std::vector<unsigned long>& vecMsgKeys )
{
	FUNCTION_ENTRY( "verifyTisParamters" );

    if ( vecZoneKeys.empty() || vecMsgKeys.empty() )
    {
        TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Invalid tis messages and tis zones configuration"));
    }

	bool isTisMessageMapped = TA_IRS_Bus::TisCachedMaps::getInstance().areAllPaDvaMsgMapped( vecMsgKeys );
	bool isTisZoneMapped = TA_IRS_Bus::TisCachedMaps::getInstance().areAllPaZoneMapped( vecZoneKeys );
	
	if( !isTisMessageMapped || !isTisZoneMapped )
    {
		TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Invalid tis messages and tis zones configuration"));
	}

	FUNCTION_EXIT;
}

void StationPaSyncTisProxy::syncTisBroadcast( const std::vector<unsigned long>& vecZoneKeys, const std::vector<unsigned long>& vecMsgKeys,
	const unsigned long startTime, const unsigned long endTime, const std::string& strSessionId, int& globleState )
{
	FUNCTION_ENTRY( "syncTisBroadcast" );

	//fetch Tis Messages
    int nTisLibSection = 0;
    unsigned long ulTisLibVersion = 0u;
    unsigned short usTisMsgTag = 0u;
    fetchTisMsgParameters( *(vecMsgKeys.begin()),nTisLibSection, ulTisLibVersion, usTisMsgTag );

    //fetch Tis Zones
    TA_Base_Core::STISDestinationList lstTisDestinations;
    fetchTisZones( vecZoneKeys, lstTisDestinations );

	//send messages to TisAgent
	try
	{
		executeTisBroadcast( nTisLibSection, ulTisLibVersion, usTisMsgTag, startTime, endTime, lstTisDestinations, strSessionId );
	}
	catch (TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)
	{
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_TIS_AGENT;
	}
	catch (TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)
	{
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_TIS_AGENT;
	}
	catch (TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException&)
	{
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TIS_AGENT;
	}
	catch(TA_Base_Core::AccessDeniedException& )
	{
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TIS_AGENT;
	}
	catch(TA_Base_Core::ObjectResolutionException& )
	{
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TIS_AGENT;
	}
	catch( ... )
	{
		// Unexpected exception...
		globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TIS_AGENT;
	}

	FUNCTION_EXIT;
}

void StationPaSyncTisProxy::executeTisBroadcast( const int nTisLibSection, const unsigned long ulTisLibVersion, 
    const unsigned short usTisMsgTag,const unsigned long startTime, const unsigned long endTime, const TA_Base_Core::STISDestinationList& lstTisDestinations, const std::string& strSessionId )
{
    FUNCTION_ENTRY( "executeTisBroadcast" );

	// Tis priority should be 0 for predefined messages.
	CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), submitPredefinedDisplayRequest2, 
        ( lstTisDestinations, static_cast<TA_Base_Core::ELibrarySection>(nTisLibSection), ulTisLibVersion, usTisMsgTag, startTime, endTime, 0, strSessionId.c_str() ));

    FUNCTION_EXIT;
}

void StationPaSyncTisProxy::fetchTisMsgParameters( const unsigned long ulMsgKey, int& nTisLibSection, unsigned long& ulTisLibVersion, unsigned short& usTisMsgTag )
{
	FUNCTION_ENTRY( "fetchTisMsgParameters" );

	TA_IRS_Bus::TisMessages tisMessages = TA_IRS_Bus::TisCachedMaps::getInstance().getTisMsgsByPADvaMsgKey(ulMsgKey);
	if( tisMessages.empty() )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PasConfigurationException : invalid tis messages configuration" ); 
	}
    else
    {
        nTisLibSection = static_cast<int>(tisMessages.begin()->ulTisLibrarySection);
        ulTisLibVersion = tisMessages.begin()->ulTisLibraryVersion;
        usTisMsgTag = static_cast<unsigned short>(tisMessages.begin()->ulTisMessageTag);
    }

	FUNCTION_EXIT;
}

void StationPaSyncTisProxy::fetchTisZones( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Core::STISDestinationList& lstTisDestinations )
{
    FUNCTION_ENTRY( "fetchTisZones" );

	TA_IRS_Bus::TisZones tisZones;

	std::set<std::string> uniquePidZones;

	for(std::vector<unsigned long>::const_iterator itZoneLoop = vecZoneKeys.begin(); itZoneLoop != vecZoneKeys.end(); ++itZoneLoop)
	{
		TA_IRS_Bus::TisZones tisZones = TA_IRS_Bus::TisCachedMaps::getInstance().getTisZonesByPaZoneKey(*itZoneLoop);

		for ( std::list<TA_IRS_Bus::TisZone>::iterator itZoneLoop = tisZones.begin(); itZoneLoop != tisZones.end(); ++itZoneLoop )
		{
			uniquePidZones.insert( itZoneLoop->strEquipmentEntity );
		}
	}

	std::map<std::string, unsigned long> locationNameToPidCount;

    std::map<std::string, TA_Base_Core::STISDestination> tisCoverageMap;
	for ( std::set<std::string>::iterator itForPidZones = uniquePidZones.begin(); itForPidZones != uniquePidZones.end(); ++itForPidZones )
	{
		try
		{
			std::string locationName = "";
			std::string pidName = "";

            getEquipmentComponentAndVerify(*itForPidZones, locationName, pidName);

			unsigned long& locationCount = locationNameToPidCount[locationName];
			TA_Base_Core::STISDestination& locationDestination = tisCoverageMap[locationName];

			if (locationCount == 0)
			{
				// Initialise new structure
				locationDestination.station = locationName.c_str();
				// Not used
				locationDestination.levels.length(0);
				// The max possible size.  We will resize later
				locationDestination.pids.length(uniquePidZones.size());
			}
			locationDestination.pids[locationCount] = pidName.c_str();

			++locationCount;
		}
		catch (TA_Base_Core::InvalidPasConfigurationException& )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PasConfigurationException : invalid tis equipment entity configuration" );
		}
	}

	// Resize in case some Pid Zones were invalid
	std::map<std::string, unsigned long>::iterator itForLocationCount = locationNameToPidCount.begin();
	for (; itForLocationCount != locationNameToPidCount.end(); ++itForLocationCount)
	{
		const std::string& locationName = itForLocationCount->first;
		unsigned long locationCount = itForLocationCount->second;

		tisCoverageMap[locationName].pids.length( locationCount );
    }

    lstTisDestinations.length(tisCoverageMap.size());

    int nTisDestIndex = 0;
    for ( std::map<std::string, TA_Base_Core::STISDestination>::iterator itLoop = tisCoverageMap.begin(); itLoop != tisCoverageMap.end(); ++itLoop )
    {
        lstTisDestinations[nTisDestIndex] = itLoop->second;
        ++nTisDestIndex;
    }

	FUNCTION_EXIT;
}

void StationPaSyncTisProxy::getEquipmentComponentAndVerify( const std::string& equipmentName, std::string& locationName, std::string& pidName)
{
	std::vector<std::string> tokens;

	PasHelpers::getInstance()->tokenise(equipmentName, tokens, ".");

	if ( tokens.size()!= 4u )
	{
		TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
	}

    const char* const ExpectedTisSystem = "TIS";
	// Determine StationSystem
	if( 0 != tokens[1].compare(ExpectedTisSystem) )
	{
		TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
	}

	locationName = tokens[0];
	pidName = tokens[3];
}

} // namespace TA_IRS_App

