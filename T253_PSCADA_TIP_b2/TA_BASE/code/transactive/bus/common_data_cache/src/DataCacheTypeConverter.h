#pragma once
#include "bus/common_data_cache/IDL/src/CommonDataCacheCorbadef.h"
#include "DataCacheTypeDef.h"

namespace TA_Base_Bus
{
    class DataCacheTypeConverter
    {
    public:

        template <typename T>
        static T* dup(const T& x)
        {
            return new T(x);
        }

        static char* dup(const std::string& x)
        {
            return CORBA::string_dup(x.c_str());
        }

        template <typename CorbaSequence, typename StdVector>
        static CorbaSequence convertToCorbaSequence(const StdVector& vec)
        {
            CorbaSequence seq;
            seq.length(vec.size());

            for (size_t i = 0; i < vec.size(); ++i)
            {
                seq[i] = convert(vec[i]);
            }

            return seq;
        }

        template <typename StdVector, typename CorbaSequence>
        static StdVector convertToStdVector(const CorbaSequence& seq)
        {
            StdVector vec;
            vec.reserve(seq.length());

            for (size_t i = 0; i < seq.length(); ++i)
            {
                vec.emplace_back(convert(seq[i]));
            }

            return std::move(vec);
        }

        static CORBA::String_member convert(const std::string& str) { return str.c_str(); }
        static std::string convert(const CORBA::String_member& str) { return str.in(); };
        static std::string convert(const char* str) { return str; }

        //
        // key-value
        //

        static DataCacheCorbaDef::KeySeq convert(const DataCache::KeyList& keys);
        static DataCache::KeyList convert(const DataCacheCorbaDef::KeySeq& seq);

        static DataCacheCorbaDef::KeyValue convert(const DataCache::KeyValue& kv);
        static DataCache::KeyValue convert(const DataCacheCorbaDef::KeyValue& kv);

        static DataCacheCorbaDef::KeyValueSeq convert(const DataCache::KeyValueList& list);
        static DataCache::KeyValueList convert(const DataCacheCorbaDef::KeyValueSeq& seq);

        //
        // sorted-list
        //

        static DataCacheCorbaDef::HighLow convert(const DataCache::HighLow& score);
        static DataCache::HighLow convert(const DataCacheCorbaDef::HighLow& score);

        static DataCacheCorbaDef::SortedListItem convert(const DataCache::SortedListItem& item);
        static DataCache::SortedListItem convert(const DataCacheCorbaDef::SortedListItem& item);

        static DataCacheCorbaDef::SortedListItemSeq convert(const DataCache::SortedListItemList& list);
        static DataCache::SortedListItemList convert(const DataCacheCorbaDef::SortedListItemSeq& seq);

        static DataCacheCorbaDef::SortedList convert(const DataCache::SortedList& list);
        static DataCache::SortedList convert(const DataCacheCorbaDef::SortedList& list);

        static DataCacheCorbaDef::SortedListSeq convert(const DataCache::SortedListList& list);
        static DataCache::SortedListList convert(const DataCacheCorbaDef::SortedListSeq& seq);

        static DataCacheCorbaDef::SortedListFilter convert(const DataCache::SortedListFilter& filter);
        static DataCache::SortedListFilter convert(const DataCacheCorbaDef::SortedListFilter& filter);

        static DataCacheCorbaDef::SortedListFilterSeq convert(const DataCache::SortedListFilterList& filters);
        static DataCache::SortedListFilterList convert(const DataCacheCorbaDef::SortedListFilterSeq& filters);
    };
}

#define CONVERT TA_Base_Bus::DataCacheTypeConverter::convert
#define DUP(x) TA_Base_Bus::DataCacheTypeConverter::dup(x)
#define DUP_CONVERT(x) DUP(CONVERT(x))
