#include "app/response_plans/plan_step_plugin/decision/common/DecisionStepParameter.h"
#include "app/response_plans/plan_step_plugin/decision/editor/src/DecisionStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"
#include <boost/lexical_cast.hpp>

namespace TA_Base_App
{

DecisionStepEditorParameter::DecisionStepEditorParameter(DecisionStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter(refParameter),
m_planStepEditorHelper(parent)
{
}

DecisionStepEditorParameter::~DecisionStepEditorParameter()
{
}

bool DecisionStepEditorParameter::failureIgnorable()
{
    return true;
}

bool DecisionStepEditorParameter::skippable()
{
    return true;
}

bool DecisionStepEditorParameter::delayable()
{
    return true;
}

bool DecisionStepEditorParameter::responseRequired()
{
    return true;
}

time_t DecisionStepEditorParameter::minimumTimeout()
{
    return 30u;
}

void DecisionStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamResponseTimeoutValid = (m_refParameter.responseTimeout >= minimumTimeout());
	bool ParamYesStepValid = false;
	bool ParamNoStepValid = false;

    ItemPair itmNameValue;

	std::string valueYesStep = m_planStepEditorHelper.getItemById(m_refParameter.yesStep).Item;
	std::string valueNoStep = m_planStepEditorHelper.getItemById(m_refParameter.noStep).Item;
	std::string valueResponseTimeout = TimeConvertHelper::converToDisplayValue(m_refParameter.responseTimeout, eMmSsTime);

	ParamYesStepValid = !valueYesStep.empty();
	ParamNoStepValid = !valueNoStep.empty();

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.decisionMessage, ParamDecisionMessage, ParamDecisionMessage, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueResponseTimeout, ParamResponseTimeout, ParamResponseTimeout, eMmSsTime, ParamResponseTimeoutValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueYesStep, ParamYesStep, ParamYesStep, eReadOnly, ParamYesStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueNoStep, ParamNoStep, ParamNoStep, eReadOnly, ParamNoStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.yesButtonLabel, ParamYesButtonLabel, ParamYesButtonLabel, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.noButtonLabel, ParamNoButtonLabel, ParamNoButtonLabel, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pauseButtonLabel, ParamPauseButtonLabel, ParamPauseButtonLabel, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.noButtonEnabled, ParamNoButtonEnabled, ParamNoButtonEnabled, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pauseButtonEnabled, ParamPauseButtonEnabled, ParamPauseButtonEnabled, eBoolean, true);
}

IStringEditor* DecisionStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}

IBoolEditor* DecisionStepEditorParameter::getBoolEditor(const std::string& strName)
{
    return this;
}

IListPicker* DecisionStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

ITimeEditor* DecisionStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool DecisionStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if(0 == strName.compare(ParamDecisionMessage))
    {
        convertToActualValue(strDisplayValue, m_refParameter.decisionMessage);
        bValid = true;
    }
    else if(0 == strName.compare(ParamYesButtonLabel))
    {
        convertToActualValue(strDisplayValue, m_refParameter.yesButtonLabel);
        bValid = true;
    }
    else if(0 == strName.compare(ParamNoButtonLabel))
    {
        convertToActualValue(strDisplayValue, m_refParameter.noButtonLabel);
        bValid = true;
    }
    else if(0 == strName.compare(ParamPauseButtonLabel))
    {
        convertToActualValue(strDisplayValue, m_refParameter.pauseButtonLabel);
        bValid = true;
    }
    // No parameter list changed
    return false;
}

bool DecisionStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(0 == strName.compare(ParamNoButtonEnabled))
    {
        m_refParameter.noButtonEnabled = bValue;
        convertToDisplayValue(m_refParameter.noButtonEnabled, strDisplayValue);
        bValid = true;
    }
    else if(0 == strName.compare(ParamPauseButtonEnabled))
    {
        m_refParameter.pauseButtonEnabled = bValue;
        convertToDisplayValue(m_refParameter.pauseButtonEnabled, strDisplayValue);
        bValid = true;
    }
    return false;
}

void DecisionStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0*/)
{
    if(0 == strName.compare(ParamNoButtonEnabled))
    {
        bValue = m_refParameter.noButtonEnabled;
    }
    else if(0 == strName.compare(ParamPauseButtonEnabled))
    {
        bValue = m_refParameter.pauseButtonEnabled;
    }
}

void DecisionStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamYesStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.yesStep);
        if(PlanStepEditorHelper::INVALID_ID != item.ID)
        {
            vecSelected.push_back(m_refParameter.yesStep);
        }
		nMaxSelection = 1;
    }
    else if(strName == ParamNoStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.noStep);
        if(PlanStepEditorHelper::INVALID_ID != item.ID)
        {
            vecSelected.push_back(m_refParameter.noStep);
        }
		nMaxSelection = 1;
    }
}

bool DecisionStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{

    if(strName == ParamYesStep)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.yesStep = vecSelectedKeys.front();
            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.yesStep ).Item;
        }
    }
    else if(strName == ParamNoStep)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.noStep = vecSelectedKeys.front();
            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.noStep ).Item;
        }
    }

    return false;
}

bool DecisionStepEditorParameter::updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */ )
{
    if ( strName == ParamResponseTimeout )
    {
		unsigned long ulTotalSeconds = TimeConvertHelper::convertToParamTime(tmValue, eMmSsTime);

        if ( 0 <= ulTotalSeconds && 300 >= ulTotalSeconds )
        {
            m_refParameter.responseTimeout = ulTotalSeconds;
			strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.responseTimeout, eMmSsTime);
            bValid = true;
        }
        else
        {
			strDisplayValue = TimeConvertHelper::converToDisplayValue(ulTotalSeconds, eMmSsTime);
            bValid = false;
        }
    }

    return false;
}

void DecisionStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/)
{
    if( strName == ParamResponseTimeout )
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.responseTimeout, eMmSsTime);
    }
}

void DecisionStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if(0 == strName.compare(ParamDecisionMessage))
    {
		convertToDisplayValue(m_refParameter.decisionMessage, strCurValue);
		szMaxLength = 800;
    }
    else if(0 == strName.compare(ParamYesButtonLabel))
    {
		convertToDisplayValue(m_refParameter.yesButtonLabel, strCurValue);
		szMaxLength = 40;
    }
    else if(0 == strName.compare(ParamNoButtonLabel))
    {
		convertToDisplayValue(m_refParameter.noButtonLabel, strCurValue);
		szMaxLength = 40;
    }
    else if(0 == strName.compare(ParamPauseButtonLabel))
    {
		convertToDisplayValue(m_refParameter.pauseButtonLabel, strCurValue);
		szMaxLength = 40;
    }
}

}
