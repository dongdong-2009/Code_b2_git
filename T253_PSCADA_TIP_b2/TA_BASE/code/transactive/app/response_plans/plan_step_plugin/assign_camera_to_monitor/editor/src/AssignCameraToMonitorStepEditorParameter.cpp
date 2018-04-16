#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/common/AssignCameraToMonitorStepParameter.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/AssignCameraToMonitorStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

    AssignCameraToMonitorStepEditorParameter::AssignCameraToMonitorStepEditorParameter(AssignCameraToMonitorStepParameter& refParameter)
    : InteractiveParameter(refParameter.basic),
      m_refParameter(refParameter)
    {
    }

    AssignCameraToMonitorStepEditorParameter::~AssignCameraToMonitorStepEditorParameter()
    {
    }

    bool AssignCameraToMonitorStepEditorParameter::failureIgnorable()
    {
        return true;
    }

    bool AssignCameraToMonitorStepEditorParameter::skippable()
    {
        return true;
    }

    bool AssignCameraToMonitorStepEditorParameter::delayable()
    {
        return true;
    }

    void AssignCameraToMonitorStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
    {
		bool ParamCameraValid = false;
		bool ParamMonitorValid = false;

        ItemPair itmNameValue;

		std::string valueCamera = m_cameras.getCameraTreeItemById(m_cameras.getItemIdByCameraKey(m_refParameter.camera)).Item;
		std::string valueMonitor = m_monitors.getMonitorTreeItemById(m_monitors.getItemIdByMonitorKey(m_refParameter.monitor)).Item;

		ParamCameraValid = !valueCamera.empty();
		ParamMonitorValid = !valueMonitor.empty();

		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueCamera, ParamCamera, ParamCamera, eTree, ParamCameraValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMonitor, ParamMonitor, ParamMonitor, eTree, ParamMonitorValid);
    }

	bool AssignCameraToMonitorStepEditorParameter::responseRequired()
	{
		return false;
	}

	ITreePicker* AssignCameraToMonitorStepEditorParameter::getTreePicker( const std::string& strName )
	{
		return this;
	}

	void AssignCameraToMonitorStepEditorParameter::getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/ )
	{
		if(strName == ParamCamera)
		{
			vecKeyItems = m_cameras.getRootCameraItems();
			unsigned int id = m_cameras.getItemIdByCameraKey(m_refParameter.camera);
			if(CameraEditorHelper::INVALID_ID != id)
			{
				vecSelected.push_back(id);
			}
			nMaxSelection = 1;
		}
		else if(strName == ParamMonitor)
		{
			vecKeyItems = m_monitors.getRootMonitorItems();
			unsigned int id = m_monitors.getItemIdByMonitorKey(m_refParameter.monitor);
			if(MonitorEditorHelper::INVALID_ID != id)
			{
				vecSelected.push_back(id);
			}
			nMaxSelection = 1;
		}
	}

	void AssignCameraToMonitorStepEditorParameter::getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId )
	{
		if(strName == ParamCamera)
		{
			vecKeyItems = m_cameras.getChildCameraItems(unNodeId);
		}
		else if(strName == ParamMonitor)
		{
			vecKeyItems = m_monitors.getChildMonitorItems(unNodeId);
		}
	}

	bool AssignCameraToMonitorStepEditorParameter::pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/ )
	{
		if(vecSelectedKeys.empty())
		{
			return false;
		}
		if(strName == ParamCamera)
		{
			unsigned int key = m_cameras.getCameraKeyByItemId(vecSelectedKeys[0]);
			if(CameraEditorHelper::INVALID_ID != key)
			{
				m_refParameter.camera = key;
				TreeItem tmp = m_cameras.getCameraTreeItemById(vecSelectedKeys[0]);
				strDisplayValue = tmp.Item;
			}
		}
		else if(strName == ParamMonitor)
		{
			unsigned int key = m_monitors.getMonitorKeyByItemId(vecSelectedKeys[0]);
			if(MonitorEditorHelper::INVALID_ID != key)
			{
				m_refParameter.monitor = key;
				TreeItem tmp = m_monitors.getMonitorTreeItemById(vecSelectedKeys[0]);
				strDisplayValue = tmp.Item;
			}
		}
		return false;
	}

}
