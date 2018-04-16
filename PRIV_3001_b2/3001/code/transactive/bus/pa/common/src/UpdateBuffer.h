/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/common/src/UpdateBuffer.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * UpdateBuffer
  * Wraps a collection of Elememt objects
  *
  */


#if !defined(UpdateBuffer_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define UpdateBuffer_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include "bus/pa/common/src/Element.h"
#include "bus/pa/common/src/LockedPacket.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <map>
#include <iterator>

namespace TA_IRS_Bus
{
    template <class UPDATE_ITEM> 
    class UpdateBuffer
    {
    public:

        UpdateBuffer()
            : 
            m_uniqueIdentifier(0)     // Unique id starts at 0
            {}

        /**
         * ~UpdateBuffer (destructor)
         *
         * @pre there are no remaining updates in buffer
         *
         * As an internal check, user should clear all updates in buffer (and not
         *  rely on destructor)
         *
         */
        virtual ~UpdateBuffer();

        /**
         * PostUpdate
         * 
         * Adds a new update to the buffer
         *
         * @param id the identifying key for the data
         * 
         * @param hWnds the set of hWnds to be associated with the data
         * 
         * @param data the actual data to be stored
         *
         * @exception TransactiveException if an update for the specified id already exists (not a unique key)
         *
         * @return the key used to add this update to the map (used in subsequent calls to fetchUpdate)
         *
         */
        DWORD postUpdate(const std::set<HWND>& hWnds, const UPDATE_ITEM& data);


        /**
         * fetchUpdate
         * 
         * Retrieves an update from the buffer - clearing the corresponding hWnd element from internal set
         * 
         * @param id the identifying key for the data
         * 
         * @param hWnd the hWnd associated with the data
         * 
         * @param bPeek set to true if want to peek at the data, but not purge hWnd set
         * 
         * @exception ValueNotSetException there is no value matching the input id, hWnd pair
         *
         * @exception TransactiveException if there is an internal error
         *
         * @return the data set in a corresponding call to postUpdate
         *
         */
        UPDATE_ITEM fetchUpdate(DWORD id, HWND hWnd, bool bPeek = false);

        /**
         * clearQueuedUpdates
         *
         * Clears out any items that are contained somewhere in the maps
         *
         * @param hWnd the handle of the window for which to purge the queue
         *
         * @return the number of updates that were cleared for the specified window
         *
         * @exception ValueNotSetException if the specified hWnd does not have a queue - or
         *              internal error (one of the queued items elements is not set)
         *              Can use getNumberOfQueuedUpdates to check for present on queue
         *
         */
        unsigned int clearQueuedUpdates(HWND hWnd);

        /**
         * getNumberOfQueuedUpdates
         *
         * @param hWnd the handle of the window for which to purge the queue
         *
         * @return number of items in queue of unprocessed (unfetched) 
         *              updates in this buffer
         *
         */
        unsigned long getNumberOfQueuedUpdates(HWND hWnd);

        /**
         * obtainLockedDataOnFirstItemInQueue
         *
         * Obtains a lock on the data for the input window (for the first element
         *  in the queue of updates awaiting collection from the client, so the client
         *  code may safely modify the contents.  Not that this function
         *  returns a 'safe' copy of the data - any modifications made
         *  to the returned reference will not affect any other hWnds, 
         *  meaning any changes are strictly specified to this hWnd identifier
         *
         * @param hWnd the identifier of the window for which we want to obtain
         *          the locked data for
         *
         * @param itemKey (Optional) [OUT] the key (as used in fetchUpdate) of the data
         *              that has been returned
         *
         * @return a locked packet which we may modify, which will change
         *      the data returned in the next call to fetchUpdate for this
         *      window WITHOUT changing the data returned for any other windows
         *      (that may be sharing the same key value).  The key value depends
         *      on which key is contained in the first item in update queue.
         *
         * @exception ValueNotSetException if window unrecognized
         *
         */
        LockedPacket<UPDATE_ITEM> obtainLockedDataOnFirstItemInQueue(
                                                HWND hWnd,
                                                unsigned long* itemKey = 0);

    private:

        /**
         * removeFromQueue
         *
         * Removes the specified update information from update queue (which
         *  is maintained for cleanup purposes)
         *
         * @param id the identifying key for the data
         * 
         * @param hWnd the hWnd associated with the data
         *
         * @exception TransactiveException if the specified ID isn't found in the queue
         *              for the specified hWnd (removing invalid ID)
         *
         */
        void    removeFromQueue(DWORD id, HWND hWnd);

    
        typedef Element< UPDATE_ITEM > WrappedUpdateItem;
		typedef std::map< DWORD, WrappedUpdateItem > UpdateItemMap;
		//typedef std::map< DWORD, WrappedUpdateItem >::iterator UpdateItemMapIterator;

        // The lock on the map - allows multiple entries from same thread, blocks separate thread access
        TA_Base_Core::ReEntrantThreadLockable     m_lock;

        // Define a map from a key to an element container for specified UPDATE_ITEM
        std::map<DWORD, WrappedUpdateItem>    m_map;
        /// Unique identifier for new entries in the m_map
        DWORD   m_uniqueIdentifier;

        typedef std::set<DWORD> WindowIdQueue;
        // Queue of updates maintained by each map
        std::map<HWND, WindowIdQueue>  m_queuedUpdateIds;      
    };

} // end namespace TA_IRS_Bus

// Template class required inclusion of inline definition
#include "bus/pa/common/src/UpdateBuffer.inl"

#endif // !defined(UpdateBuffer_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
