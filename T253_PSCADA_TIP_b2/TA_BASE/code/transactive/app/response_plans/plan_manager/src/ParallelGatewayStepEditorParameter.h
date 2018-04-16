#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"

namespace TA_Base_App
{

class ParallelGatewayParameter;
class IPlanNode;
class ParallelGatewayStepEditorParameter : public InteractiveParameter, public IListPicker
{
public:
    ParallelGatewayStepEditorParameter(ParallelGatewayParameter& refParameter, IPlanNode& parent);
    ~ParallelGatewayStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IListPicker* getListPicker(const std::string& strName);

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    ParallelGatewayParameter& m_refParameter;
    PlanStepEditorHelper m_planStepEditorHelper;
};

}