#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryStepParameter.h"
#include "FailureSummaryStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

FailureSummaryStepEditorParameter::FailureSummaryStepEditorParameter(FailureSummaryStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter(refParameter)
{
}

FailureSummaryStepEditorParameter::~FailureSummaryStepEditorParameter()
{
}

bool FailureSummaryStepEditorParameter::failureIgnorable()
{
    return true;
}

bool FailureSummaryStepEditorParameter::skippable()
{
    return true;
}

bool FailureSummaryStepEditorParameter::delayable()
{
    return true;
}

bool FailureSummaryStepEditorParameter::responseRequired()
{
    return true;
}

time_t FailureSummaryStepEditorParameter::minimumTimeout()
{
    return 30u;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void FailureSummaryStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamFailureCategoryValid = false;

	std::string valueFailureCategory = m_failureSummaryEditorHelper.getFailureSummaryDescription(m_refParameter.failureCategory);

	ParamFailureCategoryValid = !valueFailureCategory.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueFailureCategory, ParamFailureCategory, ParamFailureCategory, eList, ParamFailureCategoryValid);
}

IListPicker* FailureSummaryStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void FailureSummaryStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamFailureCategory)
    {
        vecKeyItems = m_failureSummaryEditorHelper.getItems();
        if(m_failureSummaryEditorHelper.isValidFailureSummaryType(m_refParameter.failureCategory))
        {
            vecSelected.push_back(m_refParameter.failureCategory);
        }
        nMaxSelection = 1;
    }
}

bool FailureSummaryStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamFailureCategory)
    {
        m_refParameter.failureCategory = static_cast<FailureSummaryStepParameter::EStepFailureCategory>(vecSelectedKeys[0]);
		strDisplayValue = m_failureSummaryEditorHelper.getFailureSummaryDescription(vecSelectedKeys[0]);
    }
	return false;
}


}
