#include "StructuredDataCacheImpl.h"

namespace TA_Base_Bus
{
    namespace DataCache
    {
        //
        // key-value
        //

        std::string StructuredDataCacheImpl::get(const std::string& key)
        {
            return m_keyValueListGroup.get(key).get(key);
        }

        KeyValueList StructuredDataCacheImpl::gets(const KeyList& keys)
        {
            KeyValueList result;

            for (const std::string& key : keys)
            {
                m_keyValueListGroup.get(key).get(key, result);
            }

            return result;
        }

        KeyValueList StructuredDataCacheImpl::getAll()
        {
            KeyValueList result;

            for (SafedKeyValueList& list : m_keyValueListGroup.getAll())
            {
                list.getAll(result);
            }

            return result;
        }

        void StructuredDataCacheImpl::set(const std::string& key, const std::string& value, unsigned long ttl)
        {
            m_keyValueListGroup.get(key).set(key, value, ttl);
        }

        void StructuredDataCacheImpl::sets(const KeyValueList& values, unsigned long ttl)
        {
            for (const KeyValue& kv : values)
            {
                m_keyValueListGroup.get(kv.key).set(kv.key, kv.value, ttl);
            }
        }

        void StructuredDataCacheImpl::remove(const std::string& key)
        {
            m_keyValueListGroup.get(key).remove(key);
        }

        void StructuredDataCacheImpl::removes(const KeyList& keys)
        {
            for (const std::string& key : keys)
            {
                m_keyValueListGroup.get(key).remove(key);
            }
        }

        //
        // sorted-list
        //

        void StructuredDataCacheImpl::setSortedListItem(const std::string& key, const SortedListItem& item, unsigned long ttl)
        {
            m_sortedListMapGroup.get(key).get(key).setSortedListItem(item, ttl);
        }

        void StructuredDataCacheImpl::setSortedList(const SortedList& list, unsigned long ttl)
        {
            m_sortedListMapGroup.get(list.key).get(list.key).setSortedListItems(list.items, ttl);
        }

        void StructuredDataCacheImpl::setSortedLists(const SortedListList& lists, unsigned long ttl)
        {
            for (const SortedList& list : lists)
            {
                m_sortedListMapGroup.get(list.key).get(list.key).setSortedListItems(list.items, ttl);
            }
        }

        SortedListItemList StructuredDataCacheImpl::getSortedList(const std::string& key)
        {
            return m_sortedListMapGroup.get(key).get(key).getAll();
        }

        SortedListList StructuredDataCacheImpl::getSortedLists(const KeyList& keys)
        {
            SortedListList result;

            for (const std::string& key : keys)
            {
                SortedList list;
                list.key = key;
                m_sortedListMapGroup.get(key).get(key).getAll(list.items);
                result.emplace_back(std::move(list));
            }

            return result;
        }

        SortedListItemList StructuredDataCacheImpl::getSortedListByFilter(const SortedListFilter& filter)
        {
            return m_sortedListMapGroup.get(filter.key).get(filter.key).getByRange(filter.minScore, filter.maxScore);
        }

        SortedListList StructuredDataCacheImpl::getSortedListsByFilter(const SortedListFilterList& filters)
        {
            SortedListList result;

            for (const SortedListFilter& filter : filters)
            {
                SortedList list;
                list.key = filter.key;
                m_sortedListMapGroup.get(filter.key).get(filter.key).getByRange(filter.minScore, filter.maxScore, list.items);
                result.emplace_back(std::move(list));
            }

            return result;
        }

        SortedListList StructuredDataCacheImpl::getAllSortedList()
        {
            SortedListList result;

            for (SafedSortedListMap& sortedListMap : m_sortedListMapGroup.getAll())
            {
                for (SafedSortedList* p : sortedListMap.getAll())
                {
                    SortedList list;
                    list.key = p->getKey();
                    p->getAll(list.items);
                    result.emplace_back(std::move(list));
                }
            }

            return result;
        }

        void StructuredDataCacheImpl::removeSortedList(const std::string& key)
        {
            m_sortedListMapGroup.get(key).get(key).remove();
        }

        void StructuredDataCacheImpl::removeSortedLists(const KeyList& keys)
        {
            for (const std::string& key : keys)
            {
                m_sortedListMapGroup.get(key).get(key).remove();
            }
        }
    }
}
