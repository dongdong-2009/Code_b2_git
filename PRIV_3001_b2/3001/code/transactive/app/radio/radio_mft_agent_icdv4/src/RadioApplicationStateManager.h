/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Manages basic application/agent based states. These are:
 * - initialisation
 * - operattion
 * 
 * @note The GUI has additional states to manage the configuration of resources,
 * however, this doesn't impact the agent(s).
 * 
 */

#ifndef RADIO_APPLICATION_STATE_MANAGER_H_INCLUDED
#define RADIO_APPLICATION_STATE_MANAGER_H_INCLUDED

#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class RadioApplicationStateManager
{

	public:
		
		RadioApplicationStateManager();
		~RadioApplicationStateManager();
	    

		ApplicationState getApState();

		void setApState(ApplicationState state);

	private:
		
		//Disabled Methods
		RadioApplicationStateManager(const RadioApplicationStateManager&);
		RadioApplicationStateManager& operator=(const RadioApplicationStateManager&);

	private:

		ApplicationState m_state;

		TA_Base_Core::ReEntrantThreadLockable m_lock;

};
#endif // !defined({679AA4F2-CE0F-4f01-A94A-FE3C6142318E})