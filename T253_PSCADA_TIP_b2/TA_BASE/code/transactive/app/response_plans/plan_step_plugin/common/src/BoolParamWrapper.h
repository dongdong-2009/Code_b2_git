#ifndef BOOLPARAMWRAPPER_INCLUDE
#define BOOLPARAMWRAPPER_INCLUDE

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

class BoolParamWrapper : public IParameterWrapper, IBoolEditor
{
public:
    BoolParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc,
        const std::string& strTrueLabel, const std::string& strFalseLabel, 
        std::auto_ptr<ICompareRule> ptrDisplayRule = NullCompareRule, std::auto_ptr<ICompareRule> ptrValidRule = NullCompareRule );
    virtual ~BoolParamWrapper();

    virtual std::string uniqueName();
    virtual std::string displayName();
    virtual std::string displayValue();
    virtual int type();
    virtual bool show();
    virtual bool valid();

    // IBoolEditor
    virtual bool updateBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getBooleanValue( const std::string& strName, bool& bValue, const unsigned int nRow = 0 );

private:
    void updateDisplayValue();

private:
    std::string m_strUniqueName;
    std::string m_strDisplayName;
    std::string m_strDisplayValue;
    std::string m_strTrueLabel;
    std::string m_strFalseLabel;
    bool& m_refSrc;
    std::auto_ptr<ICompareRule> m_ptrValidateRule;
    std::auto_ptr<ICompareRule> m_ptrDisplayRule;
};

#endif
