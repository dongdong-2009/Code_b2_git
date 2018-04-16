#include "app/response_plans/plan_manager/src/ExclusiveGatewayEditorParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

ExclusiveGatewayExpressionPairsGridEditor::ExclusiveGatewayExpressionPairsGridEditor( std::vector<ExpressionPair>& stepExpressionPairs, IPlanNode& refPlanNode ) :
m_stepExpressionPairs( stepExpressionPairs ), 
m_stepEditorHelper( refPlanNode )
{
}

ExclusiveGatewayExpressionPairsGridEditor::~ExclusiveGatewayExpressionPairsGridEditor()
{
}

void ExclusiveGatewayExpressionPairsGridEditor::getGridItems( const std::string& strName, std::vector<GridColumn>& vecColumns, std::vector<GridItem>& vecKeyItems, bool& bRowFixed, const unsigned int nRow /*= 0 */ )
{
	if ( strName.compare( ParamStepExpressionPairs ) != 0 )
	{
		return;
	}

	GridColumn expressionCol = {eString, ParamPairExpression, ParamPairExpression};
	GridColumn nextStepCol = {eList, ParamPairNextStep, ParamPairNextStep};
	vecColumns.push_back(expressionCol);
	vecColumns.push_back(nextStepCol);

	int row = 0;
	for(std::vector<ExpressionPair>::const_iterator it = m_stepExpressionPairs.begin(); it != m_stepExpressionPairs.end(); ++it)
	{
		GridItem itemExpression = {row, 0, (*it).expression};
		GridItem itemNextStep = {row, 1, m_stepEditorHelper.getItemById((*it).nextStep).Item};
		vecKeyItems.push_back(itemExpression);
		vecKeyItems.push_back(itemNextStep);
		row++;
	}

    // expression pairs can only be new/delete by sequence flow editing
    bRowFixed = true;
}

IStringEditor* ExclusiveGatewayExpressionPairsGridEditor::getStringEditor( const std::string& strName )
{
	return this;
}

IListPicker* ExclusiveGatewayExpressionPairsGridEditor::getListPicker( const std::string& strName )
{
	return this;
}

bool ExclusiveGatewayExpressionPairsGridEditor::updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/ )
{
    if ( nRow >= m_stepExpressionPairs.size())
    {
        // Log Error
        return false;
    }

    if ( strName.compare( ParamPairExpression ) == 0 )
    {
		m_stepExpressionPairs[nRow].expression = strDisplayValue;
        bValid = true;
    }

    return false;
}

void ExclusiveGatewayExpressionPairsGridEditor::getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/ )
{
    vecKeyItems = m_stepEditorHelper.getSteps();
    nMaxSelection = 1;
    if ( nRow >= m_stepExpressionPairs.size() )
    {
        // Log Error
        return;
    }

    if ( strName.compare( ParamPairNextStep ) == 0 )
    {
        if ( m_stepExpressionPairs[nRow].nextStep != 0 )
        {
            ListItem item = m_stepEditorHelper.getItemById( m_stepExpressionPairs[nRow].nextStep );
            if ( item.ID != m_stepEditorHelper.INVALID_ID )
            {
                vecSelected.push_back( item.ID );
            }
        }
    }
}

bool ExclusiveGatewayExpressionPairsGridEditor::pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/ )
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }

    if ( nRow >= m_stepExpressionPairs.size() )
    {
        // Log Error
        return false;
    }

    if ( strName.compare( ParamPairNextStep ) == 0 )
    {
        m_stepExpressionPairs[nRow].nextStep = vecSelectedKeys[0];
        strDisplayValue = m_stepEditorHelper.getItemById( vecSelectedKeys[0] ).Item;
    }

    return false;
}

bool ExclusiveGatewayExpressionPairsGridEditor::deleteRow( const unsigned int nRow )
{
	if(m_stepExpressionPairs.size() > nRow)
	{
		m_stepExpressionPairs.erase(m_stepExpressionPairs.begin() + nRow);
	}
	return false;
}

bool ExclusiveGatewayExpressionPairsGridEditor::newRow()
{
	m_stepExpressionPairs.push_back(ExpressionPair());
	return false;
}

std::vector<ExpressionPair> ExclusiveGatewayExpressionPairsGridEditor::getExclusiveGatewayExpressionPairs()
{
	return m_stepExpressionPairs;
}

PlanStepEditorHelper& ExclusiveGatewayExpressionPairsGridEditor::getPlanStepEditorHelper()
{
    return m_stepEditorHelper;
}

void ExclusiveGatewayExpressionPairsGridEditor::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if ( nRow >= m_stepExpressionPairs.size())
    {
		return;
    }

    if ( 0 == strName.compare( ParamPairExpression ) )
    {
		strCurValue = m_stepExpressionPairs[nRow].expression;
		szMaxLength = 800;
    }
}

//////////////////////////////////////////////////////////////////////////
///ExclusiveGatewayEditorParameter
//////////////////////////////////////////////////////////////////////////
ExclusiveGatewayEditorParameter::ExclusiveGatewayEditorParameter(ConditionalGatewayParameter& refParameter, IPlanNode& refPlanNode) :
InteractiveParameter( refParameter.basic ),
m_expressionPairsGridEditor( refParameter.stepExpressionPairs, refPlanNode ), 
m_refPairStep( refParameter.pairStep )
{
}

ExclusiveGatewayEditorParameter::~ExclusiveGatewayEditorParameter()
{
}

void ExclusiveGatewayEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
    // Vector Empty - valid; Vector Not Empty : Expression empty - invalid, step empty - invalid
	bool ParamStepExpressionPairsValid = true;
    bool ParamPairStepValid = ( 0u != m_refPairStep );

	std::string valueExpressionSteps = "";

	std::vector<ExpressionPair> expressionPairs = m_expressionPairsGridEditor.getExclusiveGatewayExpressionPairs();
	for (std::vector<ExpressionPair>::iterator itPair = expressionPairs.begin(); itPair != expressionPairs.end(); ++itPair)
    {
		if ( !itPair->expression.empty() && ( 0u != itPair->nextStep ))
		{
            valueExpressionSteps += (itPair->expression + " -> " + m_expressionPairsGridEditor.getPlanStepEditorHelper().getItemById(itPair->nextStep).Item + "; ");
		}
        else if ( ParamStepExpressionPairsValid && ( expressionPairs.size() > 1u ))
        {
            ParamStepExpressionPairsValid = false;
        }
    }

	if(!valueExpressionSteps.empty())
	{
		valueExpressionSteps.resize(valueExpressionSteps.size() - 2u);
	}

    std::string valuePairStep = m_expressionPairsGridEditor.getPlanStepEditorHelper().getItemById( m_refPairStep ).Item;

    ItemPair itmNameValue;
    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, valueExpressionSteps, ParamStepExpressionPairs, ParamStepExpressionPairs, eGrid, ParamStepExpressionPairsValid );
    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, valuePairStep, ParamPairStep, ParamPairStep, eReadOnly, ParamPairStepValid );
}

bool ExclusiveGatewayEditorParameter::failureIgnorable()
{
    return false;
}

bool ExclusiveGatewayEditorParameter::skippable()
{
    return false;
}

bool ExclusiveGatewayEditorParameter::delayable()
{
    return false;
}

bool ExclusiveGatewayEditorParameter::responseRequired()
{
    return false;
}

IGridEditor* ExclusiveGatewayEditorParameter::getGridEditor( const std::string& strName )
{
	return &m_expressionPairsGridEditor;
}

}