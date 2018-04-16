#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepParameter.h"
#include "PromptStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

PromptStepEditorParameter::PromptStepEditorParameter(PromptStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

PromptStepEditorParameter::~PromptStepEditorParameter()
{
}

bool PromptStepEditorParameter::failureIgnorable()
{
    return true;
}

bool PromptStepEditorParameter::skippable()
{
    return true;
}

bool PromptStepEditorParameter::delayable()
{
    return true;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void PromptStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamDismissTimeoutValid = (m_refParameter.dismissTimeout >= minimumTimeout());

	std::string valueDismissTimeout = TimeConvertHelper::converToDisplayValue(m_refParameter.dismissTimeout, eMmSsTime);

    ItemPair itmNameValue;


    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.message, ParamMessage, ParamMessage, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueDismissTimeout, ParamDismissTimeout, ParamDismissTimeout, eMmSsTime, ParamDismissTimeoutValid);
}

IStringEditor* PromptStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}


// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool PromptStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if(0 == strName.compare(ParamMessage))
    {
        convertToActualValue(strDisplayValue, m_refParameter.message);
        bValid = true;
    }
    // No parameter list changed
    return false;
}

bool PromptStepEditorParameter::responseRequired()
{
    return true;
}

time_t PromptStepEditorParameter::minimumTimeout()
{
    return 30u;
}

ITimeEditor* PromptStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

bool PromptStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDismissTimeout)
    {
		unsigned long ulDismissTimeout = TimeConvertHelper::convertToParamTime(tmValue, eMmSsTime);
		if(10 <= ulDismissTimeout && 300 >= ulDismissTimeout)
		{
			m_refParameter.dismissTimeout = ulDismissTimeout;
			strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.dismissTimeout, eMmSsTime);
	        bValid = true;
		}
		else
		{
			strDisplayValue = TimeConvertHelper::converToDisplayValue(ulDismissTimeout, eMmSsTime);
			bValid = false;
		}
    }
	return false;
}

void PromptStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamDismissTimeout)
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.dismissTimeout, eMmSsTime);
    }
}

void PromptStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if(0 == strName.compare(ParamMessage))
    {
		convertToDisplayValue(m_refParameter.message, strCurValue);
		szMaxLength = 800;
    }
}

}
