#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"

const unsigned int UniqueListItemCache::INVALID_ID = -1;

unsigned int UniqueListItemCache::getIdByItem(const std::string& item)
{
    ByItem& byItem = boost::multi_index::get<1>(m_container);
    ByItem::const_iterator it = byItem.find(item);
    if(byItem.end() != it)
    {
        return (*it).ID;
    }
    return INVALID_ID;
}

std::string UniqueListItemCache::getItemById(unsigned int id)
{
    ById& byId = m_container;
    ById::const_iterator it = byId.find(id);
    if(byId.end() != it)
    {
        return (*it).Item;
    }
    return "";
}

void UniqueListItemCache::insertItem(unsigned int id, const std::string& item)
{
    ListItem tmp = {id, item};
    m_container.insert(tmp);
}

void UniqueListItemCache::insertItem(const ListItem& item)
{
    m_container.insert(item);
}

std::vector<ListItem> UniqueListItemCache::getAllItems()
{
    std::vector<ListItem> tmp;
    for(Container::const_iterator it = m_container.begin(); it != m_container.end(); ++it)
    {
        tmp.push_back(*it);
    }
    return tmp;
}

void UniqueListItemCache::clear()
{
    m_container.clear();
}

bool UniqueListItemCache::isItemInCache(unsigned int id)
{
    ById& byId = m_container;
	return byId.end() != byId.find(id);
}

bool UniqueListItemCache::isItemInCache(const std::string& item)
{
    ByItem& byItem = boost::multi_index::get<1>(m_container);
	return byItem.end() != byItem.find(item);
}

