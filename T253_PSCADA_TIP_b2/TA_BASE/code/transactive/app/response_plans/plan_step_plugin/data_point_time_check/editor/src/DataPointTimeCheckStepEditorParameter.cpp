#include "app/response_plans/plan_step_plugin/data_point_time_check/common/DataPointTimeCheckStepParameter.h"
#include "DataPointTimeCheckStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

DataPointTimeCheckStepEditorParameter::DataPointTimeCheckStepEditorParameter(DataPointTimeCheckStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_planStepEditorHelper( parent )
{
}

DataPointTimeCheckStepEditorParameter::~DataPointTimeCheckStepEditorParameter()
{
}

bool DataPointTimeCheckStepEditorParameter::failureIgnorable()
{
    return true;
}

bool DataPointTimeCheckStepEditorParameter::skippable()
{
    return true;
}

bool DataPointTimeCheckStepEditorParameter::delayable()
{
    return true;
}

bool DataPointTimeCheckStepEditorParameter::responseRequired()
{
    return false;
}
// [ToDo] Design general solution for all steps, using only defineAddonParameters
void DataPointTimeCheckStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamDataPointValid = false;
	bool ParamTimeSpanValid = false;
	bool ParamSuccessStepValid = false;
	bool ParamFailureStepValid = false;

	std::string valueSuccessStep = m_planStepEditorHelper.getItemById(m_refParameter.successStep).Item;
	std::string valueFailureStep = m_planStepEditorHelper.getItemById(m_refParameter.failureStep).Item;
	std::string valueTimeSpan = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);

	ParamDataPointValid = !m_refParameter.dataPoint.empty();
	ParamTimeSpanValid = !valueTimeSpan.empty();
	ParamSuccessStepValid = !valueSuccessStep.empty();
	ParamFailureStepValid = !valueFailureStep.empty();

    ItemPair itmNameValue;
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.dataPoint, ParamDataPoint, ParamDataPoint, eTree, ParamDataPointValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTimeSpan, ParamTimeSpan, ParamTimeSpan, eHhMmSsTime, ParamTimeSpanValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueSuccessStep, ParamSuccessStep, ParamSuccessStep, eReadOnly, ParamSuccessStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueFailureStep, ParamFailureStep, ParamFailureStep, eReadOnly, ParamFailureStepValid);
}

std::string DataPointTimeCheckStepEditorParameter::displayValue( const std::string& strName )
{
	if(strName == ParamSuccessStep)
	{
		return m_planStepEditorHelper.getItemById(m_refParameter.successStep).Item;
	}
	else if(strName == ParamFailureStep)
	{
		return m_planStepEditorHelper.getItemById(m_refParameter.failureStep).Item;
	}

	return "";
}

IListPicker* DataPointTimeCheckStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

ITreePicker* DataPointTimeCheckStepEditorParameter::getTreePicker(const std::string& strName)
{
    return this;
}

ITimeEditor* DataPointTimeCheckStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

void DataPointTimeCheckStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamSuccessStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.successStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamFailureStep)
    {
        vecKeyItems = m_planStepEditorHelper.getSteps();
        ListItem item = m_planStepEditorHelper.getItemById(m_refParameter.failureStep);
        if(m_planStepEditorHelper.INVALID_ID != item.ID)
        {
            vecSelected.push_back(item.ID);
        }
        nMaxSelection = 1;
    }
}

bool DataPointTimeCheckStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamSuccessStep)
    {
        m_refParameter.successStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
    else if(strName == ParamFailureStep)
    {
        m_refParameter.failureStep = vecSelectedKeys[0];
		strDisplayValue = m_planStepEditorHelper.getItemById(vecSelectedKeys[0]).Item;
    }
	return false;
}

void DataPointTimeCheckStepEditorParameter::getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDataPoint)
    {
        vecKeyItems = m_datapointEditorHelper.getRootDataPointItems();
		const unsigned int selected = m_datapointEditorHelper.getItemIdByDataPointName(m_refParameter.dataPoint);
		if(m_datapointEditorHelper.INVALID_ID != selected)
		{
			vecSelected.push_back(selected);
		}
        nMaxSelection = 1;
    }
}

void DataPointTimeCheckStepEditorParameter::getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId)
{
    if(strName == ParamDataPoint)
    {
        vecKeyItems = m_datapointEditorHelper.getChildDataPointItems(unNodeId);
    }
}

bool DataPointTimeCheckStepEditorParameter::pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamDataPoint)
    {
        m_refParameter.dataPoint = m_datapointEditorHelper.getDataPointNameByItemId(vecSelectedKeys[0]);
		strDisplayValue = m_refParameter.dataPoint;
    }
	return false;
}

bool DataPointTimeCheckStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamTimeSpan)
    {
		m_refParameter.timeSpan = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);
        bValid = true;
    }
	return false;
}

void DataPointTimeCheckStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamTimeSpan)
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.timeSpan, eHhMmSsTime);
    }
}


}
