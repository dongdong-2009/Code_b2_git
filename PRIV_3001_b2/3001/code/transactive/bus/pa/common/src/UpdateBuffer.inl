/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $Workfile:$
  * @author Jade Lee
  * @version $Revision: 1.1.2.3 $
  * Last modification: $Modtime:$
  * Last modified by: $Author: bradleyc $
  * 
  * UpdateBuffer
  * Wraps a collection of Elememt objects
  *
  */

#include "core/exceptions/src/ValueNotSetException.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
    
    template <class UPDATE_ITEM>
    UpdateBuffer<UPDATE_ITEM>::~UpdateBuffer()
    {
        // Shouldn't rely on destructor to clear out this
        TA_ASSERT(m_map.size() == 0, "Allowing incorrect destruction sequence");
    
        // Perform some checks (that all queues for each of the window handles is empty)
        for (std::map<HWND, WindowIdQueue>::iterator queueItr = m_queuedUpdateIds.begin();
                    queueItr != m_queuedUpdateIds.end();
                    queueItr ++)
        {
            TA_ASSERT(0 == (*queueItr).second.size(), "Allowing incorrect destruction sequence");
        }
    }
    
    
    template <class UPDATE_ITEM>
    DWORD UpdateBuffer<UPDATE_ITEM>::postUpdate(const std::set<HWND>& hWnds, const UPDATE_ITEM& data)
    {
        TA_ASSERT(hWnds.size() > 0, "Nonzero set required");
    
        TA_Base_Core::ThreadGuard sync(m_lock);
    
        DWORD key = m_uniqueIdentifier;
        // Keep the unique identifier unique, increment it
        m_uniqueIdentifier ++;
    
        Element<UPDATE_ITEM> newElem(data, hWnds);
    
        // Insert the new data into the map
        if (!m_map.insert( 
                std::map<DWORD, WrappedUpdateItem>::value_type( key, newElem ) ).second)
        {
            // Insertion failed (key already present), our unique key mustn't be unique
            TA_THROW(TA_Base_Core::TransactiveException("UpdateBuffer:: Internal error generating unique key")); 
        }
    
        // Add relevant data to window update queues
        for (std::set<HWND>::const_iterator currentHwnd = hWnds.begin();
                currentHwnd != hWnds.end();
                currentHwnd ++)
        {
            std::map<HWND, WindowIdQueue>::iterator queueItr = m_queuedUpdateIds.find(*currentHwnd);
    
            // Obtain the queue array for this hWnd, or create a new one
            if (queueItr == m_queuedUpdateIds.end())
            {
                // Not in there currently - create new set with single element
                std::set<DWORD> idSet;
                if (!idSet.insert(key).second)
                {
                    TA_ASSERT(FALSE, "INTERNAL ERROR");
                }
    
                if (!m_queuedUpdateIds.insert(std::map<HWND, WindowIdQueue>::value_type(*currentHwnd, idSet)).second)
                {
                    TA_ASSERT(FALSE, "INTERNAL ERROR");
                }
            }
            else
            {       
                // Add in the new element to this hWnds existing queue
                if (!((*queueItr).second).insert(key).second)
                {
                    // This hWnd already has a matching id in this queue array, this should definitely not happen
                    TA_THROW(TA_Base_Core::TransactiveException("postUpdate: Matching id found in hWnd queue")); 
                }
            }
        }
    
        return key;
    }
    
    
    template <class UPDATE_ITEM>
    unsigned long UpdateBuffer<UPDATE_ITEM>::getNumberOfQueuedUpdates(HWND hWnd)
    {
        TA_Base_Core::ThreadGuard sync(m_lock);
    
        std::map<HWND, WindowIdQueue>::iterator idSet = m_queuedUpdateIds.find(hWnd);
    
        if (m_queuedUpdateIds.end() == idSet)
        {
            // A queue hasn't even been created, no queued updates present
            return false;
        }
    
        // Only return true if items exist in this hWnds queue
        return (*idSet).second.size();
    }
    
    
    template <class UPDATE_ITEM>
    LockedPacket<UPDATE_ITEM> UpdateBuffer<UPDATE_ITEM>::obtainLockedDataOnFirstItemInQueue(HWND hWnd, unsigned long* itemKey)
    {
        // Allocate new guard that can be passed into locked result
        std::auto_ptr<TA_Base_Core::ThreadGuard> guard( new TA_Base_Core::ThreadGuard(m_lock) );
    
        std::map<HWND, WindowIdQueue>::iterator idSet = m_queuedUpdateIds.find(hWnd);
    
        if (m_queuedUpdateIds.end() == idSet)
        {
            // No queue exists
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
    
        if (0 == (*idSet).second.size())
        {
            // No elements in the queue
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
    
        // Obtain key for item we're wanting to obtain a lock on
        unsigned long key = *(*idSet).second.begin();
    
        // Set the output key if required
        if (0 != itemKey)
        {
            *itemKey = key;
        }
		
		typedef UpdateItemMap::iterator UpdateItemMapIterator;
		UpdateItemMapIterator it = m_map.find(key);
    
        if (m_map.end() == it)
        {
            // This is more of an internal error
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
    
        // The function obtainMutableReference will make copies of data
        // as required to ensure the returned reference doesn't affect
        // other windows data (from called to fetchUpdate)
        UPDATE_ITEM& reference = ((*it).second).obtainMutableReference(hWnd);
    
        LockedPacket<UPDATE_ITEM> lockedData(guard, reference); 
    
        return lockedData;
    }
    
    
    template <class UPDATE_ITEM>
    unsigned int UpdateBuffer<UPDATE_ITEM>::clearQueuedUpdates(HWND hWnd)
    {
        unsigned int numCleared = 0;
        TA_Base_Core::ThreadGuard sync(m_lock);
    
        std::map<HWND, WindowIdQueue>::iterator idSet = m_queuedUpdateIds.find(hWnd);
    
        if (idSet == m_queuedUpdateIds.end())
        {
            // No queue set for this window
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
    
        // Clone the queued id's so when iterating we don't invalidate iterators
        std::set<DWORD> clonedQueuedIds = (*idSet).second;
    
        for (std::set<DWORD>::iterator itr = clonedQueuedIds.begin();
                itr != clonedQueuedIds.end();
                itr ++)
        {        
            // Calling this will dequeue the update
            fetchUpdate(*itr, hWnd);
            numCleared ++;
        }
    
        TA_ASSERT(0 == (*idSet).second.size(), "clearQueuedUpdates: Queue was not fully purged");
    
        return numCleared;
    }
    
    
    template <class UPDATE_ITEM>
    UPDATE_ITEM UpdateBuffer<UPDATE_ITEM>::fetchUpdate(DWORD id, HWND hWnd, bool bPeek)
    {
        TA_Base_Core::ThreadGuard sync(m_lock);
    
		typedef UpdateItemMap::iterator UpdateItemMapIterator;
        UpdateItemMapIterator it = m_map.find(id);
    
        if (it == m_map.end())
        {
            // No entry for specified ID
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
        
        try
        {
            UPDATE_ITEM result = ((*it).second).fetch(hWnd, bPeek);
    
            if (!bPeek)
            {
                // Now if not peeking, may need to erase iterator
                if (((*it).second).getRemainingReferences() == 0)
                {
                    // All references exhausted, may remove from map
                    m_map.erase(it);
                }
    
                // Need to remove from queue (since not peeking)
                removeFromQueue(id, hWnd);
            }
    
            return result;
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            throw;
        }
    }
    
    
    template <class UPDATE_ITEM>
    void UpdateBuffer<UPDATE_ITEM>::removeFromQueue(DWORD id, HWND hWnd)
    {
        // Obtain the queue array for this hWnd
        std::map<HWND, WindowIdQueue>::iterator queue = m_queuedUpdateIds.find(hWnd);
        
        // Add in the new element to this hWnds queue
        if (queue == m_queuedUpdateIds.end())
        {
            TA_THROW(TA_Base_Core::TransactiveException("removeFromQueue: Couldn't find hWnd queue")); 
        
        }
        
        // MUST use reference, to ensure we're not simply modifying the copied version!
        WindowIdQueue& existingQueue = (*queue).second;
        
        // We can remove this id from the queue, having now retrieved it
        if (existingQueue.end() != existingQueue.find(id))
        {
            existingQueue.erase(id);
        }
        else
        {
            // This update identifier doesn't match any store in windows queue 
            TA_THROW(TA_Base_Core::TransactiveException("removeFromQueue: Couldn't find id in hWnd queue")); 
        }   
    }

} // end namespace TA_IRS_Bus
    