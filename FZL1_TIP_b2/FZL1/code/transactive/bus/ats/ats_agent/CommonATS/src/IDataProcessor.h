/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This interface will be used for registration and deregistraion purposes for Data dispatch and Data Syn object.
  */

#ifndef _I_DATA_PROCESSOR_H_
#define _I_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"

namespace TA_IRS_Bus
{
	class IDataDispatcher;
	class IDataSync;

	class IDataProcessor : public IDataUpdate
	{
	public:
		/**
		 * registerDataDispatch
		 *
		 * Method used to register Data Dispatcher
		 * 
		 * @param type         : Ats Message Type
		 * @param dataDispatch : Data Dispatcher object
		 */
		virtual void registerDataDispatch(AtsMessageType type, IDataDispatcher* dataDispatch) = 0;

		/**
		 * deRegisterDataDispatch
		 *
		 * Method used to de register Data Dispatch
		 * 
		 * @param type         : Ats Message Type
		 */
		virtual void deRegisterDataDispatch(AtsMessageType type) = 0;

		/**
		 * registerDataSynch
		 *
		 * Method used to register Data Sync
		 * 
		 * @param type         : Ats Message Type
		 * @param dataDispatch : Data Dispatcher object
		 */
		virtual void registerDataSynch(AtsMessageType type, IDataSync* dataSynch) = 0;

		/**
		 * deRegisterDataSynch
		 *
		 * Method used to register Data Dispatcher
		 * 
		 * @param type         : Ats Message Type
		 */
		virtual void deRegisterDataSynch(AtsMessageType type) = 0;

		virtual void registerAtsDataProcessor(AtsMessageType type, DataProcessorThread* dataProcess) = 0;
		virtual void deRegisterAtsDataProcessor(AtsMessageType type) = 0;
	};
}
#endif