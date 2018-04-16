#pragma once
#include "IStructuredDataCache.h"
#include "DataCacheTypeDef.h"
#include "SafedSortedListMap.h"
#include "SafedKeyValueList.h"
#include "HashGroup.h"

namespace TA_Base_Bus
{
    namespace DataCache
    {
        class StructuredDataCacheImpl : public IStructuredDataCache
        {
        public:

            //
            // key-value
            //

            virtual std::string get(const std::string& key);
            virtual KeyValueList gets(const KeyList& keys);
            virtual KeyValueList getAll();
            virtual void set(const std::string& key, const std::string& value, unsigned long ttl = 0);
            virtual void sets(const KeyValueList& values, unsigned long ttl = 0);
            virtual void remove(const std::string& key);
            virtual void removes(const KeyList& keys);

            //
            // sorted-list
            //

            virtual void setSortedListItem(const std::string& key, const SortedListItem& item, unsigned long ttl = 0);
            virtual void setSortedList(const SortedList& sortedList, unsigned long ttl = 0);
            virtual void setSortedLists(const SortedListList& sortedLists, unsigned long ttl = 0);
            virtual SortedListItemList getSortedList(const std::string& key);
            virtual SortedListList getSortedLists(const KeyList& keys);
            virtual SortedListItemList getSortedListByFilter(const SortedListFilter& filter);
            virtual SortedListList getSortedListsByFilter(const SortedListFilterList& filters);
            virtual SortedListList getAllSortedList();
            virtual void removeSortedList(const std::string& key);
            virtual void removeSortedLists(const KeyList& keys);

        protected:

            HashGroup<SafedKeyValueList> m_keyValueListGroup;
            HashGroup<SafedSortedListMap> m_sortedListMapGroup;
        };
    }
}
