#pragma once
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
        class SafedSortedList
        {
        public:

            SafedSortedList();

            virtual void setKey(const std::string& key) { m_key = key; }
            virtual const std::string& getKey() { return m_key; }
            virtual void setSortedListItem(const SortedListItem& item, unsigned long ttl = 0);
            virtual void setSortedListItems(const SortedListItemList& items, unsigned long ttl = 0);
            virtual SortedListItemList getByRange(const HighLow& minScore, const HighLow& maxScore);
            virtual void getByRange(const HighLow& minScore, const HighLow& maxScore, SortedListItemList& result);
            virtual SortedListItemList getAll();
            virtual void getAll(SortedListItemList& result);
            virtual void remove();

        protected:

            void removeExpired();

        protected:

            struct ScoreTag {};
            struct TtlTag {};

            struct ScoreValueTtl
            {
                ScoreValueTtl(HighLow s, const std::string& v, unsigned long t = 0)
                    : score(s),
                      value(v)
                {
                    ttl = t ? boost::chrono::steady_clock::now() + boost::chrono::seconds(t) : boost::chrono::steady_clock::time_point::max();
                }

                HighLow score;
                std::string value;
                boost::chrono::steady_clock::time_point ttl;
            };

            typedef boost::multi_index_container<ScoreValueTtl,
                    boost::multi_index::indexed_by<
                    boost::multi_index::ordered_unique<boost::multi_index::tag<ScoreTag>, boost::multi_index::member<ScoreValueTtl, HighLow, &ScoreValueTtl::score>>,
                    boost::multi_index::ordered_non_unique<boost::multi_index::tag<TtlTag>, boost::multi_index::member<ScoreValueTtl, boost::chrono::steady_clock::time_point, &ScoreValueTtl::ttl>>
                    >> ScoreValueTtlList;

            typedef ScoreValueTtlList::index<ScoreTag>::type ScoreValueTtlListByScore;
            typedef ScoreValueTtlList::index<TtlTag>::type ScoreValueTtlListByTtl;

            std::string m_key;
            ScoreValueTtlList m_list;
            ScoreValueTtlListByScore& m_listByScore;
            ScoreValueTtlListByTtl& m_listByTtl;
            boost::shared_mutex m_mutex;
        };
    }
}
