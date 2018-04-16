#if !defined(IProposedStationModeObserver_E1E1FC1D_05E9_451a_81E7_50B7C975038A__INCLUDED_)
#define IProposedStationModeObserver_E1E1FC1D_05E9_451a_81E7_50B7C975038A__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/IProposedStationModeObserver.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Interface definition for objects interested in updates to the value and validity
  * (validity meaning the value is confirmed from its source) of the proposed Station 
  * Mode Values.
  *
  */

#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"

#include <vector>

namespace TA_IRS_App
{
    class IProposedStationModeObserver
    {

    public:

        /** 
          * proposedStationModeUpdated
          *
          * Implementation of IProposedStationModeObserver. Callback to notify of change in
          * Proposed Station Mode value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Proposed Station Mode value.
          */
		virtual void proposedStationModeUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* masterModes ) = 0;

		virtual void overridingProposedStationModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes) = 0;

		virtual void stationModeExecutionUpdated(bool isValid, TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeExecutionInfo) = 0;

        /** 
          * Destructor
          */
        virtual ~IProposedStationModeObserver() {};
    };
} // namespace TA_IRS_App

#endif // !defined(IProposedStationModeObserver_E1E1FC1D_05E9_451a_81E7_50B7C975038A__INCLUDED_)
