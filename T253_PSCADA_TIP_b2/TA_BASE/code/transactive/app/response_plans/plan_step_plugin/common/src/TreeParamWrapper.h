#ifndef TREEPARAMWRAPPER_INCLUDE
#define TREEPARAMWRAPPER_INCLUDE

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

class TreeParamWrapper : public IParameterWrapper, ITreePicker
{
public:
    TreeParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, ITreePicker* pPicker = NULL, std::auto_ptr<ICompareRule> ptrDisplayRule = NullCompareRule );
    virtual ~TreeParamWrapper();

    virtual std::string uniqueName();
    virtual std::string displayName();
    virtual std::string displayValue();
    virtual int type();
    virtual bool show();
    virtual bool valid();

    // ITreePicker
    virtual void getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 );
    virtual void getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId );
    virtual bool pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 );

private:
    void updateDisplayValue();

private:
    std::string m_strUniqueName;
    std::string m_strDisplayName;
    std::string m_strDisplayValue;
    std::auto_ptr<ICompareRule> m_ptrValidateRule;
    std::auto_ptr<ICompareRule> m_ptrDisplayRule;
    ITreePicker* m_pPicker;
};

#endif
