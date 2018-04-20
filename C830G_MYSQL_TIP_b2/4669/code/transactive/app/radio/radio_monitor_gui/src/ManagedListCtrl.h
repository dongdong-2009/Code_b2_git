/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/ManagedListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides a list control that automatically manages memory allocation
  *     / destruction of memory pointed to in GetItemData
  *
  */

#pragma once

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

namespace TA_IRS_App
{
    template <class StorageType, class BaseClass = TA_Base_Bus::ListCtrlSelNoFocus>
    class ManagedListCtrl : public BaseClass
    {
    public:

        virtual ~ManagedListCtrl();

        /**
         * InsertItem
         *
         * Inserts item with the specified text, and allocates new memory for 'data' - setting
         *  its ItemData to relevant pointer object
         *
         */
        virtual int InsertItemPtr(int nIndex, const std::string& text, const StorageType& data);

        /**
         * RefreshItemDataPtr
         *
         * Provides functionality to refresh the data contained at the specified index
         *
         * @param nIndex the index whose data is to be refreshed
         *
         * @param updateData the new data for the specified item
         *
         * @exception ValueNotSetException if the item data at specified index
         *              does not correspond to a locally allocated object of 'StorageType'
         */
        virtual void RefreshItemDataPtr(int nIndex, const StorageType& updateData);

        /**
         * GetItemDataRef
         *
         * @return data associated with the specified element
         *
         * @exception ValueNotSetException if the item data at specified index
         *              does not correspond to a locally allocated object of 'StorageType'
         */
        const StorageType& GetItemDataRef(int nIndex) const;
        
        /**
         * SetItemData (virtual extension)
         *
         * Must intercept to clean up memory if changing item data pointer
         */
        virtual BOOL SetItemData( int nItem, DWORD dwData );

        /**
         * DeleteAllItems (virtual extension)
         *
         * Automatic memory management on destruction
         */
        virtual BOOL DeleteAllItems();

        /**
         * DeleteAllItems (virtual extension)
         *
         * Automatic memory management on destruction
         */
	    virtual BOOL DeleteItem(int nItem);

        typedef ManagedListCtrl<StorageType, BaseClass> ManagedListCtrl2;

    protected:
        
        
    private:

        // For added safety checking, maintain list of allocations we have made
        std::set<StorageType*>   m_allocated;
    };

}   // end namespace TA_IRS_App


#include "ManagedListCtrl.inl"
