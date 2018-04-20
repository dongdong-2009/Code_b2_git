/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/ICachedMapObserver.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Simple observer interface for Cached Map observers
  *
  */

#ifndef _ICACHEDMAP_OBSERVER_HEADER_
#define _ICACHEDMAP_OBSERVER_HEADER_


#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
    class ICachedMapObserver
    {
    public:
    
        /**
         * onCachedMapChanged
         *
         * Called whenever there is a change to an item in the cached
         *  maps, it is important that no registerObserver or registerObserver
         *  calls are made from within here (back to the associated Cached
         *  map class) - will result in an assert
         * The cached maps will already be up to date before this notification
         *  received, so user may query cached maps directly to fetch the update
         *
         * @param key the primary key of the record that has changed
         * 
         * @param changesMade false if we received an update notification, but
         *      didn't detect any changes in data (ie *appears* no change to database)
         *
         * @exception if there is some error responding to the change
         *             
         */
        virtual void onCachedMapChanged(unsigned long pKey, bool changesMade) = 0;
                        //throw (TA_Base_Core::TransactiveException) = 0;
    
    };

} // end namespace TA_IRS_Bus

#endif // _ICACHEDMAP_OBSERVER_HEADER_