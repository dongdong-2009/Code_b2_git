#include "app/response_plans/plan_step_plugin/data_point_set/common/DataPointSetStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/DataPointSetStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

DataPointSetStepEditorParameter::DataPointSetStepEditorParameter(DataPointSetStepParameter& refParameter) :
InteractiveParameter( refParameter.basic ), 
m_refParameter( refParameter )
{
}

DataPointSetStepEditorParameter::~DataPointSetStepEditorParameter()
{
}

bool DataPointSetStepEditorParameter::failureIgnorable()
{
    return true;
}

bool DataPointSetStepEditorParameter::skippable()
{
    return true;
}

bool DataPointSetStepEditorParameter::delayable()
{
    return true;
}

bool DataPointSetStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void DataPointSetStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamDataPointValid = !m_refParameter.dataPoint.empty();
	bool ParamNewValueValid = false;

	std::string valueNewValueType = m_datapointEditorHelper.getTypeDescription( m_refParameter.newValueType );

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.dataPoint, ParamDataPoint, ParamDataPoint, eTree, ParamDataPointValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueNewValueType, ParamNewValueType, ParamNewValueType, eReadOnly, true);
	if(( BOOLEAN_DATA_POINT == m_refParameter.newValueType ) || ( COMBINED_DATA_POINT == m_refParameter.newValueType ))
	{
        std::string displayValue = m_refParameter.newValue;
        m_datapointEditorHelper.getDataPointDisplayValue( m_refParameter.dataPoint, m_refParameter.newValueType, m_refParameter.newValue, displayValue );
		ParamNewValueValid = !displayValue.empty();
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, displayValue, ParamNewValue, ParamNewValue, eList, ParamNewValueValid);
	}
	else if( NUMERIC_DATA_POINT == m_refParameter.newValueType )
	{
		ParamNewValueValid = !m_refParameter.newValue.empty();
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.newValue, ParamNewValue, ParamNewValue, eInteger, ParamNewValueValid);
	}
	else
	{
		ParamNewValueValid = true;
        EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.newValue, ParamNewValue, ParamNewValue, eString, ParamNewValueValid);
	}
}

IStringEditor* DataPointSetStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool DataPointSetStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if( 0 == strName.compare( ParamNewValue ))
    {
        convertToActualValue( strDisplayValue, m_refParameter.newValue );
        bValid = !strDisplayValue.empty();
    }
    // No parameter list changed
    return false;
}

ITreePicker* DataPointSetStepEditorParameter::getTreePicker(const std::string& strName)
{
    return this;
}

void DataPointSetStepEditorParameter::getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
    if( 0 == strName.compare( ParamDataPoint ))
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

void DataPointSetStepEditorParameter::getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId)
{
    if(strName == ParamDataPoint)
    {
        vecKeyItems = m_datapointEditorHelper.getChildDataPointItems(unNodeId);
    }
}

bool DataPointSetStepEditorParameter::pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    bool bListUpdated = false;

    if( 0 == strName.compare( ParamDataPoint ))
    {
        if ( !vecSelectedKeys.empty() )
        {
            const std::string strPrevDpName = m_refParameter.dataPoint;

            m_datapointEditorHelper.getDataPointInfo( vecSelectedKeys.front(), m_refParameter.dataPoint, m_refParameter.newValue, m_refParameter.newValueType );

            strDisplayValue = m_refParameter.dataPoint;

            bListUpdated = ( 0 != strPrevDpName.compare( m_refParameter.dataPoint ));
        }
    }

    return bListUpdated;
}

IListPicker* DataPointSetStepEditorParameter::getListPicker( const std::string& strName )
{
	return this;
}

void DataPointSetStepEditorParameter::getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */ )
{
    if ( 0 == strName.compare( ParamNewValue ))
    {
        unsigned int unValueSelected = 0u;
        m_datapointEditorHelper.getDataPointValues( m_refParameter.dataPoint, m_refParameter.newValueType, m_refParameter.newValue, vecKeyItems, unValueSelected );

        vecSelected.push_back( unValueSelected );

        nMaxSelection = 1;
    }
}

bool DataPointSetStepEditorParameter::pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */ )
{
    if ( 0 == strName.compare( ParamNewValue ))
    {
        if ( !vecSelectedKeys.empty() )
        {
            m_datapointEditorHelper.getDataPointValue( m_refParameter.dataPoint, m_refParameter.newValueType, vecSelectedKeys.front(), m_refParameter.newValue, strDisplayValue );
        }
    }

    return false;
}

void DataPointSetStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if( 0 == strName.compare( ParamNewValue ))
    {
        convertToDisplayValue( m_refParameter.newValue, strCurValue );
		szMaxLength = ( NUMERIC_DATA_POINT == m_refParameter.newValueType ) ? 10u : 40u;
    }
}

}
