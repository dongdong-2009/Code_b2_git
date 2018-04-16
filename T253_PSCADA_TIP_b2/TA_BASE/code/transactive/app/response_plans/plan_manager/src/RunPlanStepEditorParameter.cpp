#include "app/response_plans/plan_step_plugin/internal/src/RunPlanParameter.h"
#include "app/response_plans/plan_manager/src/RunPlanStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
//#include "core/utilities/src/DebugUtil.h"
//#include "app/response_plans/plan_step_plugin/common/src/ParameterType.h"

namespace TA_Base_App
{

RunPlanStepEditorParameter::RunPlanStepEditorParameter(RunPlanParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_planNodeEditorHelper()
{
}

RunPlanStepEditorParameter::~RunPlanStepEditorParameter()
{
}

bool RunPlanStepEditorParameter::failureIgnorable()
{
    return true;
}

bool RunPlanStepEditorParameter::skippable()
{
    return true;
}

bool RunPlanStepEditorParameter::delayable()
{
    return true;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void RunPlanStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamPlanValid = (0 != m_refParameter.plan);
	bool ParamManualLaunchValid = true;
	bool ParamWaitForCompletionValid = true;

	std::string valuePlan = m_planNodeEditorHelper.getPlanPath(m_refParameter.plan);

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePlan, ParamPlan, ParamPlan, eTree, ParamPlanValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.manualLaunch, ParamManualLaunch, ParamManualLaunch, eBoolean, ParamManualLaunchValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.waitForCompletion, ParamWaitForCompletion, ParamWaitForCompletion, eBoolean, ParamWaitForCompletionValid);
}


bool RunPlanStepEditorParameter::responseRequired()
{
    return true;
}

time_t RunPlanStepEditorParameter::minimumTimeout()
{
    return 30u;
}

bool RunPlanStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamManualLaunch))
    {
        m_refParameter.manualLaunch = bValue;
        convertToDisplayValue(m_refParameter.manualLaunch, strDisplayValue);
        bValid = true;
    }
    else if(0 == strName.compare(ParamWaitForCompletion))
    {
        m_refParameter.waitForCompletion = bValue;
        convertToDisplayValue(m_refParameter.waitForCompletion, strDisplayValue);
        bValid = true;
    }
    return false;
}

void RunPlanStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamManualLaunch))
    {
        bValue = m_refParameter.manualLaunch;
    }
    else if(0 == strName.compare(ParamWaitForCompletion))
    {
        bValue = m_refParameter.waitForCompletion;
    }
}

IBoolEditor* RunPlanStepEditorParameter::getBoolEditor(const std::string& strName)
{
    return this;
}

ITreePicker* RunPlanStepEditorParameter::getTreePicker(const std::string& strName)
{
    return this;
}

void RunPlanStepEditorParameter::getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
	if(strName == ParamPlan)
	{
		vecKeyItems = m_planNodeEditorHelper.getRootItems();
		if(!m_planNodeEditorHelper.getPlanPath(m_refParameter.plan).empty())
		{
			vecSelected.push_back(m_refParameter.plan);
		}
		nMaxSelection = 1;
	}
}

void RunPlanStepEditorParameter::getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId)
{
	vecKeyItems = m_planNodeEditorHelper.getChildItems(unNodeId);
}

bool RunPlanStepEditorParameter::pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(!vecSelectedKeys.empty())
    {
        m_refParameter.plan = vecSelectedKeys.front();
        strDisplayValue = m_planNodeEditorHelper.getPlanPath( m_refParameter.plan );
    }

    return false;
}

}