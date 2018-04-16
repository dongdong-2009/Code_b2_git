/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $Workfile:$
  * @author Jade Lee
  * @version $Revision: 1.1.2.2 $
  *
  * Last modification: $Modtime: $
  * Last modified by: $Author: bradleyc $
  * 
  * Provides a list control that automatically manages memory allocation
  *     / destruction of memory pointed to in GetItemData
  *
  */
  
#include "app/radio/radio_monitor_gui/src/stdafx.h"

namespace TA_IRS_App
{

/*
    #define TMP template <class StorageType, class BaseClass>\
                ManagedListCtrl<StorageType, BaseClass>

    
    BEGIN_MESSAGE_MAP(TMP, BaseClass)
	    //{{AFX_MSG_MAP(ManagedListCtrl)
        ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()
*/

    template <class StorageType, class BaseClass> 
    ManagedListCtrl<StorageType, BaseClass>::~ManagedListCtrl()
    {
        // Perform final cleanup
        deleteAllItemsInArray(m_allocated);
    }


    template <class StorageType, class BaseClass> 
    int ManagedListCtrl<StorageType, BaseClass>::InsertItemPtr(int nIndex, const std::string& text, const StorageType& data)
    {
        int nItem = CListCtrl::InsertItem(nIndex, text.c_str());
        StorageType* clonedDataPtr = new StorageType(data);

        // Must ensure we add to local allocations array first,
        // for the SetItemData call to work
        m_allocated.insert(std::set<StorageType*>::value_type(clonedDataPtr));

        SetItemData(nItem, reinterpret_cast<DWORD>(clonedDataPtr));
        return nItem;
    }


    template <class StorageType, class BaseClass> 
    const StorageType& ManagedListCtrl<StorageType, BaseClass>::GetItemDataRef(int nIndex) const
    {
        StorageType* data = reinterpret_cast<StorageType*>(GetItemData(nIndex));
        if (0 != data)
        {
            return *data;
        }
        else
        {
            throw TA_Base_Core::ValueNotSetException();
        }
    }


    template <class StorageType, class BaseClass> 
    void ManagedListCtrl<StorageType, BaseClass>::RefreshItemDataPtr(int nIndex,  const StorageType& updateData)
    {
        StorageType* data = reinterpret_cast<StorageType*>(GetItemData(nIndex));
        if (0 != data)
        {
            *data = updateData; 
        }
        else
        {
            throw TA_Base_Core::ValueNotSetException();
        }
    }


    template <class StorageType, class BaseClass> 
    BOOL ManagedListCtrl<StorageType, BaseClass>::SetItemData( int nItem, DWORD dwData )
    {
        StorageType* data = reinterpret_cast<StorageType*>(GetItemData(nItem));

        if (0 != data)
        {
            // This is where actual memory gets cleaned up - confirm in our allocations array
            std::set<StorageType*>::iterator itr = m_allocated.find(data);

            if (itr != m_allocated.end())
            {
                delete data;
                m_allocated.erase(itr);
            }
        }

        // Don't allow any SetItemData calls for pointers not in allocated array
        if (0 != dwData)
        {
            StorageType* newData = reinterpret_cast<StorageType*>(dwData);

            if (m_allocated.end() == m_allocated.find(newData))
            {
                TA_ASSERT(FALSE, "Setting invalid data pointer");
            }
        }

        return BaseClass::SetItemData(nItem, dwData);
    }


    template <class StorageType, class BaseClass> 
    BOOL ManagedListCtrl<StorageType, BaseClass>::DeleteAllItems()
    {
        // Destroy all memory
        for (int i = 0; i < GetItemCount(); i ++)
        {
            SetItemData(i, 0);
        }
        return BaseClass::DeleteAllItems();
    }


	template <class StorageType, class BaseClass> 
    BOOL ManagedListCtrl<StorageType, BaseClass>::DeleteItem(int nItem)
    {
        SetItemData(nItem, 0);
        return BaseClass::DeleteItem(nItem);
    }
   
}   // end namespace TA_App
