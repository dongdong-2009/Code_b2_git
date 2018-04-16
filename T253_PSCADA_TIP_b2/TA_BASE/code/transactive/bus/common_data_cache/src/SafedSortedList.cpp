#include "SafedSortedList.h"
#include <boost/thread/lock_types.hpp>

typedef boost::unique_lock<boost::shared_mutex> ReadLock;
typedef boost::shared_lock<boost::shared_mutex> WriteLock;

namespace TA_Base_Bus
{
    namespace DataCache
    {
        SafedSortedList::SafedSortedList()
            : m_listByScore(m_list.get<ScoreTag>()),
              m_listByTtl(m_list.get<TtlTag>())
        {
        }

        void SafedSortedList::setSortedListItem(const SortedListItem& item, unsigned long ttl)
        {
            WriteLock lock(m_mutex);
            m_listByScore.erase(item.score);
            m_listByScore.emplace(ScoreValueTtl(item.score, item.value, ttl));
        }

        void SafedSortedList::setSortedListItems(const SortedListItemList& items, unsigned long ttl)
        {
            WriteLock lock(m_mutex);

            for (const SortedListItem& item : items)
            {
                m_listByScore.erase(item.score);
                m_listByScore.emplace(ScoreValueTtl(item.score, item.value, ttl));
            }
        }

        SortedListItemList SafedSortedList::getByRange(const HighLow& minScore, const HighLow& maxScore)
        {
            SortedListItemList result;
            getByRange(minScore, maxScore, result);
            return result;
        }

        void SafedSortedList::getByRange(const HighLow& minScore, const HighLow& maxScore, SortedListItemList& result)
        {
            removeExpired();
            ReadLock lock(m_mutex);
            auto range = std::make_pair(m_listByScore.lower_bound(minScore), m_listByScore.upper_bound(maxScore));

            for (auto it = range.first; it != range.second; ++it)
            {
                result.emplace_back(SortedListItem{ it->score, it->value });
            }
        }

        SortedListItemList SafedSortedList::getAll()
        {
            SortedListItemList result;
            getAll(result);
            return result;
        }

        void SafedSortedList::getAll(SortedListItemList& result)
        {
            removeExpired();
            ReadLock lock(m_mutex);

            for (auto& item : m_listByScore)
            {
                result.emplace_back(SortedListItem{ item.score, item.value });
            }
        }

        void SafedSortedList::remove()
        {
            WriteLock lock(m_mutex);
            m_listByScore.clear();
        }

        void SafedSortedList::removeExpired()
        {
            WriteLock lock(m_mutex);
            m_listByTtl.erase(m_listByTtl.begin(), m_listByTtl.upper_bound(boost::chrono::steady_clock::now()));
        }
    }
}
