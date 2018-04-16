/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PADispatcher.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is object is responsible for handling PA Dispatching.
  */

#ifndef _PA_DISPATCHER_H_
#define _PA_DISPATCHER_H_

#include "bus/ats/ats_agent/CommonATS/src/IDataDispatcher.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_IRS_Bus
{
	class PADispatcher : public IDataDispatcher
	{
	public:
		/**
		 * Constructor
		 */
		PADispatcher();
		/**
		 * Destructor
		 */
		virtual ~PADispatcher();

		/**
		 * sendDataUpdate
		 *
		 * Method to use to send Data update
		 * 
		 */
		void sendDataUpdate();
	private:
		/**
		 * Disable copy, and assignment constructor
		 */
		PADispatcher(const PADispatcher&);
		PADispatcher& operator= (const PADispatcher&);
	};
}
#endif