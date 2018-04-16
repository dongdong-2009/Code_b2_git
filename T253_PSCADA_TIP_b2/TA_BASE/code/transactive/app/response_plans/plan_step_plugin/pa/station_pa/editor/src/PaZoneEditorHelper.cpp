#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/PaZoneEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "core/data_access_interface/pa/src/IPaZone.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{
PaZoneEditorHelper::PaZoneEditorHelper()
{
    try
    {
        TA_Base_Core::IPaZones zones =
            TA_Base_Core::PaZoneAccessFactory::getInstance().getAllPaZones();
        BOOST_FOREACH(TA_Base_Core::IPaZone * i, zones)
        {
            m_paZoneMap.insert(std::make_pair(i->getKey(), i));
        }
    }
    catch(...)
    {
        // TODO
    }
}

PaZoneEditorHelper::~PaZoneEditorHelper()
{
    BOOST_FOREACH(PaZoneMap::value_type & i, m_paZoneMap)
    {
        delete i.second;
    }
}

std::vector<ListItem> PaZoneEditorHelper::getAllPaZoneItems()
{
    std::vector<ListItem> items;
    BOOST_FOREACH(PaZoneMap::value_type & i, m_paZoneMap)
    {
        TA_Base_Core::IPaZone* zone = i.second;
        ListItem tmp = {zone->getKey(), zone->getLabel()};
        items.push_back(tmp);
    }
    return items;
}

std::vector<ListItem> PaZoneEditorHelper::getPaZoneItemsByLocation(unsigned long locationKey)
{
    if(0 == locationKey)
    {
        return getAllPaZoneItems();
    }
    std::vector<ListItem> items;
    BOOST_FOREACH(PaZoneMap::value_type & i, m_paZoneMap)
    {
        TA_Base_Core::IPaZone* zone = i.second;
        if(zone->getKey() == locationKey)
        {
            ListItem tmp = {zone->getKey(), zone->getLabel()};
            items.push_back(tmp);
        }
    }
	return items;
}

TA_Base_Core::IPaZone* PaZoneEditorHelper::getPaZoneByKey(unsigned long key)
{
    if(0 != m_paZoneMap.count(key))
    {
        return m_paZoneMap[key];
    }
    return NULL;
}
}