#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/PaDvaMsgEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "core/data_access_interface/pa/src/IPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{
PaDvaMsgEditorHelper::PaDvaMsgEditorHelper()
{
    try
    {
        TA_Base_Core::IPaDvaMessages dvaMsgs = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getAllPaDvaMessages();
        BOOST_FOREACH(TA_Base_Core::IPaDvaMessage * i, dvaMsgs)
        {
            m_paDvaMsgMap.insert(std::make_pair(i->getKey(), i));
        }
    }
    catch(...)
    {
        // TODO
    }
}

PaDvaMsgEditorHelper::~PaDvaMsgEditorHelper()
{
    BOOST_FOREACH(PaDvaMsgMap::value_type & i, m_paDvaMsgMap)
    {
        delete i.second;
    }
}

std::vector<ListItem> PaDvaMsgEditorHelper::getAllPaDvaMsgItems()
{
    std::vector<ListItem> items;
    BOOST_FOREACH(PaDvaMsgMap::value_type & i, m_paDvaMsgMap)
    {
        TA_Base_Core::IPaDvaMessage* msg = i.second;
        ListItem tmp = {msg->getKey(), msg->getLabel()};
        items.push_back(tmp);
    }
    return items;
}

std::vector<ListItem> PaDvaMsgEditorHelper::getPaDvaMsgItemsByLocation(unsigned long locationkey)
{
    if(0 == locationkey)
    {
        return getAllPaDvaMsgItems();
    }
    std::vector<ListItem> items;
    BOOST_FOREACH(PaDvaMsgMap::value_type & i, m_paDvaMsgMap)
    {
        TA_Base_Core::IPaDvaMessage* msg = i.second;
        if(msg->getLocationKey() == locationkey)
        {
            ListItem tmp = {msg->getKey(), msg->getLabel()};
            items.push_back(tmp);
        }
    }
    return items;
}

TA_Base_Core::IPaDvaMessage* PaDvaMsgEditorHelper::getPaDvsMsgByKey(unsigned long key)
{
    if(0 != m_paDvaMsgMap.count(key))
    {
	    return m_paDvaMsgMap[key];
    }
    return NULL;
}
}