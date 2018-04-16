#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanNodeEditorHelper.h"


namespace TA_Base_App
{

class RunPlanParameter;
class RunPlanStepEditorParameter : public InteractiveParameter, public ITreePicker, public IBoolEditor
{
public:
    RunPlanStepEditorParameter(RunPlanParameter& refParameter);
    ~RunPlanStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);

    // IBoolEditor
    virtual IBoolEditor* getBoolEditor(const std::string& strName);
    virtual bool updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow = 0);

    // ITreePick
    virtual ITreePicker* getTreePicker(const std::string& strName);
    virtual void getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual void getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId);
    virtual bool pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();
    virtual time_t minimumTimeout();

private:
    RunPlanParameter& m_refParameter;
	PlanNodeEditorHelper m_planNodeEditorHelper;
};

}
