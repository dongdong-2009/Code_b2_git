#pragma once;
#include "TisConfig.h"
#include <vector>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>

struct ListItem;

namespace TA_Base_App
{

class TisPredefinedMsgEditorHelper
{
public:
    enum {STATION, TRAIN};
    static const unsigned int INVALID_ID;
    TisPredefinedMsgEditorHelper(const int type);
    ~TisPredefinedMsgEditorHelper();
    std::vector<ListItem> getPredefinedMsgItems() const;
    TisConfig::PredefinedTisMessage getPredefinedMsgById(const unsigned int id) const;
    unsigned long getLibraryVersion() const;
    unsigned int getIdBySectionAndMessageId(const int section, const unsigned short msgId) const;
    TisConfig::PredefinedTisMessage getPredefinedMsgBySectionAndMessageId(const int section, const unsigned short msgId) const;
private:
    struct PredefinedMsgItem
    {
        unsigned int id;
        int section;
        unsigned short msgId;
        TisConfig::PredefinedTisMessage msg;
        PredefinedMsgItem() : id(INVALID_ID), section(0), msgId(0)
        {
        }
        PredefinedMsgItem(const unsigned int rid,
                          const int rsection,
                          const unsigned short rmsgId,
                          const TisConfig::PredefinedTisMessage& rmsg)
            : id(rid), section(rsection), msgId(rmsgId), msg(rmsg)
        {
        }
    };
    typedef boost::multi_index::member<PredefinedMsgItem, unsigned int, &PredefinedMsgItem::id> KeyItemId;
    typedef boost::multi_index::composite_key <
    PredefinedMsgItem,
    boost::multi_index::member<PredefinedMsgItem, int, &PredefinedMsgItem::section>,
    boost::multi_index::member<PredefinedMsgItem, unsigned short, &PredefinedMsgItem::msgId>
    > KeySecionAndMsgId;
    typedef boost::multi_index_container <
    PredefinedMsgItem,
    boost::multi_index::indexed_by <
    boost::multi_index::ordered_unique<KeyItemId>,
    boost::multi_index::ordered_unique<KeySecionAndMsgId>
    >
    > Container;

    typedef boost::multi_index::nth_index<Container, 1>::type IndexSectionAndMsgId;

    Container m_predefinedMsgItems;
    unsigned long m_libraryVersion;
};

}
