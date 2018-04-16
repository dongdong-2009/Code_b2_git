
#include "app/response_plans/plan_step_plugin/internal/src/TerminatePlanParameter.h"
#include "app/response_plans/plan_manager/src/TerminatePlanEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

TerminatePlanEditorParameter::TerminatePlanEditorParameter( TerminatePlanParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

TerminatePlanEditorParameter::~TerminatePlanEditorParameter()
{
}

void TerminatePlanEditorParameter::getNameValuePairs( std::vector<ItemPair>& vecNameValues )
{
	bool ParamPlanValid = false;

	std::string valuePlan = m_planNodeEditorHelper.getPlanPath(m_refParameter.plan);
	ParamPlanValid = !valuePlan.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePlan, ParamPlan, ParamPlan, eTree, ParamPlanValid);
}

bool TerminatePlanEditorParameter::failureIgnorable()
{
	return true;
}

bool TerminatePlanEditorParameter::skippable()
{
	return true;
}

bool TerminatePlanEditorParameter::delayable()
{
	return true;
}

bool TerminatePlanEditorParameter::responseRequired()
{
	return false;
}

ITreePicker* TerminatePlanEditorParameter::getTreePicker( const std::string& strName )
{
	return this;
}

void TerminatePlanEditorParameter::getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/ )
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

void TerminatePlanEditorParameter::getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId )
{
	vecKeyItems = m_planNodeEditorHelper.getChildItems(unNodeId);
}

bool TerminatePlanEditorParameter::pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/ )
{
    if( !vecSelectedKeys.empty() )
    {
        m_refParameter.plan = vecSelectedKeys.front();
        strDisplayValue = m_planNodeEditorHelper.getPlanPath( m_refParameter.plan );
    }

    return false;
}

}