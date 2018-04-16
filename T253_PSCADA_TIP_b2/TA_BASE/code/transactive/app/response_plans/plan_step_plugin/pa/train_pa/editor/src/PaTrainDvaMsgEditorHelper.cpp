#include "app/response_plans/plan_step_plugin/pa/train_pa/editor/src/PaTrainDvaMsgEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{
PaTrainDvaMsgEditorHelper::PaTrainDvaMsgEditorHelper()
{
    try
    {
        TA_Base_Core::IPaTrainDvaMessages dvaMsgs = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getAllPaTrainDvaMessages();
        BOOST_FOREACH(TA_Base_Core::IPaTrainDvaMessage * i, dvaMsgs)
        {
            m_paTrainDvaMsgMap.insert(std::make_pair(i->getKey(), i));
        }
    }
    catch(...)
    {
        // TODO
    }
}

PaTrainDvaMsgEditorHelper::~PaTrainDvaMsgEditorHelper()
{
    BOOST_FOREACH(PaTrainDvaMsgMap::value_type & i, m_paTrainDvaMsgMap)
    {
        delete i.second;
    }
}

std::vector<ListItem> PaTrainDvaMsgEditorHelper::getAllPaTrainDvaMsgItems()
{
    std::vector<ListItem> items;
    BOOST_FOREACH(PaTrainDvaMsgMap::value_type & i, m_paTrainDvaMsgMap)
    {
        TA_Base_Core::IPaTrainDvaMessage* msg = i.second;
        ListItem tmp = {msg->getKey(), msg->getLabel()};
        items.push_back(tmp);
    }
    return items;
}

TA_Base_Core::IPaTrainDvaMessage* PaTrainDvaMsgEditorHelper::getPaTrainDvsMsgByKey(unsigned long key)
{
    if(0 != m_paTrainDvaMsgMap.count(key))
    {
	    return m_paTrainDvaMsgMap[key];
    }
    return NULL;
}
}