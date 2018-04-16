#include "SafedSortedListMap.h"
#include <boost/thread/lock_types.hpp>

typedef boost::unique_lock<boost::shared_mutex> ReadLock;
typedef boost::shared_lock<boost::shared_mutex> WriteLock;

namespace TA_Base_Bus
{
    namespace DataCache
    {
        SafedSortedList& SafedSortedListMap::get(const std::string& key)
        {
            {
                ReadLock lock(m_mutex);
                std::map<std::string, SafedSortedList>::iterator it = m_map.find(key);

                if (it != m_map.end())
                {
                    return it->second;
                }
            }

            WriteLock lock(m_mutex);
            SafedSortedList& result = m_map[key];
            result.setKey(key);
            return result;
        }

        std::vector<SafedSortedList*> SafedSortedListMap::getAll()
        {
            ReadLock lock(m_mutex);
            std::vector<SafedSortedList*> result;
            result.reserve(m_map.size());

            for (auto& pair : m_map)
            {
                result.push_back(&pair.second);
            }

            return result;
        }
    }
}
