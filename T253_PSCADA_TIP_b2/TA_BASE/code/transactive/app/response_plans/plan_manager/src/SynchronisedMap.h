//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SynchronisedMap.h $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/12/15 15:23:59 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_SYNCHRONISEDMAP_H__843AA3F8_9B38_4CA9_8D80_BD08EB06E03E__INCLUDED_)
#define AFX_SYNCHRONISEDMAP_H__843AA3F8_9B38_4CA9_8D80_BD08EB06E03E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // SynchronisedMap
    // 
    template <typename KeyType, typename ItemType>
    class SynchronisedMap : public TA_Base_Core::NonReEntrantThreadLockable
    {
        // Operations
    public:
        SynchronisedMap();
        ~SynchronisedMap();

        bool empty();
        long size();
        void clear();
        void insert( const KeyType key, ItemType item );
        void erase( const KeyType &key );
        bool find( const KeyType &key );
        bool find( const KeyType &refInput, ItemType& refOutput );
        ItemType& operator[]( const KeyType &key );
        ItemType& front();
        ItemType& back();
        KeyType frontKey();
        KeyType backKey();
        void getKeys( std::vector<KeyType> &keys );
        void getItems( std::vector<ItemType> &items );

    private:
        SynchronisedMap( const SynchronisedMap<KeyType, ItemType> &synchronisedMap );
        SynchronisedMap &operator=(const SynchronisedMap<KeyType, ItemType> &synchronisedMap);

        // Attributes
    private:
        std::map<KeyType, ItemType> m_map;
    };

} // namespace TA_Base_App

#include "SynchronisedMap.inl"

#endif // !defined(AFX_SYNCHRONISEDMAP_H__843AA3F8_9B38_4CA9_8D80_BD08EB06E03E__INCLUDED_)
