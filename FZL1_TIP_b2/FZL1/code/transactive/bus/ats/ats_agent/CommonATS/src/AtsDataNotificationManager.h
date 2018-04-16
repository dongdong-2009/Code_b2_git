/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsDataNotificationManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for receiving Data notification and process the data based on the event receive.
  */

#ifndef _MODBUS_NOTIFICATION_HANDLER_H_
#define _MODBUS_NOTIFICATION_HANDLER_H_

#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/threads/src/QueueProcessor.h"

namespace TA_IRS_Bus
{
	class IDataUpdate;
}
namespace TA_IRS_Bus
{
	class AtsDataNotificationManager : public TA_Base_Core::QueueProcessor<TA_IRS_Bus::DataNotification>, 
									   public TA_IRS_Bus::IAtsDataNotification
	{
	public:
		/**
		 * Constructor
		 */
		AtsDataNotificationManager(TA_IRS_Bus::IDataUpdate* dataUpdate);

		/**
		 * Destructor
		 */
		virtual ~AtsDataNotificationManager();

		/**
		 * notifyAtsDataEvent
		 *
		 * Method to use to notify Ats Data Event
		 * 
		 * @param event : DataNotification type
		 */
		 void notifyAtsDataEvent(TA_IRS_Bus::DataNotificationPtr event);

		/**
		 * processEvent
		 *
		 * Method to use to process the Ats Event data
		 * 
		 * @param event : DataNotification type
		 */
		virtual void processEvent( TA_IRS_Bus::DataNotificationPtr newEvent );
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		AtsDataNotificationManager(const AtsDataNotificationManager&);
		AtsDataNotificationManager& operator= (const AtsDataNotificationManager);

		TA_IRS_Bus::IDataUpdate* m_dataUpdate;
	};
}
#endif