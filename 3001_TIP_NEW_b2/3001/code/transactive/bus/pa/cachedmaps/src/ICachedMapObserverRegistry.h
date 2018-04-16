/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/ICachedMapObserverRegistry.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Simple interface for registering / registering ICachedMapObserver
  * objects
  *
  */

#ifndef _ICACHEDMAP_OBSERVER_REGISTRY_HEADER_
#define _ICACHEDMAP_OBSERVER_REGISTRY_HEADER_

#include "bus/pa/cachedmaps/src/ICachedMapObserver.h"

namespace TA_IRS_Bus
{

    class ICachedMapObserverRegistry
    {
    public:
        /**
         * registerObserver
         *
         * Adds a new observer that watches out for changes to the cached map
         *
         * @param ob the observer that is to receive notifications when a change
         *          takes place (database change notification)
         *
         * @pre the observer 'ob' hasn't already been registered, and is not null
         *
         * @see deRegisterObserver
         *
         */
        virtual void registerObserver(ICachedMapObserver* ob) = 0;
    
        /**
         * deRegisterObserver
         *
         * Removes the specified observer that was added with registerObserver
         *
         * @param ob the observer we want to remove
         *
         * @pre the specified observer had already been added with registerObserver
         *
         * @see registerObserver
         *
         */
        virtual void deRegisterObserver(ICachedMapObserver* ob) = 0;
    };

} // end namespace TA_IRS_Bus

#endif // _ICACHEDMAP_OBSERVER_REGISTRY_HEADER_