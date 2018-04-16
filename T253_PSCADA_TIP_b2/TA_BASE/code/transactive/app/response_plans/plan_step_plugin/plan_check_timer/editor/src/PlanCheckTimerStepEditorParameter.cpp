#include "app/response_plans/plan_step_plugin/plan_check_timer/common/PlanCheckTimerStepParameter.h"
#include "PlanCheckTimerStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

PlanCheckTimerStepEditorParameter::PlanCheckTimerStepEditorParameter(PlanCheckTimerStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_planStepEditorHelper( parent )
{
}

PlanCheckTimerStepEditorParameter::~PlanCheckTimerStepEditorParameter()
{
}

bool PlanCheckTimerStepEditorParameter::failureIgnorable()
{
    return true;
}

bool PlanCheckTimerStepEditorParameter::skippable()
{
    return true;
}

bool PlanCheckTimerStepEditorParameter::delayable()
{
    return true;
}

bool PlanCheckTimerStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void PlanCheckTimerStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamTimeSpanValid = false;
	bool ParamSuccessStepValid = false;
	bool ParamFailureStepValid = false;

	std::string valueSuccessStep = m_planStepEditorHelper.getItemById(m_refParameter.successStep).Item;
	std::string valueFailureStep = m_planStepEditorHelper.getItemById(m_refParameter.failureStep).Item;
	std::string valueTimeSpan = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);

	ParamTimeSpanValid = !valueTimeSpan.empty();
	ParamSuccessStepValid = !valueSuccessStep.empty();
	ParamFailureStepValid = !valueFailureStep.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTimeSpan, ParamTimeSpan, ParamTimeSpan, eHhMmSsTime, ParamTimeSpanValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueSuccessStep, ParamSuccessStep, ParamSuccessStep, eReadOnly, ParamSuccessStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueFailureStep, ParamFailureStep, ParamFailureStep, eReadOnly, ParamFailureStepValid);
}

IListPicker* PlanCheckTimerStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

ITimeEditor* PlanCheckTimerStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

void PlanCheckTimerStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
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

bool PlanCheckTimerStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
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

bool PlanCheckTimerStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamTimeSpan)
    {
		m_refParameter.timeSpan = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);
        bValid = true;
    }
	return false;
}

void PlanCheckTimerStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamTimeSpan)
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.timeSpan, eHhMmSsTime);
    }
}
}
