#pragma once
#include "SafedSortedList.h"
#include <boost/thread/shared_mutex.hpp>
#include <map>

namespace TA_Base_Bus
{
    namespace DataCache
    {
        class SafedSortedListMap
        {
        public:

            SafedSortedList& get(const std::string& key);
            std::vector<SafedSortedList*> getAll();

        protected:

            std::map<std::string, SafedSortedList> m_map;
            boost::shared_mutex m_mutex;
        };
    }
}
