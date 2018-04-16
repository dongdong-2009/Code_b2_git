#pragma once
#include "DataCacheTypeDef.h"

namespace TA_Base_Bus
{
    namespace DataCache
    {
        class IStructuredDataCache
        {
        public:

            virtual ~IStructuredDataCache() {}

            //
            // key-value
            //

            virtual std::string get(const std::string& key) = 0;
            virtual KeyValueList gets(const KeyList& keys) = 0;
            virtual KeyValueList getAll() = 0;
            virtual void set(const std::string& key, const std::string& value, unsigned long ttl = 0) = 0;
            virtual void sets(const KeyValueList& values, unsigned long ttl = 0) = 0;
            virtual void remove(const std::string& key) = 0;
            virtual void removes(const KeyList& keys) = 0;

            //
            // sorted-list
            //

            virtual void setSortedListItem(const std::string& key, const SortedListItem& item, unsigned long ttl = 0) = 0;
            virtual void setSortedList(const SortedList& sortedList, unsigned long ttl = 0) = 0;
            virtual void setSortedLists(const SortedListList& sortedLists, unsigned long ttl = 0) = 0;
            virtual SortedListItemList getSortedList(const std::string& key) = 0;
            virtual SortedListList getSortedLists(const KeyList& keys) = 0;
            virtual SortedListItemList getSortedListByFilter(const SortedListFilter& filter) = 0;
            virtual SortedListList getSortedListsByFilter(const SortedListFilterList& filters) = 0;
            virtual SortedListList getAllSortedList() = 0;
            virtual void removeSortedList(const std::string& key) = 0;
            virtual void removeSortedLists(const KeyList& keys) = 0;
        };
    }
}
