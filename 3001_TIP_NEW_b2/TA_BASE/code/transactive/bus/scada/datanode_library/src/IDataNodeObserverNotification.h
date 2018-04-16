/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datanode_library/src/IDataNodeObserverNotification.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2014/07/11 18:24:37 $
  * Last modified by:  $Author: haijun.li $
  *
  *	This interface allows an instance of the class DataNode to notify
  * a registered observer its internal state changes.
  *
  */

#ifndef IDATANODE_OBSERVER_NOTIFICATION_H
#define IDATANODE_OBSERVER_NOTIFICATION_H


#include "bus/scada/common_library/src/CommonDefs.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class IDataNodeObserverNotification
	{
	public:
		virtual ~IDataNodeObserverNotification()
		{

		}
		/**
		 * processDataNodeUpdate
		 *
		 * Call back for processing of the new data node update details
		 *
		 * @param 	pkey			The pkey of the data node
		 * @param 	updateDetails	The new updates of the data node specified by the pkey
		 *
		 */
		virtual void processDataNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails ) = 0;

	};
}

#endif

