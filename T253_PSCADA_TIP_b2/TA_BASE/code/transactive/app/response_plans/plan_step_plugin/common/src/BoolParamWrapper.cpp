#include "BoolParamWrapper.h"


BoolParamWrapper::BoolParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, 
    bool& refSrc, const std::string& strTrueLabel, const std::string& strFalseLabel, 
    std::auto_ptr<ICompareRule> ptrDisplayRule, std::auto_ptr<ICompareRule> ptrValidRule ) :
m_strUniqueName( strUniqueName.c_str() ),
m_strDisplayName( strDisplayName.c_str() ),
m_strDisplayValue( "" ),
m_refSrc( refSrc ),
m_ptrValidateRule( ptrValidRule ),
m_ptrDisplayRule( ptrDisplayRule ),
m_strTrueLabel( strTrueLabel.c_str() ),
m_strFalseLabel( strTrueLabel.c_str() )
{
    updateDisplayValue();
}

BoolParamWrapper::~BoolParamWrapper()
{
}

void BoolParamWrapper::getBooleanValue( const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */ )
{
    bValue = m_refSrc;
}

bool BoolParamWrapper::updateBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */ )
{
    bValid = (m_ptrValidateRule.get() != NULL) ? m_ptrValidateRule->result() : true;

    bool bRefresh = (m_refSrc != bValue);

    m_refSrc = bValue;

    updateDisplayValue();

    strDisplayValue = m_strDisplayValue;

    return bRefresh;
}

bool BoolParamWrapper::valid()
{
    bool bValid = true;

    if ( NULL != m_ptrValidateRule.get() )
    {
        bValid = m_ptrValidateRule->result();
    }

    return bValid;
}

bool BoolParamWrapper::show()
{
    bool bShow = true;

    if ( NULL != m_ptrDisplayRule.get() )
    {
        bShow = m_ptrDisplayRule->result();
    }

    return bShow;
}

int BoolParamWrapper::type()
{
    return eBoolean;
}

std::string BoolParamWrapper::displayValue()
{
    return m_strDisplayValue;
}

std::string BoolParamWrapper::displayName()
{
    return m_strDisplayName;
}

std::string BoolParamWrapper::uniqueName()
{
    return m_strUniqueName;
}

void BoolParamWrapper::updateDisplayValue()
{
    m_strDisplayValue = m_refSrc ? m_strTrueLabel : m_strFalseLabel;
}
