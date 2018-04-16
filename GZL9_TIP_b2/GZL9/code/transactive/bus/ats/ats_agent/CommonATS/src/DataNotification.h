/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is a base class for all data notification.
  * Specific Data object must extend and use this class.
  * E.g. PADataNotification, TrainDataNotification & PIDSDataNotification 
  */

#ifndef _DATA_NOTIFICATION_H_
#define _DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "boost/shared_ptr.hpp"
#include "core/types/public_types/Constants.h"

namespace TA_IRS_Bus
{
	class IDataUpdate;
	class DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		DataNotification(void* data);

		/**
		 * Destructor
		 */
		virtual ~DataNotification();

		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		virtual void updateAtsData() = 0;

		/**
		 * processAtsData
		 *
		 * Method that will process Ats Data
		 * 
		 */
		virtual void processAtsData() = 0;

		/**
		 * setDataUpdater
		 *
		 * Method that sets the data updater
		 * 
		 */
		void setDataUpdater(IDataUpdate* dataUpater);

		/**
		 * getData
		 *
		 * This method will return data base on in its specified data structure object
		 * 
		 * @return void*   : Data structure object
		 */
		void* getData();

		/**
		 * setDataQuality
		 *
		 * Sets the Data Block Quality
		 *
		 *@param quality : Data Block Quality
		 */
		void setDataQuality(const TA_Base_Core::EDataPointQuality& quality);

		/**
		 * getDataQuality
		 *
		 * Returns the Data Quality
		 */
		TA_Base_Core::EDataPointQuality getDataQuality();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		DataNotification();
		DataNotification(const DataNotification&);
		DataNotification& operator= (const DataNotification&);

		void* m_Data;
		TA_Base_Core::EDataPointQuality m_dataQuality;

	protected:
		IDataUpdate* m_dataUpdater;
	};

	typedef boost::shared_ptr< DataNotification > DataNotificationPtr;
}
#endif