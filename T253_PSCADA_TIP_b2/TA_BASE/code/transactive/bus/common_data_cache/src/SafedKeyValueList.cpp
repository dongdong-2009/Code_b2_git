#include "SafedKeyValueList.h"
#include <boost/thread/lock_types.hpp>

typedef boost::unique_lock<boost::shared_mutex> ReadLock;
typedef boost::shared_lock<boost::shared_mutex> WriteLock;

namespace TA_Base_Bus
{
    namespace DataCache
    {
        SafedKeyValueList::SafedKeyValueList()
            : m_listByKey(m_list.get<KeyTag>()),
              m_listByTtl(m_list.get<TtlTag>())
        {
        }

        std::string SafedKeyValueList::get(const std::string& key)
        {
            removeExpiredKeyValue();
            ReadLock lock(m_mutex);
            auto it = m_listByKey.find(key);
            static const std::string empty;
            return it != m_listByKey.end() ? it->value : empty;
        }

        void SafedKeyValueList::get(const std::string& key, KeyValueList& result)
        {
            std::string v = get(key);

            if (v.size())
            {
                result.emplace_back(KeyValue{ key, v });
            }
        }

        void SafedKeyValueList::getAll(KeyValueList& result)
        {
            removeExpiredKeyValue();
            ReadLock lock(m_mutex);

            for (auto& keyValue : m_listByKey)
            {
                result.emplace_back(KeyValue{ keyValue.key, keyValue.value });
            }
        }

        void SafedKeyValueList::set(const std::string& key, const std::string& value, unsigned long ttl)
        {
            WriteLock lock(m_mutex);
            m_listByKey.erase(key);
            m_listByKey.emplace(KeyValueTtl(key, value, ttl));
        }

        void SafedKeyValueList::remove(const std::string& key)
        {
            WriteLock lock(m_mutex);
            m_listByKey.erase(key);
        }

        void SafedKeyValueList::removeExpiredKeyValue()
        {
            WriteLock lock(m_mutex);
            m_listByTtl.erase(m_listByTtl.begin(), m_listByTtl.upper_bound(boost::chrono::steady_clock::now()));
        }
    }
}
