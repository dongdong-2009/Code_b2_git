#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/MonitorEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/src/EntityTypeConstants.h"
#include <boost/foreach.hpp>
#include <map>

namespace TA_Base_App
{

const unsigned int MonitorEditorHelper::INVALID_ID = -1;
MonitorEditorHelper::MonitorItems MonitorEditorHelper::s_monitorItems;
bool MonitorEditorHelper::s_inited = false;

MonitorEditorHelper::MonitorEditorHelper()
{
}

MonitorEditorHelper::~MonitorEditorHelper()
{

}

std::vector<TreeItem> MonitorEditorHelper::getRootMonitorItems()
{
	initItems();
    return s_monitorItems.getRootItems();
}

std::vector<TreeItem> MonitorEditorHelper::getChildMonitorItems(unsigned int id)
{
	initItems();
    return s_monitorItems.getChildItems(id);
}

unsigned int MonitorEditorHelper::getItemIdByMonitorKey(unsigned int key)
{
	initItems();
    return s_monitorItems.getItemIdByUserData(key, Equal());
}

unsigned int MonitorEditorHelper::getMonitorKeyByItemId( unsigned int id )
{
	initItems();
	unsigned int* tmp = s_monitorItems.getUserDataByItemId(id);
	if(NULL != tmp)
	{
		return *tmp;
	}
	return INVALID_ID;
}

TreeItem MonitorEditorHelper::getMonitorTreeItemById( unsigned int id )
{
	initItems();
	return s_monitorItems.getTreeItem(id);
}

void MonitorEditorHelper::initItems()
{
	if(s_inited)
	{
		return;
	}
    TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputGroupVector outputGroups;
    try
    {
        outputGroups = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutputGroups();
    }
    catch(...)
    {
        // TODO
    }
    int id = 1;
    BOOST_FOREACH(TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputGroupNamedObject * i, outputGroups)
    {
        std::string groupType, groupName;
        // Get groups
        try
        {
            CORBA_CALL_RETURN(groupType, (*i), getGroupType, ());
        }
        catch(...)
        {
            continue;
        }
        if(TA_Base_App::TA_Visual::VIDEO_MONITOR != groupType)
        {
            continue;
        }

        // Get group name
        try
        {
            CORBA_CALL_RETURN(groupName, (*i), getName, ());
        }
        catch(...)
        {
            continue;
        }
        if(groupName.empty())
        {
            continue;
        }

        // Get video outputs
        TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence_var videoOutputs;
        try
        {
            CORBA_CALL_RETURN(videoOutputs, (*i), getVideoOutputList, ());
        }
        catch(...)
        {
            continue;
        }
        if(videoOutputs->length() == 0)
        {
            continue;
        }

        // Insert group
        unsigned int parentId = id;
        TreeItem item = {true, id, id, groupName};
        s_monitorItems.insertTreeItem(item, NULL, true);
		id++;

        for(unsigned int j = 0; j < videoOutputs->length(); ++j)
        {
            std::string monitorName;
            unsigned long monitorKey;
            try
            {
                monitorName = videoOutputs[j]->getName();
                monitorKey = videoOutputs[j]->getKey();
            }
            catch(...)
            {
                continue;
            }

            // Insert group
            TreeItem item = {false, id++, parentId, monitorName};
            s_monitorItems.insertTreeItem(item, new unsigned int(monitorKey), false);
        }

    }
	s_inited = true;
}

}

