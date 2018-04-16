#pragma once;

#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <vector>


class UniqueListItemCache 
{
public:
    static const unsigned int INVALID_ID;
    unsigned int getIdByItem(const std::string& item);
    std::string getItemById(unsigned int id);
    void insertItem(unsigned int id, const std::string& item);
    void insertItem(const ListItem& item);
    std::vector<ListItem> getAllItems();
    void clear();
	bool isItemInCache(unsigned int id);
	bool isItemInCache(const std::string& item);
private:
    typedef boost::multi_index::multi_index_container <
    ListItem,
    boost::multi_index::indexed_by <
    boost::multi_index::ordered_unique<boost::multi_index::member<ListItem, unsigned int, &ListItem::ID>>,
    boost::multi_index::ordered_unique<boost::multi_index::member<ListItem, std::string, &ListItem::Item>>
    >
    > Container;
    typedef boost::multi_index::nth_index<Container, 0>::type ById;
    typedef boost::multi_index::nth_index<Container, 1>::type ByItem;
    Container m_container;
};
