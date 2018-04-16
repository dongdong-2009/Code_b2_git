/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/GZL9/transactive/bus/bas/bas_agent_library/src/BasAgentProxy.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/02/27 17:12:34 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the bas agent.
  */
#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    void BasAgentProxy::executeStationMode( unsigned long subsystemKey, unsigned long stationMode, bool canOverrideStationMode, const std::string& sessionId )
    {
        FUNCTION_ENTRY("BasAgentProxy::executeStationMode");
        try
        {
            CORBA_CALL((*m_basAgentPtr), executeStationMode, (subsystemKey, stationMode, canOverrideStationMode, sessionId.c_str()));
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            TA_THROW(BasAgentInvalidException(ex.what()));
        }
        FUNCTION_EXIT;
    }

    void BasAgentProxy::executeTimeTable( unsigned long tableKey, const std::string& sessionId, bool isTodayTimeTable )
    {
        FUNCTION_ENTRY("BasAgentProxy::executeTimeTable");
        TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType;
        if(false == isTodayTimeTable)
        {
            timeTableType = TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_NextDay;
        }
        else
        {
            timeTableType = TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_Today;
        }

        try
        {
            CORBA_CALL((*m_basAgentPtr), executeTimeTable, (timeTableType, tableKey, sessionId.c_str()));
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            TA_THROW(BasAgentInvalidException(ex.what()));
        }
        FUNCTION_EXIT
    }
    

    BasAgentProxy::BasAgentProxy( const std::string& entityName, const bool isAgentEntity )
    {
        FUNCTION_ENTRY("BasAgentProxy");
        try
        {
            m_basAgentPtr = std::auto_ptr<BASAgentObject>(new BASAgentObject(entityName, isAgentEntity));
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Create Basagent Proxy fail for entity = %s", entityName.c_str());
        }
        FUNCTION_EXIT;
    }

    BasAgentProxy::~BasAgentProxy()
    {
        FUNCTION_ENTRY("~BasAgentProxy");
        FUNCTION_EXIT;
    }

}