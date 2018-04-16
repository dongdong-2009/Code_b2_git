#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/CameraEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include <boost/foreach.hpp>
#include <map>
	
namespace TA_Base_App
{

const unsigned int CameraEditorHelper::INVALID_ID = -1;
bool CameraEditorHelper::s_inited = false;
CameraEditorHelper::CameraItems CameraEditorHelper::s_cameraItems;

CameraEditorHelper::CameraEditorHelper()
{
}

CameraEditorHelper::~CameraEditorHelper()
{

}

std::vector<TreeItem> CameraEditorHelper::getRootCameraItems()
{
	initItems();
    return s_cameraItems.getRootItems();
}

std::vector<TreeItem> CameraEditorHelper::getChildCameraItems(unsigned int id)
{
	initItems();
    return s_cameraItems.getChildItems(id);
}

unsigned int CameraEditorHelper::getItemIdByCameraKey(unsigned int key)
{
	initItems();
    return s_cameraItems.getItemIdByUserData(key, Equal());
}

unsigned int CameraEditorHelper::getCameraKeyByItemId( unsigned int id )
{
	initItems();
	unsigned int* tmp = s_cameraItems.getUserDataByItemId(id);
	if(NULL != tmp)
	{
		return *tmp;
	}
	return INVALID_ID;
}

TreeItem CameraEditorHelper::getCameraTreeItemById( unsigned int id )
{
	initItems();
	return s_cameraItems.getTreeItem(id);
}

void CameraEditorHelper::initItems()
{
	if(s_inited)
	{
		return;
	}
    TA_Base_Bus::VideoSwitchAgentFactory::CameraVector cameras;
    try
    {
        cameras = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCameras();
    }
    catch(...)
    {
        // TODO
    }

    int id = 1;
    std::map<std::string, unsigned int> groupNameItemIdMap;
    std::string groupName, cameraName;
	unsigned int cameraKey;
    BOOST_FOREACH(TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject * i, cameras)
    {
        try
        {
			CORBA_CALL_RETURN(groupName, (*i), getVideoInputGroupName, ());
        }
        catch(...)
        {
            groupName.clear();
            // TODO
        }
        if(!groupName.empty())
        {
            TreeItem item = {true, id, id, groupName};
            s_cameraItems.insertTreeItem(item, NULL, true);
            groupNameItemIdMap.insert(std::make_pair(groupName, id));
            id++;
        }
        try
        {
            CORBA_CALL_RETURN(cameraName, (*i), getName, ());
            CORBA_CALL_RETURN(cameraKey, (*i), getKey, ());
        }
        catch(...)
        {
            // TODO
            continue;
        }
		bool rootItem = true;
		id++;
        TreeItem item = {false, id, id, cameraName};
        if(groupNameItemIdMap.count(groupName) != 0)
        {
            item.ParentID = groupNameItemIdMap[groupName];
			rootItem = false;
        }
        s_cameraItems.insertTreeItem(item, new unsigned int(cameraKey), rootItem);
    }
	s_inited = true;
}

}
