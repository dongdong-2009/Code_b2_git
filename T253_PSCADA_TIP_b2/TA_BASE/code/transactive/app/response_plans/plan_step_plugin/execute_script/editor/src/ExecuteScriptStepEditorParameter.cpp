#include "app/response_plans/plan_step_plugin/execute_script/common/ExecuteScriptStepParameter.h"
#include "ExecuteScriptStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

    ExecuteScriptStepEditorParameter::ExecuteScriptStepEditorParameter(ExecuteScriptStepParameter& refParameter) :
    InteractiveParameter( refParameter.basic ),
    m_refParameter( refParameter )
    {
    }

    ExecuteScriptStepEditorParameter::~ExecuteScriptStepEditorParameter()
    {
    }

    bool ExecuteScriptStepEditorParameter::failureIgnorable()
    {
        return true;
    }

    bool ExecuteScriptStepEditorParameter::skippable()
    {
        return true;
    }

    bool ExecuteScriptStepEditorParameter::delayable()
    {
        return true;
    }

    // [ToDo] Design general solution for all steps, using only defineAddonParameters
    void ExecuteScriptStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
    {
        ItemPair itmNameValue;

		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.commands, ParamCommands, ParamCommands, eString, true);
    }

	IStringEditor* ExecuteScriptStepEditorParameter::getStringEditor(const std::string& strName)
    {
        return this;
    }

    // [ToDo] Design general solution for all steps, using only defineAddonParameters
    bool ExecuteScriptStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
    {
        if(0 == strName.compare(ParamCommands))
        {
			convertToActualValue(strDisplayValue, m_refParameter.commands);
            bValid = true;
        }
        // No parameter list changed
        return false;
    }

	bool ExecuteScriptStepEditorParameter::responseRequired()
	{
		return false;
	}

	void ExecuteScriptStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
	{
        if(0 == strName.compare(ParamCommands))
        {
			convertToDisplayValue(m_refParameter.commands, strCurValue);
			szMaxLength = 800;
        }
	}

}
