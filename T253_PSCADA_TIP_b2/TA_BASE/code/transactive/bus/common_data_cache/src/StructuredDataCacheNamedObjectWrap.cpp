#include "StructuredDataCacheNamedObjectWrap.h"
#include "StructuredDataCacheCorbaDefImpl.h"
#include "DataCacheTypeConverter.h"

namespace TA_Base_Bus
{
    namespace DataCache
    {
        StructuredDataCacheNamedObjectWrap::StructuredDataCacheNamedObjectWrap(const std::string& agentName)
            : m_object(agentName, StructuredDataCacheCorbaDefImpl::SERVANT_NAME)
        {
        }

        //
        // key-value
        //

        std::string StructuredDataCacheNamedObjectWrap::get(const std::string& key)
        {
            CORBA::String_var result;
            CORBACALL(result = m_object->get(key.c_str()), m_object);
            return result.in();
        }

        KeyValueList StructuredDataCacheNamedObjectWrap::gets(const KeyList& keys)
        {
            DataCacheCorbaDef::KeyValueSeq_var result;
            CORBACALL(result = m_object->gets(CONVERT(keys)), m_object);
            return CONVERT(result.in());
        }

        KeyValueList StructuredDataCacheNamedObjectWrap::getAll()
        {
            DataCacheCorbaDef::KeyValueSeq_var result;
            CORBACALL(result = m_object->getAll(), m_object);
            return CONVERT(result.in());
        }

        void StructuredDataCacheNamedObjectWrap::set(const std::string& key, const std::string& value, unsigned long ttl)
        {
            CORBACALL(m_object->set(key.c_str(), value.c_str(), ttl), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::sets(const KeyValueList& values, unsigned long ttl)
        {
            CORBACALL(m_object->sets(CONVERT(values), ttl), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::remove(const std::string& key)
        {
            CORBACALL(m_object->remove(key.c_str()), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::removes(const KeyList& keys)
        {
            CORBACALL(m_object->removes(CONVERT(keys)), m_object);
        }

        //
        // sorted-list
        //

        void StructuredDataCacheNamedObjectWrap::setSortedListItem(const std::string& key, const SortedListItem& item, unsigned long ttl)
        {
            CORBACALL(m_object->setSortedListItem(key.c_str(), CONVERT(item), ttl), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::setSortedList(const SortedList& sortedList, unsigned long ttl)
        {
            CORBACALL(m_object->setSortedList(CONVERT(sortedList), ttl), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::setSortedLists(const SortedListList& sortedLists, unsigned long ttl)
        {
            CORBACALL(m_object->setSortedLists(CONVERT(sortedLists), ttl), m_object);
        }

        SortedListItemList StructuredDataCacheNamedObjectWrap::getSortedList(const std::string& key)
        {
            DataCacheCorbaDef::SortedListItemSeq_var result;
            CORBACALL(result = m_object->getSortedList(key.c_str()), m_object);
            return CONVERT(result.in());
        }

        SortedListList StructuredDataCacheNamedObjectWrap::getSortedLists(const KeyList& keys)
        {
            DataCacheCorbaDef::SortedListSeq_var result;
            CORBACALL(result = m_object->getSortedLists(CONVERT(keys)), m_object);
            return CONVERT(result.in());
        }

        SortedListItemList StructuredDataCacheNamedObjectWrap::getSortedListByFilter(const SortedListFilter& filter)
        {
            DataCacheCorbaDef::SortedListItemSeq_var result;
            CORBACALL(result = m_object->getSortedListByFilter(CONVERT(filter)), m_object);
            return CONVERT(result.in());
        }

        SortedListList StructuredDataCacheNamedObjectWrap::getSortedListsByFilter(const SortedListFilterList& filters)
        {
            DataCacheCorbaDef::SortedListSeq_var result;
            CORBACALL(result = m_object->getSortedListsByFilter(CONVERT(filters)), m_object);
            return CONVERT(result.in());
        }

        SortedListList StructuredDataCacheNamedObjectWrap::getAllSortedList()
        {
            DataCacheCorbaDef::SortedListSeq_var result;
            CORBACALL(result = m_object->getAllSortedList(), m_object);
            return CONVERT(result.in());
        }

        void StructuredDataCacheNamedObjectWrap::removeSortedList(const std::string& key)
        {
            CORBACALL(m_object->removeSortedList(key.c_str()), m_object);
        }

        void StructuredDataCacheNamedObjectWrap::removeSortedLists(const KeyList& keys)
        {
            CORBACALL(m_object->removeSortedLists(CONVERT(keys)), m_object);
        }
    }
}
