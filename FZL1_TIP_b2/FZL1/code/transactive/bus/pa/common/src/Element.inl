// Inline definitions..

namespace TA_IRS_Bus
{

    template <class ELEM_TYPE>
    Element<ELEM_TYPE>::Element(const ELEM_TYPE& data, 
                                const std::set<HWND>& hWnds)
    :
    m_hWnds(hWnds)
    {
        m_data.push_back(data);

        // Originally create a many to one relationship (all windows
        // pointing to the main data, at index zero)
        for (std::set<HWND>::const_iterator itr = hWnds.begin();
                itr != hWnds.end();
                itr ++)
        {
            m_map[*itr] = 0;
        }
    }

    template <class ELEM_TYPE>
    ELEM_TYPE Element<ELEM_TYPE>::fetch(HWND hWnd, bool bPeek)
    {     
        std::set<HWND>::iterator elem = m_hWnds.find(hWnd);
        if (elem == m_hWnds.end())
        {
            // The hWnd passed in doesn't belong to this update - ie the
            // update data has been no update set for this hWnd
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
        else
        {
            // Request accepted, erase item from list
            if (!bPeek)
            {
                m_hWnds.erase(elem);
            }
        }
    
        return getDataRef(hWnd);
    }


    template <class ELEM_TYPE>
    ELEM_TYPE& Element<ELEM_TYPE>::obtainMutableReference(HWND hWnd)
    {
        if (isSharingData(hWnd))
        {
            return splitAndCloneDataForWindow(hWnd);
        }
        else
        {
            return getDataRef(hWnd);
        }
    }


    template <class ELEM_TYPE>
    ELEM_TYPE& Element<ELEM_TYPE>::splitAndCloneDataForWindow(HWND hWnd)
    {
        TA_ASSERT(isSharingData(hWnd), "No need to split and clone data");

        ELEM_TYPE& currentSharedData = getDataRef(hWnd);

        unsigned int newDataIndex = m_data.size();
        m_data.push_back(currentSharedData);
    
        // Update the map with new index, getDataRef will now return unique reference
        // (unique to hWnd that is)
        m_map[hWnd] = newDataIndex;

        // Return the cloned data
        return getDataRef(hWnd);
    }


    template <class ELEM_TYPE>
    bool Element<ELEM_TYPE>::isSharingData(HWND hWnd)
    {
        std::map<HWND, unsigned long>::iterator searchItr = m_map.find(hWnd);

        if (searchItr == m_map.end())
        {
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }

        // If any other windows point have same data index, it's being shared
        for (std::map<HWND, unsigned long>::iterator itr = m_map.begin();
                itr != m_map.end();
                itr ++)
        {
            if (itr == searchItr)
            {
                // Don't consider ourselves
                continue;
            }

            if (searchItr->second == itr->second)
            {
                // Both reference same data
                return true;
            }
        }

        // All testing passed, must be no sharing
        return false;
    }


    template <class ELEM_TYPE>
    ELEM_TYPE& Element<ELEM_TYPE>::getDataRef(HWND hWnd)
    {
        std::map<HWND, unsigned long>::iterator itr = m_map.find(hWnd);
        if (itr == m_map.end())
        {
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }

        if (itr->second >= 0 && itr->second < m_data.size())
        {
            // Return reference to exact data
            return m_data[itr->second];
        }
        else
        {
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
    }

} // end namespace TA_IRS_Bus
