/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/cachedmaps/src/ICachedMapObserverRegistry.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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