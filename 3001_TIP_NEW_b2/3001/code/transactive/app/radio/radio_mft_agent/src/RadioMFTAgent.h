/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgent.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * The RadioMFTAgent is the entry-point class for the outsourcer.  This class is
 * responsible for setting up the outsourced components to enable runtime
 * operations.
 * 
 * The deployment architecture for this agent is to run it as a library/component
 * withing the Radio Manager GUI. As such, there is no use of Generic Agent,
 * however, the MFT Agent must be written to facilitate subsequent separation from
 * the GUI, if required.
 * 
 */

#ifndef RADIO_MFT_AGENT_H_INCLUDED
#define RADIO_MFT_AGENT_H_INCLUDED


#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentCommandServant.h"

class RadioMFTAgent
{

	public:

		RadioMFTAgent();
		
		virtual ~RadioMFTAgent();

        void setServant(RadioMFTAgentCommandServant* pServant);
		
		/**
		 * Attempt to initialise the various API's and refresh state. 
		 */	
		void initialisation();

		/**
		 * Orderly shutdown of API connections.
		 */
		void termination();

		/**
		* This is not relevant for this MFT-based agent.
		* 
		* There is no peer agent, so by definition this agent is always in control mode.
		*/
		void agentSetMonitor();

		/**
		 * If the agent's entities implement CORBA interfaces, this method should take
		 * care of deactivating them.
		 * 
		 * @note If the class implementing this interface holds the GenericAgent as a
		 * member variable, DO NOT delete it inside this method. GenericAgent waits for
		 * this call to return before shutting down the ORB. Deleting the GenericAgent
		 * from within this method will cause obvious problems.
		 */
		void agentTerminate();

		/**
		 * This is not relevant for this MFT-based agent.
		 * 
		 * There is no peer agent, so by definition this agent is always in control mode.
		 */
		void agentSetControl();


	private:
        //Disabled Methods
        RadioMFTAgent(const RadioMFTAgent&);
        RadioMFTAgent& operator=(const RadioMFTAgent&);
		
		/**
		 * Initialize the current consoles radio session, and refresh the
		 * operator/profile's configuration.
		 */
		void operatorSessionInitialisation();

	private:

		IRadioMFTAgentCommand* m_pCommandImpl;

		RadioApplicationStateManager m_ApStateMgr;
};

#endif