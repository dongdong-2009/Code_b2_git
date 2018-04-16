#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/LocationEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

LocationEditorHelper::LocationEditorHelper()
{
    try
    {
		std::vector<TA_Base_Core::ILocation*> locations =
            TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        BOOST_FOREACH(TA_Base_Core::ILocation * i, locations)
        {
            m_locationMap.insert(std::make_pair(i->getKey(), i));
        }
    }
    catch(...)
    {
        // TODO
    }

}

LocationEditorHelper::~LocationEditorHelper()
{
    BOOST_FOREACH(LocationMap::value_type & i, m_locationMap)
    {
        delete i.second;
    }
}

std::vector<ListItem> LocationEditorHelper::getLocationItems()
{
    std::vector<ListItem> items;
    BOOST_FOREACH(LocationMap::value_type & i, m_locationMap)
    {
        TA_Base_Core::ILocation* location = i.second;
        ListItem tmp = {location->getKey(), location->getName()};
        items.push_back(tmp);
    }
    return items;
}

TA_Base_Core::ILocation* LocationEditorHelper::getLocationByKey(unsigned long locationKey)
{
    if(0 != m_locationMap.count(locationKey))
    {
        return m_locationMap[locationKey];
    }
    return NULL;
}

}