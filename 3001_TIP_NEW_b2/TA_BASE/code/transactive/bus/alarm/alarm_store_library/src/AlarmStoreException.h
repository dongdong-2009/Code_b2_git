/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmStoreException.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Alarm store exceptions.
  *
  */

///////////////////////////////////////////////////////////
//  TrainInvalidParameterException.h
//  Implementation of the Class TrainInvalidParameterException
//  Created on:      28-Nov-2003 04:41:02 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_)
#define AlarmStoreException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
    /**
     * Base class for all train exceptions.
     *
     * @todo Add operator<< to facilitate output of message
     */
	class AlarmStoreIndexNotFoundException : public TA_Base_Core::TransactiveException 
	{
		public:
            AlarmStoreIndexNotFoundException() throw()  {};
            virtual ~AlarmStoreIndexNotFoundException() throw() {};
        private:
	};


}
#endif // !defined(AlarmStoreException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_)
