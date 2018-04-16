/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PIDSDispatcher.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is object is responsible for handling PIDS Dispatching.
  */

#ifndef _PIDS_DISPATCHER_H_
#define _PIDS_DISPATCHER_H_

#include "bus/ats/ats_agent/CommonATS/src/IDataDispatcher.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"


namespace TA_IRS_Bus
{
	class PIDSAgentInterface;
	class PIDSDispatcher : public IDataDispatcher
	{
	public:
		/**
		 * Constructor
		 */
		PIDSDispatcher();
		/**
		 * Destructor
		 */
		virtual ~PIDSDispatcher();
		/**
		 * sendDataUpdate
		 *
		 * Method to use to send Data update 
		 */
		void sendDataUpdate();
	private:
		/**
		 * Disable copy, and assignment constructor
		 */
		PIDSDispatcher(const PIDSDispatcher&);
		PIDSDispatcher& operator= (const PIDSDispatcher&);

		TA_IRS_Bus::PIDSAgentInterface* m_pidsInterface;
		/**
		 * initialize
		 *
		 * Method to use to initialize local object
		 */
		void initialize();

		/**
		 * deInitialize
		 *
		 * Method to use to de-initialize local object
		 */
		void deInitialize();
	};
	
}

#endif