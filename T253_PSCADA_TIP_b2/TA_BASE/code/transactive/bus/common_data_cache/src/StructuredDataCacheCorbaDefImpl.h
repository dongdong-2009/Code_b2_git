#pragma once
#include "StructuredDataCacheImpl.h"
#include "bus/common_data_cache/IDL/src/CommonDataCacheCorbadef.h"
#include "core/corba/src/ServantBase.h"
#include <ace/Singleton.h>

namespace TA_Base_Bus
{
    namespace DataCache
    {
        class StructuredDataCacheCorbaDefImpl
            : public virtual POA_TA_Base_Bus::DataCacheCorbaDef::IStructuredDataCacheCorbaDef,
              public virtual TA_Base_Core::ServantBase
        {
        public:

            StructuredDataCacheCorbaDefImpl();
            ~StructuredDataCacheCorbaDefImpl();
            friend class ACE_Singleton<StructuredDataCacheCorbaDefImpl, ACE_Recursive_Thread_Mutex>;

            static StructuredDataCacheCorbaDefImpl& getInstance();
            static void removeInstance();
            IStructuredDataCache* getStructuredDataCache();

            virtual void activate();
            virtual void deactivate();

            //
            // key-value
            //

            virtual char* get(const char* key);
            virtual DataCacheCorbaDef::KeyValueSeq* gets(const DataCacheCorbaDef::KeySeq& keys);
            virtual DataCacheCorbaDef::KeyValueSeq* getAll();
            virtual void set(const char* key, const char* value, ::CORBA::ULong timeToLive = 0);
            virtual void sets(const DataCacheCorbaDef::KeyValueSeq& keyValues, ::CORBA::ULong timeToLive = 0);
            virtual void remove(const char* key);
            virtual void removes(const DataCacheCorbaDef::KeySeq& keys);

            //
            // sorted-list
            //

            virtual void setSortedListItem(const char* key, const DataCacheCorbaDef::SortedListItem& item, ::CORBA::ULong timeToLive = 0);
            virtual void setSortedList(const DataCacheCorbaDef::SortedList& list, ::CORBA::ULong timeToLive = 0);
            virtual void setSortedLists(const DataCacheCorbaDef::SortedListSeq& lists, ::CORBA::ULong timeToLive = 0);
            virtual DataCacheCorbaDef::SortedListItemSeq* getSortedList(const char* key);
            virtual DataCacheCorbaDef::SortedListSeq* getSortedLists(const DataCacheCorbaDef::KeySeq& keys);
            virtual DataCacheCorbaDef::SortedListItemSeq* getSortedListByFilter(const DataCacheCorbaDef::SortedListFilter& filter);
            virtual DataCacheCorbaDef::SortedListSeq* getSortedListsByFilter(const DataCacheCorbaDef::SortedListFilterSeq& filters);
            virtual DataCacheCorbaDef::SortedListSeq* getAllSortedList();
            virtual void removeSortedList(const char* key);
            virtual void removeSortedLists(const DataCacheCorbaDef::KeySeq& keys);

        public:

            static const std::string SERVANT_NAME;

        protected:

            StructuredDataCacheImpl m_cache;
        };
    }
}
