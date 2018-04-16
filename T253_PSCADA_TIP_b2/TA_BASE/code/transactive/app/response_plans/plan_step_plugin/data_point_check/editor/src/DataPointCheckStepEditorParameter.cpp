#include "app/response_plans/plan_step_plugin/data_point_check/common/DataPointCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/DataPointCheckStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

DataPointCheckStepEditorParameter::DataPointCheckStepEditorParameter(DataPointCheckStepParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_datapointEditorHelper( true ),
m_planStepEditorHelper( parent )
{
}

DataPointCheckStepEditorParameter::~DataPointCheckStepEditorParameter()
{
}

bool DataPointCheckStepEditorParameter::failureIgnorable()
{
    return true;
}

bool DataPointCheckStepEditorParameter::skippable()
{
    return true;
}

bool DataPointCheckStepEditorParameter::delayable()
{
    return true;
}

bool DataPointCheckStepEditorParameter::responseRequired()
{
    return false;
}

void DataPointCheckStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamDataPointValid = false;
	bool ParamValueTypeValid = false;
	bool ParamValueValid = false;
	bool ParamTestTypeValid = false;
	bool ParamSuccessStepValid = false;
	bool ParamFailureStepValid = false;

	std::string valueTestType = m_datapointTestTypeEditorHelper.getTypeDescription(m_refParameter.testType);
	std::string valueSuccessStep = m_planStepEditorHelper.getItemById(m_refParameter.successStep).Item;
	std::string valueFailurestep = m_planStepEditorHelper.getItemById(m_refParameter.failureStep).Item;
	std::string valueValueType = m_datapointEditorHelper.getTypeDescription(m_refParameter.valueType);

	ParamDataPointValid = !m_refParameter.dataPoint.empty();
	ParamValueTypeValid = !valueValueType.empty();
	ParamTestTypeValid = !valueTestType.empty();
	ParamSuccessStepValid = !valueSuccessStep.empty();
	ParamFailureStepValid = !valueFailurestep.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.dataPoint, ParamDataPoint, ParamDataPoint, eTree, ParamDataPointValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueValueType, ParamValueType, ParamValueType, eReadOnly, ParamValueTypeValid);
	if(( BOOLEAN_DATA_POINT == m_refParameter.valueType ) || ( COMBINED_DATA_POINT == m_refParameter.valueType ))
	{
        std::string displayValue = m_refParameter.value;
        m_datapointEditorHelper.getDataPointDisplayValue(m_refParameter.dataPoint, m_refParameter.valueType, m_refParameter.value, displayValue);
        ParamValueValid = !displayValue.empty();
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, displayValue, ParamValue, ParamValue, eList, ParamValueValid);
	}
	else if(NUMERIC_DATA_POINT == m_refParameter.valueType)
	{
		ParamValueValid = !m_refParameter.value.empty();
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.value, ParamValue, ParamValue, eInteger, ParamValueValid);
	}
	else
	{
		ParamValueValid = true;
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.value, ParamValue, ParamValue, eString, ParamValueValid);
	}
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTestType, ParamTestType, ParamTestType, eList, ParamTestTypeValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueSuccessStep, ParamSuccessStep, ParamSuccessStep, eReadOnly, ParamSuccessStepValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueFailurestep, ParamFailureStep, ParamFailureStep, eReadOnly, ParamFailureStepValid);
}

std::string DataPointCheckStepEditorParameter::displayValue( const std::string& strName )
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

IStringEditor* DataPointCheckStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}


// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool DataPointCheckStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if( 0 == strName.compare( ParamValue ))
    {
        convertToActualValue( strDisplayValue, m_refParameter.value );
        bValid = !strDisplayValue.empty();
    }

    // No parameter list changed
    return false;
}

ITreePicker* DataPointCheckStepEditorParameter::getTreePicker(const std::string& strName)
{
    return this;
}

IListPicker* DataPointCheckStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void DataPointCheckStepEditorParameter::getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
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

void DataPointCheckStepEditorParameter::getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId)
{
    if(strName == ParamDataPoint)
    {
        vecKeyItems = m_datapointEditorHelper.getChildDataPointItems(unNodeId);
    }
}

bool DataPointCheckStepEditorParameter::pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    bool bListUpdated = false;

    if(strName == ParamDataPoint)
    {
        if ( !vecSelectedKeys.empty() )
        {
            const std::string strPrevDpName = m_refParameter.dataPoint;

            m_datapointEditorHelper.getDataPointInfo( vecSelectedKeys.front(), m_refParameter.dataPoint, m_refParameter.value, m_refParameter.valueType );

            strDisplayValue = m_refParameter.dataPoint;

            bListUpdated = ( 0 != strPrevDpName.compare( m_refParameter.dataPoint ));
        }
    }

    return bListUpdated;
}

void DataPointCheckStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamTestType)
    {
        vecKeyItems = m_datapointTestTypeEditorHelper.getItems(m_refParameter.valueType);
        if(m_datapointTestTypeEditorHelper.isValidTestType(m_refParameter.testType))
        {
            vecSelected.push_back(m_refParameter.testType);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamSuccessStep)
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
    else if(strName == ParamValue)
    {
        unsigned int unValueSelected = 0u;
        m_datapointEditorHelper.getDataPointValues(m_refParameter.dataPoint, m_refParameter.valueType, m_refParameter.value, vecKeyItems, unValueSelected);

        vecSelected.push_back(unValueSelected);

        nMaxSelection = 1;
    }
}

bool DataPointCheckStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamTestType)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.testType = static_cast<EDataPointTest>(vecSelectedKeys.front());
            strDisplayValue = m_datapointTestTypeEditorHelper.getTypeDescription( m_refParameter.testType );
        }
    }
    else if(strName == ParamSuccessStep)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.successStep = static_cast<EDataPointTest>(vecSelectedKeys.front());
            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.successStep ).Item;
        }
    }
    else if(strName == ParamFailureStep)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.failureStep = vecSelectedKeys.front();
            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.failureStep ).Item;
        }
    }
    else if(strName == ParamValueType)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_refParameter.valueType = static_cast<int>(vecSelectedKeys.front());
            strDisplayValue = m_datapointEditorHelper.getTypeDescription( m_refParameter.valueType );
        }
    }
    else if(strName == ParamValue)
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_datapointEditorHelper.getDataPointValue( m_refParameter.dataPoint, m_refParameter.valueType, vecSelectedKeys.front(), m_refParameter.value, strDisplayValue );
        }
    }

    return false;
}

void DataPointCheckStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if( 0 == strName.compare( ParamValue ))
    {
        convertToDisplayValue( m_refParameter.value, strCurValue );
		szMaxLength = ( NUMERIC_DATA_POINT == m_refParameter.valueType ) ? 10u : 40u;
    }
}

}
