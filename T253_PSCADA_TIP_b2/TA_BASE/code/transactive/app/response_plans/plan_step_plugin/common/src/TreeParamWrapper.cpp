#include "TreeParamWrapper.h"


TreeParamWrapper::TreeParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, ITreePicker* pPicker, std::auto_ptr<ICompareRule> ptrDisplayRule ) :
m_strUniqueName( strUniqueName.c_str() ),
m_strDisplayName( strDisplayName.c_str() ),
m_strDisplayValue( "" ),
m_ptrDisplayRule( ptrDisplayRule ),
m_ptrValidateRule(),
m_pPicker( pPicker )
{
    updateDisplayValue();
}

TreeParamWrapper::~TreeParamWrapper()
{
}

bool TreeParamWrapper::valid()
{
    bool bValid = true;

    if ( NULL != m_ptrValidateRule.get() )
    {
        bValid = m_ptrValidateRule->result();
    }

    return bValid;
}

bool TreeParamWrapper::show()
{
    bool bShow = true;

    if ( NULL != m_ptrDisplayRule.get() )
    {
        bShow = m_ptrDisplayRule->result();
    }

    return true;
}

int TreeParamWrapper::type()
{
    return eTree;
}

std::string TreeParamWrapper::displayValue()
{
    return m_strDisplayValue;
}

std::string TreeParamWrapper::displayName()
{
    return m_strDisplayName;
}

std::string TreeParamWrapper::uniqueName()
{
    return m_strUniqueName;
}

void TreeParamWrapper::updateDisplayValue()
{
    m_strDisplayValue = "";
}

void TreeParamWrapper::getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */ )
{

}

void TreeParamWrapper::getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId )
{

}

bool TreeParamWrapper::pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */ )
{
    return false;
}
