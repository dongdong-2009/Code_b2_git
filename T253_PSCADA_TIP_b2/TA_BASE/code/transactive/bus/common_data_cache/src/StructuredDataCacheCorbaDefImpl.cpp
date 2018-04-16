#include "StructuredDataCacheCorbaDefImpl.h"
#include "DataCacheTypeConverter.h"
#include "core/corba/src/CorbaUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    namespace DataCache
    {
        const std::string StructuredDataCacheCorbaDefImpl::SERVANT_NAME = "CommonDataCache";

        StructuredDataCacheCorbaDefImpl& StructuredDataCacheCorbaDefImpl::getInstance()
        {
            return *ACE_Unmanaged_Singleton<StructuredDataCacheCorbaDefImpl, ACE_Recursive_Thread_Mutex>::instance();
        }

        void StructuredDataCacheCorbaDefImpl::removeInstance()
        {
            ACE_Unmanaged_Singleton<StructuredDataCacheCorbaDefImpl, ACE_Recursive_Thread_Mutex>::close();
        }

        StructuredDataCacheCorbaDefImpl::StructuredDataCacheCorbaDefImpl()
        {
        }

        StructuredDataCacheCorbaDefImpl::~StructuredDataCacheCorbaDefImpl()
        {
            deactivate();
        }

        void StructuredDataCacheCorbaDefImpl::activate()
        {
            CorbaUtil::getInstance().setServantKey(this, SERVANT_NAME);
            activateServantWithName(SERVANT_NAME);
        }

        void StructuredDataCacheCorbaDefImpl::deactivate()
        {
            deactivateServant();
        }

        IStructuredDataCache* StructuredDataCacheCorbaDefImpl::getStructuredDataCache()
        {
            return &m_cache;
        }

        //
        // key-value
        //

        char* StructuredDataCacheCorbaDefImpl::get(const char* key)
        {
            return DUP(m_cache.get(key));
        }

        DataCacheCorbaDef::KeyValueSeq* StructuredDataCacheCorbaDefImpl::gets(const DataCacheCorbaDef::KeySeq& keys)
        {
            return DUP_CONVERT(m_cache.gets(CONVERT(keys)));
        }

        DataCacheCorbaDef::KeyValueSeq* StructuredDataCacheCorbaDefImpl::getAll()
        {
            return DUP_CONVERT(m_cache.getAll());
        }

        void StructuredDataCacheCorbaDefImpl::set(const char* key, const char* value, ::CORBA::ULong timeToLive)
        {
            m_cache.set(key, value, timeToLive);
        }

        void StructuredDataCacheCorbaDefImpl::sets(const DataCacheCorbaDef::KeyValueSeq& keyValues, ::CORBA::ULong timeToLive)
        {
            m_cache.sets(CONVERT(keyValues), timeToLive);
        }

        void StructuredDataCacheCorbaDefImpl::remove(const char* key)
        {
            m_cache.remove(key);
        }

        void StructuredDataCacheCorbaDefImpl::removes(const DataCacheCorbaDef::KeySeq& keys)
        {
            m_cache.removes(CONVERT(keys));
        }

        //
        // sorted-list
        //

        void StructuredDataCacheCorbaDefImpl::setSortedListItem(const char* key, const DataCacheCorbaDef::SortedListItem& item, ::CORBA::ULong timeToLive)
        {
            m_cache.setSortedListItem(key, CONVERT(item), timeToLive);
        }

        void StructuredDataCacheCorbaDefImpl::setSortedList(const DataCacheCorbaDef::SortedList& list, ::CORBA::ULong timeToLive)
        {
            m_cache.setSortedList(CONVERT(list), timeToLive);
        }

        void StructuredDataCacheCorbaDefImpl::setSortedLists(const DataCacheCorbaDef::SortedListSeq& lists, ::CORBA::ULong timeToLive)
        {
            m_cache.setSortedLists(CONVERT(lists), timeToLive);
        }

        DataCacheCorbaDef::SortedListItemSeq* StructuredDataCacheCorbaDefImpl::getSortedList(const char* key)
        {
            return DUP_CONVERT(m_cache.getSortedList(key));
        }

        DataCacheCorbaDef::SortedListSeq* StructuredDataCacheCorbaDefImpl::getSortedLists(const DataCacheCorbaDef::KeySeq& keys)
        {
            return DUP_CONVERT(m_cache.getSortedLists(CONVERT(keys)));
        }

        DataCacheCorbaDef::SortedListItemSeq* StructuredDataCacheCorbaDefImpl::getSortedListByFilter(const DataCacheCorbaDef::SortedListFilter& filter)
        {
            return DUP_CONVERT(m_cache.getSortedListByFilter(CONVERT(filter)));
        }

        DataCacheCorbaDef::SortedListSeq* StructuredDataCacheCorbaDefImpl::getSortedListsByFilter(const DataCacheCorbaDef::SortedListFilterSeq& filters)
        {
            return DUP_CONVERT(m_cache.getSortedListsByFilter(CONVERT(filters)));
        }

        DataCacheCorbaDef::SortedListSeq* StructuredDataCacheCorbaDefImpl::getAllSortedList()
        {
            return DUP_CONVERT(m_cache.getAllSortedList());
        }

        void StructuredDataCacheCorbaDefImpl::removeSortedList(const char* key)
        {
            m_cache.removeSortedList(key);
        }

        void StructuredDataCacheCorbaDefImpl::removeSortedLists(const DataCacheCorbaDef::KeySeq& keys)
        {
            m_cache.removeSortedLists(CONVERT(keys));
        }
    }
}
