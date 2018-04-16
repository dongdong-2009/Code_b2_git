#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"

namespace TA_Base_App
{
class DecisionStepParameter;
class IPlanNode;
class DecisionStepEditorParameter : public InteractiveParameter, public IStringEditor, public IBoolEditor, public IListPicker, public ITimeEditor
{
public:
    DecisionStepEditorParameter(DecisionStepParameter& refParameter, IPlanNode& parent);
    ~DecisionStepEditorParameter();

    // InteractiveParameter
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IStringEditor* getStringEditor(const std::string& strName);
    virtual IBoolEditor* getBoolEditor(const std::string& strName);
    virtual IListPicker* getListPicker(const std::string& strName);
    virtual ITimeEditor* getTimeEditor(const std::string& strName);

    // IStringEditor
    virtual bool updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );

    // IboolEditor
    virtual bool updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow = 0);

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

    // ITimeEditor
    virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow = 0 );

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();
    virtual time_t minimumTimeout();

private:
    DecisionStepParameter& m_refParameter;
	PlanStepEditorHelper m_planStepEditorHelper;
};

}