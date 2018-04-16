#include "app/response_plans/plan_step_plugin/evaluate_expression/common/EvaluateExpressionStepParameter.h"
#include "EvaluateExpressionStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

EvaluateExpressionStepEditorParameter::EvaluateExpressionStepEditorParameter(EvaluateExpressionStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter(refParameter),
m_planStepEditorHelper(parent)
{
}

EvaluateExpressionStepEditorParameter::~EvaluateExpressionStepEditorParameter()
{
}

bool EvaluateExpressionStepEditorParameter::failureIgnorable()
{
    return true;
}

bool EvaluateExpressionStepEditorParameter::skippable()
{
    return true;
}

bool EvaluateExpressionStepEditorParameter::delayable()
{
    return true;
}

bool EvaluateExpressionStepEditorParameter::responseRequired()
{
    return false;
}

void EvaluateExpressionStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamSuccessStepValid = false;
	bool ParamFailureStepValid = false;

	std::string valueSuccessStep = m_planStepEditorHelper.getItemById(m_refParameter.successStep).Item;
	std::string valueFailureStep = m_planStepEditorHelper.getItemById(m_refParameter.failureStep).Item;

	ParamSuccessStepValid = !valueSuccessStep.empty();
	ParamFailureStepValid = !valueFailureStep.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.expression, ParamExpression, ParamExpression, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueSuccessStep, ParamSuccessStep, ParamSuccessStep, eList, ParamSuccessStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueFailureStep, ParamFailureStep, ParamFailureStep, eList, ParamFailureStepValid);
}

IStringEditor* EvaluateExpressionStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool EvaluateExpressionStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if(0 == strName.compare(ParamExpression))
    {
        convertToActualValue(strDisplayValue, m_refParameter.expression);
        bValid = true;
    }
    // No parameter list changed
    return false;
}

IListPicker* EvaluateExpressionStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void EvaluateExpressionStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamSuccessStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.successStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamFailureStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.failureStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
}

bool EvaluateExpressionStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamSuccessStep)
    {
        m_refParameter.successStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
    else if(strName == ParamFailureStep)
    {
        m_refParameter.failureStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
	return false;
}

void EvaluateExpressionStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if(0 == strName.compare(ParamExpression))
    {
		convertToDisplayValue(m_refParameter.expression, strCurValue);
		szMaxLength = 150;
    }
}

}
