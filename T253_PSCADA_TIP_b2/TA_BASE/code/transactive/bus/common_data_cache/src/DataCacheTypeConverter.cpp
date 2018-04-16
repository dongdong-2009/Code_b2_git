#include "DataCacheTypeConverter.h"

namespace TA_Base_Bus
{
    //
    // key-value
    //

    DataCacheCorbaDef::KeySeq DataCacheTypeConverter::convert(const DataCache::KeyList& keys)
    {
        return convertToCorbaSequence<DataCacheCorbaDef::KeySeq>(keys);
    }

    DataCache::KeyList DataCacheTypeConverter::convert(const DataCacheCorbaDef::KeySeq& seq)
    {
        return convertToStdVector<DataCache::KeyList>(seq);
    }

    DataCacheCorbaDef::KeyValue DataCacheTypeConverter::convert(const DataCache::KeyValue& kv)
    {
        return { kv.key.c_str(), kv.value.c_str() };
    }

    DataCache::KeyValue DataCacheTypeConverter::convert(const DataCacheCorbaDef::KeyValue& kv)
    {
        return { kv.key.in(), kv.value.in() };
    }

    DataCacheCorbaDef::KeyValueSeq DataCacheTypeConverter::convert(const DataCache::KeyValueList& list)
    {
        return convertToCorbaSequence<DataCacheCorbaDef::KeyValueSeq>(list);
    }

    DataCache::KeyValueList DataCacheTypeConverter::convert(const DataCacheCorbaDef::KeyValueSeq& seq)
    {
        return convertToStdVector<DataCache::KeyValueList>(seq);
    }

    //
    // sorted-list
    //

    DataCacheCorbaDef::HighLow DataCacheTypeConverter::convert(const DataCache::HighLow& score)
    {
        return { score.high, score.low };
    }

    DataCache::HighLow DataCacheTypeConverter::convert(const DataCacheCorbaDef::HighLow& score)
    {
        return { score.high, score.low };
    }

    DataCacheCorbaDef::SortedListItem DataCacheTypeConverter::convert(const DataCache::SortedListItem& item)
    {
        return { convert(item.score), item.value.c_str() };
    }

    DataCache::SortedListItem DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedListItem& item)
    {
        return { convert(item.score), item.value.in() };
    }

    DataCacheCorbaDef::SortedListItemSeq DataCacheTypeConverter::convert(const DataCache::SortedListItemList& list)
    {
        return convertToCorbaSequence<DataCacheCorbaDef::SortedListItemSeq>(list);
    }

    DataCache::SortedListItemList DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedListItemSeq& seq)
    {
        return convertToStdVector<DataCache::SortedListItemList>(seq);
    }

    DataCacheCorbaDef::SortedList DataCacheTypeConverter::convert(const DataCache::SortedList& list)
    {
        return { list.key.c_str(), convert(list.items) };
    }

    DataCache::SortedList DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedList& list)
    {
        return { list.key.in(), convert(list.items) };
    }

    DataCacheCorbaDef::SortedListSeq DataCacheTypeConverter::convert(const DataCache::SortedListList& list)
    {
        return convertToCorbaSequence<DataCacheCorbaDef::SortedListSeq>(list);
    }

    DataCache::SortedListList DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedListSeq& seq)
    {
        return convertToStdVector<DataCache::SortedListList>(seq);
    }

    DataCacheCorbaDef::SortedListFilter DataCacheTypeConverter::convert(const DataCache::SortedListFilter& filter)
    {
        return { filter.key.c_str(), convert(filter.minScore), convert(filter.maxScore) };
    }

    DataCache::SortedListFilter DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedListFilter& filter)
    {
        return { filter.key.in(), convert(filter.minScore), convert(filter.maxScore) };
    }

    DataCacheCorbaDef::SortedListFilterSeq DataCacheTypeConverter::convert(const DataCache::SortedListFilterList& filters)
    {
        return convertToCorbaSequence<DataCacheCorbaDef::SortedListFilterSeq>(filters);
    }

    DataCache::SortedListFilterList DataCacheTypeConverter::convert(const DataCacheCorbaDef::SortedListFilterSeq& filters)
    {
        return convertToStdVector<DataCache::SortedListFilterList>(filters);
    }
}
