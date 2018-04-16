#ifndef STRINGPARAMWRAPPER_INCLUDE
#define STRINGPARAMWRAPPER_INCLUDE

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

template<class BasicType>
class StringParamWrapper : public IParameterWrapper, IStringEditor
{
public:
    StringParamWrapper( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc,
        std::auto_ptr<ICompareRule> ptrValidateRule = NullCompareRule, std::auto_ptr<ICompareRule> ptrDisplayRule = NullCompareRule );
    virtual ~StringParamWrapper();

    virtual std::string uniqueName();
    virtual std::string displayName();
    virtual std::string displayValue();
    virtual int type();
    virtual bool show();
    virtual bool valid();

    // IStringEditor
    virtual bool updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );

    void convertToDisplayValue( const BasicType& refSrc, std::string& strDisplayValue );
    void convertToActualValue( const std::string& strDisplayValue, BasicType& refSrc );

private:
    std::string m_strUniqueName;
    std::string m_strDisplayName;
    std::string m_strDisplayValue;
    int m_nInputType;
    BasicType& m_refSrc;
    std::auto_ptr<ICompareRule> m_ptrValidateRule;
    std::auto_ptr<ICompareRule> m_ptrDisplayRule;
};

template<class BasicType>
StringParamWrapper<BasicType>::StringParamWrapper( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
    BasicType& refSrc, std::auto_ptr<ICompareRule> ptrValidateRule, std::auto_ptr<ICompareRule> ptrDisplayRule ) :
    m_strUniqueName( strUniqueName.c_str() ),
    m_strDisplayName( strDisplayName.c_str() ),
    m_strDisplayValue( "" ),
    m_nInputType( nInputType ),
    m_refSrc( refSrc ),
    m_ptrValidateRule( ptrValidateRule ),
    m_ptrDisplayRule( ptrDisplayRule )
{
    convertToDisplayValue( m_refSrc, m_strDisplayValue );
}

template<class BasicType>
StringParamWrapper<BasicType>::~StringParamWrapper()
{
}

template<typename BasicType>
void StringParamWrapper<BasicType>::convertToActualValue( const std::string& strDisplayValue, BasicType& refSrc )
{
    std::istringstream ostConvert( strDisplayValue );

    ostConvert >> refSrc;
}

template<typename BasicType>
void StringParamWrapper<BasicType>::convertToDisplayValue( const BasicType& refSrc, std::string& strDisplayValue )
{
    std::stringstream ostConvert;

    ostConvert << refSrc;

    ostConvert >> strDisplayValue;
}

template<class BasicType>
bool StringParamWrapper<BasicType>::updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */ )
{
    bValid = (m_ptrValidateRule.get() != NULL) ? m_ptrValidateRule->result() : true;

    BasicType tpCopy = m_refSrc;

    convertToActualValue( strDisplayValue, m_refSrc );

    convertToDisplayValue( m_refSrc, m_strDisplayValue );

    return ( m_refSrc != tpCopy );
}

template<class BasicType>
bool StringParamWrapper<BasicType>::valid()
{
    bool bValid = true;

    if ( NULL != m_ptrValidateRule.get() )
    {
        bValid = m_ptrValidateRule->result();
    }

    return bValid;
}

template<class BasicType>
bool StringParamWrapper<BasicType>::show()
{
    bool bShow = true;

    if ( NULL != m_ptrDisplayRule.get() )
    {
        bShow = m_ptrDisplayRule->result();
    }

    return bShow;
}

template<class BasicType>
int StringParamWrapper<BasicType>::type()
{
    return m_nInputType;
}

template<class BasicType>
std::string StringParamWrapper<BasicType>::displayValue()
{
    return m_strDisplayValue;
}

template<class BasicType>
std::string StringParamWrapper<BasicType>::displayName()
{
    return m_strDisplayName;
}

template<class BasicType>
std::string StringParamWrapper<BasicType>::uniqueName()
{
    return m_strUniqueName;
}

#endif
