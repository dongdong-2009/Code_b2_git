#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"

namespace TA_Base_App
{
class IPlanNode;
class PlanCheckTimerStepParameter;
class PlanCheckTimerStepEditorParameter : public InteractiveParameter, public IListPicker, public ITimeEditor
{
public:
    PlanCheckTimerStepEditorParameter(PlanCheckTimerStepParameter& refParameter, IPlanNode& parent);
    ~PlanCheckTimerStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IListPicker* getListPicker(const std::string& strName);
    virtual ITimeEditor* getTimeEditor(const std::string& strName);

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

    // ITimeEditor
	virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow = 0);
protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    PlanCheckTimerStepParameter& m_refParameter;
    PlanStepEditorHelper m_planStepEditorHelper;
};

}