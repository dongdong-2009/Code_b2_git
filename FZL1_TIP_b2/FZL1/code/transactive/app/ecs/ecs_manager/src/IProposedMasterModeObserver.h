#if !defined(IProposedMasterModeObserver_BD4D02DC_D507_4348_BB80_2FB8725F20D5__INCLUDED_)
#define IProposedMasterModeObserver_BD4D02DC_D507_4348_BB80_2FB8725F20D5__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/IProposedMasterModeObserver.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Interface definition for objects interested in updates to the value and validity
  * (validity meaning the value is confirmed from its source) of the proposed and 
  * current Master Mode Values.
  *
  */

#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"

namespace TA_IRS_App
{
    class IProposedMasterModeObserver
    {

    public:

        /** 
          * proposedMasterMode1Updated
          *
          * Callback to notify of change in Proposed Master Mode 1 value (including
          * invalidation when communications with related Agent is lost)
          *
          * @param isValid  - true if value is confirmed value from agent, false otherwise.
          * @param value    - New Proposed Master Mode 1 value.
          */
        //virtual void proposedMasterMode1Updated( bool isValid, int value ) = 0;


        /** 
          * proposedMasterMode2Updated
          *
          * Callback to notify of change in Proposed Master Mode 2 value (including 
          * invalidation when communications with related Agent is lost)
          *
          * @param isValid  - true if value is confirmed value from agent, false otherwise.
          * @param value    - New Proposed Master Mode 2 value.
          */
        //virtual void proposedMasterMode2Updated( bool isValid, int value ) = 0;


        /** 
          * currentMasterModeUpdated
          *
          * Callback to notify of change in Current Master Mode value (including
          * invalidation when communications with related Agent is lost)
          *
          * @param isValid  - true if value is confirmed value from agent, false otherwise.
          * @param value    - New Current Master Mode value.
          */
        //virtual void currentMasterModeUpdated( bool isValid, int value ) = 0;

		virtual void proposedMasterModesUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* masterModes) = 0;

		virtual void masterModeExecutionUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* masterModeExecutionInfo) = 0;


        /** 
          * Destructor
          */
        virtual ~IProposedMasterModeObserver() {};
    };
} // namespace TA_IRS_App

#endif // !defined(IProposedMasterModeObserver_BD4D02DC_D507_4348_BB80_2FB8725F20D5__INCLUDED_)
