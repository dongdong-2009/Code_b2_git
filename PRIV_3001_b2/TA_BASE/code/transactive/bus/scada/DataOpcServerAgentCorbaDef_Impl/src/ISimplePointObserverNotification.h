/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/IDataPointObserverNotification.h $
  * @author  HoaVu
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Description
  *
  */

#ifndef IPOINT_OBSERVER_NOTIFICATION_H
#define IPOINT_OBSERVER_NOTIFICATION_H


#include "bus/scada/common_library/src/CommonDefs.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class ISimplePointObserverNotification
	{
	public:
		virtual ~ISimplePointObserverNotification()
		{

		}
		/**
		 * processDataPointUpdate
		 *
		 * Call back for processing of the new data point update details
		 *
		 * @param 	pkey			The pkey of the data point
		 * @param 	updateDetails	The new updates of the data point specified by the pkey
		 *
		 */
		virtual void processSimplePointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails ) = 0;

	};
}

#endif

