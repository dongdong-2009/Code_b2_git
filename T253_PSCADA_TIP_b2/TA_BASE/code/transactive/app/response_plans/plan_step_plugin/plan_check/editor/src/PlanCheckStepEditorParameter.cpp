#include "app/response_plans/plan_step_plugin/plan_check/common/PlanCheckStepParameter.h"
#include "PlanCheckStepEditorParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

PlanCheckStepEditorParameter::PlanCheckStepEditorParameter(PlanCheckStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_planNodeEditorHelper(),
m_planStepEditorHelper( parent )
{
}

PlanCheckStepEditorParameter::~PlanCheckStepEditorParameter()
{
}

bool PlanCheckStepEditorParameter::failureIgnorable()
{
    return true;
}

bool PlanCheckStepEditorParameter::skippable()
{
    return true;
}

bool PlanCheckStepEditorParameter::delayable()
{
    return true;
}

bool PlanCheckStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void PlanCheckStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamPlanValid = false;
	bool ParamYesStepValid = false;
	bool ParamNoStepValid = false;

	std::string valuePlan = m_planNodeEditorHelper.getPlanPath(m_refParameter.plan);
	std::string valueYesStep = m_planStepEditorHelper.getItemById(m_refParameter.yesStep).Item;
	std::string valueNoStep = m_planStepEditorHelper.getItemById(m_refParameter.noStep).Item;

	ParamPlanValid = !valuePlan.empty();
	ParamYesStepValid = !valueYesStep.empty();
	ParamNoStepValid = !valueNoStep.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePlan, ParamPlan, ParamPlan, eTree, ParamPlanValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueYesStep, ParamYesStep, ParamYesStep, eReadOnly, ParamYesStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueNoStep, ParamNoStep, ParamNoStep, eReadOnly, ParamNoStepValid);
}

IListPicker* PlanCheckStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void PlanCheckStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{

    if(strName == ParamYesStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.yesStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamNoStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.noStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
}

bool PlanCheckStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamYesStep)
    {
        m_refParameter.yesStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
    else if(strName == ParamNoStep)
    {
        m_refParameter.noStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
	return false;
}

ITreePicker* PlanCheckStepEditorParameter::getTreePicker(const std::string& strName)
{
	return this;
}

void PlanCheckStepEditorParameter::getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */ )
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

void PlanCheckStepEditorParameter::getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId )
{
	if(strName == ParamPlan)
	{
		vecKeyItems = m_planNodeEditorHelper.getChildItems(unNodeId);
	}
}

bool PlanCheckStepEditorParameter::pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */ )
{
	if(vecSelectedKeys.empty())
	{
		return false;
	}
	if(strName == ParamPlan)
	{
		m_refParameter.plan = vecSelectedKeys[0];
		strDisplayValue = m_planNodeEditorHelper.getPlanPath(m_refParameter.plan);
	}
	return false;
}

}
