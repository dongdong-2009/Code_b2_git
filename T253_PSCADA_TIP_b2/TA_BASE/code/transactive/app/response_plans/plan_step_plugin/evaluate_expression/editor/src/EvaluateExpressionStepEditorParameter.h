#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"

namespace TA_Base_App
{
class EvaluateExpressionStepParameter;
class IPlanNode;
class EvaluateExpressionStepEditorParameter : public InteractiveParameter, public IStringEditor, public IListPicker
{
public:
    EvaluateExpressionStepEditorParameter(EvaluateExpressionStepParameter& refParameter, IPlanNode& parent);
    ~EvaluateExpressionStepEditorParameter();

    // InteractiveParameter
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IStringEditor* getStringEditor(const std::string& strName);
    virtual IListPicker* getListPicker(const std::string& strName);

    // IStringEditor
    virtual bool updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    EvaluateExpressionStepParameter& m_refParameter;
	PlanStepEditorHelper m_planStepEditorHelper;
};

}