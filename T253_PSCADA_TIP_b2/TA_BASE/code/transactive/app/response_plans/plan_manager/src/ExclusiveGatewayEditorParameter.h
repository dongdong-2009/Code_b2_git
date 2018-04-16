#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"
#include "app/response_plans/plan_step_plugin/internal/src/ConditionalGatewayParameter.h"

namespace TA_Base_App
{

class IPlanNode;
class ExclusiveGatewayExpressionPairsGridEditor : public IGridEditor, public IStringEditor, public IListPicker
{
public:
	ExclusiveGatewayExpressionPairsGridEditor(std::vector<ExpressionPair>& stepExpressionPairs, IPlanNode& refPlanNode);
	~ExclusiveGatewayExpressionPairsGridEditor();
	// IGridEditor
    virtual void getGridItems( const std::string& strName, std::vector<GridColumn>& vecColumns, std::vector<GridItem>& vecKeyItems, bool& bRowFixed, const unsigned int nRow = 0 );
	virtual IStringEditor* getStringEditor( const std::string& strName );
    virtual IListPicker* getListPicker( const std::string& strName );
	virtual bool deleteRow(const unsigned int nRow);
	virtual bool newRow();
    // IStringEditor
    virtual bool updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );
    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);
    
	std::vector<ExpressionPair> getExclusiveGatewayExpressionPairs();
	PlanStepEditorHelper& getPlanStepEditorHelper();
	
private:
	std::vector<ExpressionPair>& m_stepExpressionPairs;
    PlanStepEditorHelper m_stepEditorHelper;
};

class ExclusiveGatewayEditorParameter : public InteractiveParameter
{
public:
    ExclusiveGatewayEditorParameter(ConditionalGatewayParameter& refParameter, IPlanNode& refPlanNode);
    ~ExclusiveGatewayEditorParameter();

    // InteractiveParameter
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
	virtual IGridEditor* getGridEditor(const std::string& strName);

protected:
    virtual bool failureIgnorable();
    virtual bool skippable();
    virtual bool delayable();
    virtual bool responseRequired();

private:
    ExclusiveGatewayExpressionPairsGridEditor m_expressionPairsGridEditor;
    unsigned int& m_refPairStep;
};

}
