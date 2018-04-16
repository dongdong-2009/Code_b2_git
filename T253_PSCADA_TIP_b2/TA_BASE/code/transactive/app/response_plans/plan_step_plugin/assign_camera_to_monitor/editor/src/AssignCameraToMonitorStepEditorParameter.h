#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/MonitorEditorHelper.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/CameraEditorHelper.h"

namespace TA_Base_App
{
class AssignCameraToMonitorStepParameter;
class AssignCameraToMonitorStepEditorParameter : public InteractiveParameter, public ITreePicker
{
public:
    AssignCameraToMonitorStepEditorParameter(AssignCameraToMonitorStepParameter& refParameter);
    ~AssignCameraToMonitorStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual ITreePicker* getTreePicker(const std::string& strName);

    // ITreePicker
    virtual void getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual void getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId);
    virtual bool pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    AssignCameraToMonitorStepParameter& m_refParameter;
	MonitorEditorHelper m_monitors;
	CameraEditorHelper m_cameras;
};

}