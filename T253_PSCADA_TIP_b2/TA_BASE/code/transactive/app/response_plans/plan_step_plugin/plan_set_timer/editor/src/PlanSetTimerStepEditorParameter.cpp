#include "app/response_plans/plan_step_plugin/plan_set_timer/common/PlanSetTimerStepParameter.h"
#include "PlanSetTimerStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

    PlanSetTimerStepEditorParameter::PlanSetTimerStepEditorParameter(PlanSetTimerStepParameter& refParameter) :
    InteractiveParameter( refParameter.basic ),
    m_refParameter( refParameter )
    {
    }

    PlanSetTimerStepEditorParameter::~PlanSetTimerStepEditorParameter()
    {
    }

    bool PlanSetTimerStepEditorParameter::failureIgnorable()
    {
        return true;
    }

    bool PlanSetTimerStepEditorParameter::skippable()
    {
        return true;
    }

    bool PlanSetTimerStepEditorParameter::delayable()
    {
        return true;
    }

    // [ToDo] Design general solution for all steps, using only defineAddonParameters
    void PlanSetTimerStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
    {
		bool ParamTimeValid = false;

		std::string valueTime = TimeConvertHelper::converToDisplayValue(m_refParameter.time, eHhMmSsTime);
		ParamTimeValid = !valueTime.empty();

        ItemPair itmNameValue;

	    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTime, ParamTime, ParamTime, eHhMmSsTime, ParamTimeValid);
    }

	bool PlanSetTimerStepEditorParameter::responseRequired()
	{
		return false;
	}

	ITimeEditor* PlanSetTimerStepEditorParameter::getTimeEditor( const std::string& strName )
	{
		return this;
	}

	bool PlanSetTimerStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
	{
		if(ParamTime == strName)
		{
		    m_refParameter.time = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
			strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.time, eHhMmSsTime);
			bValid = true;
		}
		return false;
	}

	void PlanSetTimerStepEditorParameter::getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0 */ )
	{
		if(ParamTime == strName)
		{
		    tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.time, eHhMmSsTime);
		}
	}

}
