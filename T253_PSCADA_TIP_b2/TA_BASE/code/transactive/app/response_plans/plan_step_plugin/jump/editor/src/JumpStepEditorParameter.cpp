#include "app/response_plans/plan_step_plugin/jump/common/JumpStepParameter.h"
#include "JumpStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

JumpStepEditorParameter::JumpStepEditorParameter(JumpStepParameter& refParameter, IPlanNode& parent) :
InteractiveParameter( refParameter.basic ), 
m_refParameter( refParameter ),
m_planStepEditorHelper( parent )
{
}

JumpStepEditorParameter::~JumpStepEditorParameter()
{
}

bool JumpStepEditorParameter::failureIgnorable()
{
    return true;
}

bool JumpStepEditorParameter::skippable()
{
    return true;
}

bool JumpStepEditorParameter::delayable()
{
    return true;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void JumpStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamStepIdValid = false;

	std::string valueStepId = m_planStepEditorHelper.getItemById(m_refParameter.stepId).Item;

	ParamStepIdValid = !valueStepId.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueStepId, ParamStepId, ParamStepId, eReadOnly, ParamStepIdValid);
}

bool JumpStepEditorParameter::responseRequired()
{
    return false;
}

IListPicker* JumpStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void JumpStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamStepId)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.stepId);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
}

bool JumpStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamStepId)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.stepId = vecSelectedKeys.front();
            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.stepId ).Item;
        }
    }

    return false;
}

}
