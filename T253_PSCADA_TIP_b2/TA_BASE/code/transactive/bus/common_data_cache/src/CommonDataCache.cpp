#include "CommonDataCache.h"
#include "StructuredDataCacheImpl.h"
#include "StructuredDataCacheNamedObjectWrap.h"
#include "StructuredDataCacheCorbaDefImpl.h"

namespace TA_Base_Bus
{
    namespace DataCache
    {
        CommonDataCache::CommonDataCache(const std::string& agentName)
            : m_agentName(agentName),
              m_cache(NULL)
        {
            if (m_agentName.empty())
            {
                m_cache = StructuredDataCacheCorbaDefImpl::getInstance().getStructuredDataCache();
            }
            else
            {
                m_cache = new StructuredDataCacheNamedObjectWrap(agentName);
            }
        }

        CommonDataCache::~CommonDataCache()
        {
            if (m_agentName.size())
            {
                delete m_cache;
            }

            m_cache = NULL;
        }

        void CommonDataCache::activate()
        {
            if (m_agentName.empty())
            {
                StructuredDataCacheCorbaDefImpl::getInstance().activate();
            }
        }

        void CommonDataCache::deactivate()
        {
            if (m_agentName.empty())
            {
                StructuredDataCacheCorbaDefImpl::getInstance().deactivate();
            }
        }

        //
        // key-value
        //

        std::string CommonDataCache::get(const std::string& key)
        {
            return m_cache->get(key);
        }

        KeyValueList CommonDataCache::gets(const KeyList& keys)
        {
            return m_cache->gets(keys);
        }

        KeyValueList CommonDataCache::getAll()
        {
            return m_cache->getAll();
        }

        void CommonDataCache::set(const std::string& key, const std::string& value, unsigned long ttl)
        {
            m_cache->set(key, value, ttl);
        }

        void CommonDataCache::sets(const KeyValueList& values, unsigned long ttl)
        {
            m_cache->sets(values, ttl);
        }

        void CommonDataCache::remove(const std::string& key)
        {
            m_cache->remove(key);
        }

        void CommonDataCache::removes(const KeyList& keys)
        {
            m_cache->removes(keys);
        }

        //
        // sorted-list
        //

        void CommonDataCache::setSortedListItem(const std::string& key, const SortedListItem& item, unsigned long ttl)
        {
            m_cache->setSortedListItem(key, item, ttl);
        }

        void CommonDataCache::setSortedList(const SortedList& sortedList, unsigned long ttl)
        {
            m_cache->setSortedList(sortedList, ttl);
        }

        void CommonDataCache::setSortedLists(const SortedListList& sortedLists, unsigned long ttl)
        {
            m_cache->setSortedLists(sortedLists, ttl);
        }

        SortedListItemList CommonDataCache::getSortedList(const std::string& key)
        {
            return m_cache->getSortedList(key);
        }

        SortedListList CommonDataCache::getSortedLists(const KeyList& keys)
        {
            return m_cache->getSortedLists(keys);
        }

        SortedListItemList CommonDataCache::getSortedListByFilter(const SortedListFilter& filter)
        {
            return m_cache->getSortedListByFilter(filter);
        }

        SortedListList CommonDataCache::getSortedListsByFilter(const SortedListFilterList& filters)
        {
            return m_cache->getSortedListsByFilter(filters);
        }

        SortedListList CommonDataCache::getAllSortedList()
        {
            return m_cache->getAllSortedList();
        }

        void CommonDataCache::removeSortedList(const std::string& key)
        {
            m_cache->removeSortedList(key);
        }

        void CommonDataCache::removeSortedLists(const KeyList& keys)
        {
            m_cache->removeSortedLists(keys);
        }
    }
}
