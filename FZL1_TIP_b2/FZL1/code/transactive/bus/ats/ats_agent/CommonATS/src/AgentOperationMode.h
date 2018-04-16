/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\AgentOperationMode.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for managing the Agent Operation mode
  */

#ifndef _AGENT_OPERATION_MODE_H_
#define _AGENT_OPERATION_MODE_H_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
	class AgentOperationMode
	{
	public:
		/**
          * getInstance
          *
          * Gets the one and only AgentOperationMode.
          *
          * @return     AgentOperationMode&
          *             The only AgentOperationMode.
          */

		static AgentOperationMode& getInstance();

        /**
          * ~AgentOperationMode
          *
          * Standard destructor.
          */

        virtual ~AgentOperationMode() {};
        
        /**
          * isInControlMode
          *
          * Returns true if the agent is in Control mode.
          *
          * @return     bool
          *             True if the agent is in Control mode.
          *             False otherwise - i.e. if the agent is in Monitor mode.
          */

        bool isInControlMode();
    
        /**
          * setToControlMode
          *
          * Stores that the agent is in Control mode.
          */

        void setToControlMode();

        /**
          * setToMonitorMode
          *
          * Stores that the agent is in Monitor mode.
          */

        void setToMonitorMode();

	private:

		/**
          * AgentOperationMode
          *
          * Standard constructor.  Declared private because this class is 
          * as singleton.  Use getInstance() to get access to the 
          * single instance of this class.
          */

        AgentOperationMode();

        //
        // Disable copy constructor and assignment operator.
        //

        AgentOperationMode( const AgentOperationMode& theAgentOperationMode );
        AgentOperationMode& operator=( const AgentOperationMode& theAgentOperationMode );

		//
		// The one and only instance of this class.
		//

		static AgentOperationMode* m_theClassInstance;
		
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        //
        // Flag indicating if the agent is in Control mode and
        // the lock to guard the flag from simultaneous thread access.
        //
        
        bool m_isInControlMode;        
        TA_Base_Core::ReEntrantThreadLockable m_modeLock;

	};
}
#endif