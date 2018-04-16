#include "GridParamWrapper.h"


GridParamWrapper::GridParamWrapper( const std::string& strUniqueName, const std::string& strDisplayName, IGridEditor* pEditor, std::auto_ptr<ICompareRule> ptrDisplayRule ) :
m_strUniqueName( strUniqueName.c_str() ),
m_strDisplayName( strDisplayName.c_str() ),
m_strDisplayValue( "" ),
m_ptrDisplayRule( ptrDisplayRule ),
m_ptrValidateRule(),
m_pEditor( pEditor )
{
    updateDisplayValue();
}

GridParamWrapper::~GridParamWrapper()
{
}

bool GridParamWrapper::valid()
{
    bool bValid = true;

    if ( NULL != m_ptrValidateRule.get() )
    {
        bValid = m_ptrValidateRule->result();
    }

    return bValid;
}

bool GridParamWrapper::show()
{
    bool bShow = true;

    if ( NULL != m_ptrDisplayRule.get() )
    {
        bShow = m_ptrDisplayRule->result();
    }

    return true;
}

int GridParamWrapper::type()
{
    return eTree;
}

std::string GridParamWrapper::displayValue()
{
    return m_strDisplayValue;
}

std::string GridParamWrapper::displayName()
{
    return m_strDisplayName;
}

std::string GridParamWrapper::uniqueName()
{
    return m_strUniqueName;
}

void GridParamWrapper::updateDisplayValue()
{
    m_strDisplayValue = "";
}

void GridParamWrapper::getGridItems(const std::string& strName, std::vector<GridColumn>& vecColumns, std::vector<GridItem>& vecKeyItems, bool& bRowFixed, const unsigned int nRow /*= 0 */)
{

}

bool GridParamWrapper::deleteRow(const unsigned int nRow)
{
	return false;
}

bool GridParamWrapper::newRow()
{
	return false;
}