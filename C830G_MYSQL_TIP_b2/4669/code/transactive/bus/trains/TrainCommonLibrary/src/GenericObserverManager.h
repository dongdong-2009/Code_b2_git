/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(GenericObserverManager_H)
#define GenericObserverManager_H

#include <vector>
#include <algorithm>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    /**
     * A simple template that is used to manage observers.
     * @author adamr
     * @version 1.0
     * @created 13-Nov-2007 3:50:46 PM
     */
    template<typename ObserverType> class GenericObserverManager
    {

    protected:

        /**
         * A typedef for the list of observers used in the generic observer class.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:47 PM
         */
        typedef std::vector< ObserverType* > ObserverList;


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when the first observer is added, and can be overridden to perform
         * some specific operation (eg subscribe to messages)
         */
        virtual void firstObserverAdded()
        {
        }


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when last first observer is removed, and can be overridden to
         * perform some specific operation (eg unsubscribe to messages)
         */
        virtual void lastObserverRemoved()
        {
        }


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when any observer is added, and can be overridden to perform some specific operation (eg send latest state to observer)
         *
         * @param observer    The observer that was added
         */
        virtual void observerAdded( ObserverType* observer )
        {
        }


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when an observer is removed, and can be overridden to perform some specific operation
         *
         * @param observer    The observer that was removed
         */
        virtual void observerRemoved( ObserverType* observer )
        {
        }


    public:

        /**
         * constructor
         */
        GenericObserverManager(){}


        /**
         * destructor
         */
        virtual ~GenericObserverManager(){}


        /**
         * Adds an observer to the list of observers.
         * If this observer already exists, no change is made.
         * 
         * @param observer    The observer to add
         */
        void addObserver( ObserverType* observer )
        {
            // If the observer doesnt already exist, add it to the vector
            //
            // if this is the first observer, call firstObserverAdded
            //
            // the observer didnt previously exist, call observerAdded

            int size = 0;
            bool added = false;
    
            {
                TA_THREADGUARD( m_observerLock );
    
                typename ObserverList::iterator findIter = std::find(m_observers.begin(), m_observers.end(), observer );
    
                if ( findIter == m_observers.end() )
                {
                    m_observers.push_back( observer );
                    added = true;
                }
    
                size = m_observers.size();
            }
    
            if ( true == added )
            {
                if ( 1 == size )
                {
                    firstObserverAdded();
                }

                observerAdded( observer );
            }
        }


        /**
         * Removes an observer to the list of observers.
         * If this observer doesnt exist, no change is made.
         * 
         * @param observer    The observer to add
         */
        void removeObserver( ObserverType* observer )
        {
            // If the observer exists, remove it from the vector
            //
            // if the observer was removed, call observerRemoved
            //
            // if this is the last observer, call lastObserverRemoved

            int size = 0;
            bool removed = false;
    
            {
                TA_THREADGUARD( m_observerLock );
    
                typename ObserverList::iterator findIter = std::find( m_observers.begin(), m_observers.end(), observer );
    
                if ( findIter != m_observers.end() )
                {
                    m_observers.erase( findIter );
                    removed = true;
                }
    
                size = m_observers.size();
            }
    
            if ( true == removed )
            {
                observerRemoved( observer );

                if ( 0 == size )
                {
                    lastObserverRemoved();
                }
            }
        }


        /**
         * This will reset all observers of this object, so no observers exist.
         */
        void clearObservers()
        {
            // Clear all items from the vector

            ObserverList originalList;

            {
                TA_THREADGUARD( m_observerLock );
    
                m_observers.swap( originalList );
            }

            // notify callbacks of removal
            for ( typename ObserverList::iterator iter = originalList.begin();
                  iter != originalList.end(); ++iter )
            {
                observerRemoved( *iter );
            }

            if ( originalList.size() > 0 )
            {
                lastObserverRemoved();
            }
        }


        /**
         * This checks if this observer class has any registered observers
         *
         * @return true if there is at least one observer, false otherwise
         */
        bool hasObservers()
        {
            return ( false == m_observers.empty() );
        }


    protected:

        /**
         * Gets the current list of observers for this object so they can be notified.
         * 
         * @return the current observer list
         */
        ObserverList getObservers()
        {
            // return the vector of observers
            TA_THREADGUARD( m_observerLock );
            return m_observers;
        }

 
    protected:

        /**
         * The list of observers
         */
        ObserverList m_observers;

        /**
         * A lock for the observer list
         */
        TA_Base_Core::NonReEntrantThreadLockable m_observerLock;

    };

}

#endif // !defined(GenericObserverManager_H)
