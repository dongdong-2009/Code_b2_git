/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/pa/pa_agent_proxy/src/PaAgentProxy.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the pa agent interface.
  */
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/pa/pa_agent/IDL/src/IStationPaMethod.h"
#include "bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.h"
#include "bus/pa/pa_agent_proxy/src/PaAgentProxy.h"

namespace TA_Base_Bus
{
    PaAgentProxy::PaAgentProxy() 
    {
        FUNCTION_ENTRY( "PaAgentProxy" );

        FUNCTION_EXIT;
    }

    PaAgentProxy::~PaAgentProxy()
    {
	    FUNCTION_ENTRY( "~PaAgentProxy" );

        FUNCTION_EXIT;
    }

    void PaAgentProxy::broadcastSigTriggeredStationDva( unsigned long ulAgentLocationKey, unsigned long ulEventType, 
        unsigned long ulSrcStationLocationKey, unsigned long ulDestStationLocationKey, unsigned long ulDestPlatformId, 
        bool bIsLastTrain, bool bTrainInfoValid, time_t tmStartTime )
    {
        PaNamedObj& refPaAgent = PaAgentAccessFactory::getInstance().getPaAgentByLocationKey( ulAgentLocationKey );

        try
        {
            CORBA_CALL( refPaAgent, broadcastSigTriggeredStationDva, 
                ( ulEventType, ulSrcStationLocationKey, ulDestStationLocationKey, ulDestPlatformId, bIsLastTrain, bTrainInfoValid, tmStartTime ) );
        }
        catch (...)
        {
            // TA_Base_Bus::IPAAgentCorbaDef::PaErrorException
            // TA_Base_Core::OperationModeException
            throw;
        }
    }

    void PaAgentProxy::broadcastFasTriggeredStationDva( unsigned long ulAgentLocationKey, 
        unsigned long ulEventType, unsigned long ulSrcStationLocationKey )
    {
        PaNamedObj& refPaAgent = PaAgentAccessFactory::getInstance().getPaAgentByLocationKey( ulAgentLocationKey );

        try
        {
            CORBA_CALL( refPaAgent, broadcastFasTriggeredStationDva, ( ulEventType, ulSrcStationLocationKey ) );
        }
        catch (...)
        {
            // TA_Base_Bus::IPAAgentCorbaDef::PaErrorException
            // TA_Base_Core::OperationModeException
            throw;
        }
    }
} // end namespace TA_Base_Bus
