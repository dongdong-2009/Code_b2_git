#pragma once
#include "IStructuredDataCache.h"
#include "DataCacheTypeDef.h"
#include <boost/chrono.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace TA_Base_Bus
{
    namespace DataCache
    {
        class SafedKeyValueList
        {
        public:

            SafedKeyValueList();

            virtual std::string get(const std::string& key);
            virtual void get(const std::string& key, KeyValueList& result);
            virtual void getAll(KeyValueList& result);
            virtual void set(const std::string& key, const std::string& value, unsigned long ttl = 0);
            virtual void remove(const std::string& key);

        protected:

            void removeExpiredKeyValue();

            struct KeyTag {};
            struct TtlTag {};

            struct KeyValueTtl
            {
                KeyValueTtl(const std::string& k, const std::string& v, unsigned long t = 0)
                    : key(k),
                      value(v)
                {
                    ttl = t ? boost::chrono::steady_clock::now() + boost::chrono::seconds(t) : boost::chrono::steady_clock::time_point::max();
                }

                std::string key;
                std::string value;
                boost::chrono::steady_clock::time_point ttl;
            };

            typedef boost::multi_index_container<KeyValueTtl,
                    boost::multi_index::indexed_by<
                    boost::multi_index::ordered_unique<boost::multi_index::tag<KeyTag>, boost::multi_index::member<KeyValueTtl, std::string, &KeyValueTtl::key>>,
                    boost::multi_index::ordered_non_unique<boost::multi_index::tag<TtlTag>, boost::multi_index::member<KeyValueTtl, boost::chrono::steady_clock::time_point, &KeyValueTtl::ttl>>
                    >> KeyValueTtlList;

            typedef KeyValueTtlList::index<KeyTag>::type KeyValuTtlListByKey;
            typedef KeyValueTtlList::index<TtlTag>::type KeyValuTtlListByTtl;

            KeyValueTtlList m_list;
            boost::shared_mutex m_mutex;
            KeyValuTtlListByKey& m_listByKey;
            KeyValuTtlListByTtl& m_listByTtl;
        };
    }
}
