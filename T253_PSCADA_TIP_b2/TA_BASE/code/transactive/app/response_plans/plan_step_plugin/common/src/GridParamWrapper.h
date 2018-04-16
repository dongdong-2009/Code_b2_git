#ifndef GRIDPARAMWRAPPER_INCLUDE
#define GRIDPARAMWRAPPER_INCLUDE

#include <string>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "bus/response_plans/PlanDataAccess/src/Serialization.h"
#include "ParameterType.h"
#include "ICompareRule.h"
#include "IParameterWrapper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"

class GridParamWrapper : public IParameterWrapper, IGridEditor
{
public:
    GridParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, IGridEditor* pEditor = NULL, std::auto_ptr<ICompareRule> ptrDisplayRule = NullCompareRule );
    virtual ~GridParamWrapper();

    virtual std::string uniqueName();
    virtual std::string displayName();
    virtual std::string displayValue();
    virtual int type();
    virtual bool show();
    virtual bool valid();

    // IGridEditor
    virtual void getGridItems(const std::string& strName, std::vector<GridColumn>& vecColumns, std::vector<GridItem>& vecKeyItems, bool& bRowFixed, const unsigned int nRow = 0);
	virtual bool deleteRow(const unsigned int nRow);
	virtual bool newRow();

private:
    void updateDisplayValue();

private:
    std::string m_strUniqueName;
    std::string m_strDisplayName;
    std::string m_strDisplayValue;
    std::auto_ptr<ICompareRule> m_ptrValidateRule;
    std::auto_ptr<ICompareRule> m_ptrDisplayRule;
    IGridEditor* m_pEditor;
};

#endif
