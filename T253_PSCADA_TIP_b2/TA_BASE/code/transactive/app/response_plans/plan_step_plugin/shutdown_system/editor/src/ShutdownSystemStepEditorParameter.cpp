#include "app/response_plans/plan_step_plugin/shutdown_system/common/ShutdownSystemStepParameter.h"
#include "ShutdownSystemStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
//#include "app/response_plans/plan_step_plugin/common/src/ParameterType.h"

using namespace TA_Base_App;

ShutdownSystemStepEditorParameter::ShutdownSystemStepEditorParameter(ShutdownSystemStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

ShutdownSystemStepEditorParameter::~ShutdownSystemStepEditorParameter()
{
}

bool ShutdownSystemStepEditorParameter::failureIgnorable()
{
    return true;
}

bool ShutdownSystemStepEditorParameter::skippable()
{
    return true;
}

bool ShutdownSystemStepEditorParameter::delayable()
{
    return true;
}

IBoolEditor* ShutdownSystemStepEditorParameter::getBoolEditor(const std::string& strName)
{
	return this;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void ShutdownSystemStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.notifyPeer, ParamNotifyPeer, ParamNotifyPeer, eBoolean, true);
}

bool ShutdownSystemStepEditorParameter::responseRequired()
{
    return false;
}

bool ShutdownSystemStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamNotifyPeer))
    {
		m_refParameter.notifyPeer = bValue;
		convertToDisplayValue(m_refParameter.notifyPeer, strDisplayValue);
        bValid = true;
    }
    return false;
}

void ShutdownSystemStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamNotifyPeer))
    {
        bValue = m_refParameter.notifyPeer;
    }
}