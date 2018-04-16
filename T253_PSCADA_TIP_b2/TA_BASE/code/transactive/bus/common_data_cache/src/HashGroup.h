#pragma once
#include <boost/functional/hash.hpp>
#include <vector>

namespace TA_Base_Bus
{
    namespace DataCache
    {
        template <typename T, size_t size = 97>
        class HashGroup
        {
        public:

            HashGroup()
                : m_list(size)
            {
            }

            T& get(const std::string& key)
            {
                return m_list[m_hash(key) % size];
            }

            std::vector<T>& getAll()
            {
                return m_list;
            }

            std::vector<T> m_list;
            boost::hash<std::string> m_hash;
        };
    }
}
