#pragma once
#include <string>
#include <vector>

namespace TA_Base_Bus
{
    namespace DataCache
    {
        //
        // key-value
        //

        typedef std::vector<std::string> KeyList;

        struct KeyValue
        {
            std::string key;
            std::string value;

            friend bool operator ==(const KeyValue& lhs, const KeyValue& rhs)
            {
                return (lhs.key == rhs.key) && (lhs.value == rhs.value);
            }
        };

        typedef std::vector<KeyValue> KeyValueList;

        //
        // sorted-list
        //

        struct HighLow
        {
            unsigned long high;
            unsigned long low;

            friend bool operator ==(const HighLow& lhs, const HighLow& rhs)
            {
                return (lhs.high == rhs.high) && (lhs.low == rhs.low);
            }

            friend bool operator <(const HighLow& lhs, const HighLow& rhs)
            {
                return (lhs.high * 1000 + lhs.low) < (rhs.high * 1000 + rhs.low);
            }
        };

        struct SortedListItem
        {
            HighLow score;
            std::string value;

            friend bool operator ==(const SortedListItem& lhs, const SortedListItem& rhs)
            {
                return (lhs.score == rhs.score) && (lhs.value == rhs.value);
            }
        };

        typedef std::vector<SortedListItem> SortedListItemList;

        struct SortedList
        {
            std::string key;
            SortedListItemList items;

            friend bool operator ==(const SortedList& lhs, const SortedList& rhs)
            {
                return (lhs.key == rhs.key) && (lhs.items == rhs.items);
            }
        };

        typedef std::vector<SortedList> SortedListList;

        struct SortedListFilter
        {
            std::string key;
            HighLow minScore;
            HighLow maxScore;

            friend bool operator ==(const SortedListFilter& lhs, const SortedListFilter& rhs)
            {
                return (lhs.key == rhs.key) && (lhs.minScore == rhs.minScore) && (lhs.maxScore == rhs.maxScore);
            }
        };

        typedef std::vector<SortedListFilter> SortedListFilterList;
    }
}
