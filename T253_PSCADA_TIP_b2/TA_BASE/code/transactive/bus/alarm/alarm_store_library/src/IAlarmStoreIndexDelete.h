/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/IAlarmStoreIndexDelete.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This interface is used to notify the AlarmStoreServant to delete an index
  * It is provided as an index to allow the AlarmStoreCallbackManager to callback
  * and use it.
  *
  */

///////////////////////////////////////////////////////////
//  IAlarmUpdate.h
//  Implementation of the Interface IAlarmUpdate
//  Created on:      24-Jan-2004 04:14:33 PM
///////////////////////////////////////////////////////////

#if !defined(IAlarmStoreIndexDelete__INCLUDED_)
#define IAlarmStoreIndexDelete__INCLUDED_

#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include <string>

/**
 * @version 1.0
 * @updated 24-Jan-2004 04:16:31 PM
 */

namespace TA_Base_Bus
{
   class IAlarmStoreIndexDelete
   {

	   public:

		/**
		 * New deleteIndex
		 * @param indexRef - reference to index to delete
		 * 
		 */
		virtual void deleteIndex(TA_Base_Core::IAlarmStoreCorbaDef::IndexReference indexRef)=0;

   };
};
#endif // !defined(IAlarmStoreIndexDelete__INCLUDED_)
