#include <sstream>
#include <boost/foreach.hpp>

#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointConfigAccess.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"
#include <ace/Guard_T.h>

namespace TA_Base_App
{

const unsigned int DataPointEditorHelper::INVALID_ID = -1;
TreeItemCache<TA_Base_Core::IEntityData> DataPointEditorHelper::s_dataPoints;
ACE_RW_Mutex DataPointEditorHelper::s_mtxDataPoints;

DataPointEditorHelper::DataPointEditorHelper( const bool bIdAsValue ) : 
m_bIdAsValue( bIdAsValue )
{
	if(s_dataPoints.isEmpty())
	{
		ACE_Write_Guard<ACE_RW_Mutex> gudWrite(s_mtxDataPoints);
		if(s_dataPoints.isEmpty())
		{
		    TA_Base_Core::IEntityDataList roots;

		    try
		    {
		        roots = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("SCADAROOT");
		    }
		    catch(...)
		    {
		    }

		    BOOST_FOREACH( TA_Base_Core::IEntityData *pData, roots )
			{
		        if ( NULL == pData )
		        {
		            continue;
		        }

		        TreeItem treeItem = { true, pData->getKey(), pData->getKey(), pData->getName() };
		        s_dataPoints.insertTreeItem( treeItem, pData, true );
			}
		}
	}

}

DataPointEditorHelper::~DataPointEditorHelper()
{
}

std::string DataPointEditorHelper::getDisplayName(const std::string& fullName)
{
    std::string newName(fullName);

    int position = fullName.find_last_of('.');

    if (position!=-1)
    {
        newName = fullName.substr(position + 1, fullName.length() - position - 1);
    }

    return newName;
}

std::vector<TreeItem> DataPointEditorHelper::getRootDataPointItems()
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    return s_dataPoints.getRootItems();
}

std::vector<TreeItem> DataPointEditorHelper::getChildDataPointItems(const unsigned int id)
{
	ACE_Write_Guard<ACE_RW_Mutex> gudWrite(s_mtxDataPoints);
	if( s_dataPoints.hasChild( id ))
	{
		return s_dataPoints.getChildItems( id );
	}

	TA_Base_Core::IEntityData* parent = s_dataPoints.getUserDataByItemId( id );
	if( NULL == parent )
	{
		return std::vector<TreeItem>();
	}

    TA_Base_Core::IEntityDataList childs;
    try
    {
        childs = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf( parent->getKey() );
    }
    catch(...)
    {
        // TODO
    }

    BOOST_FOREACH(TA_Base_Core::IEntityData *pData, childs)
    {
        bool hasChild = (TA_Base_Core::DataNodeEntityData::getStaticType() == pData->getType());
        TreeItem treeItem = { hasChild, pData->getKey(), id, getDisplayName(pData->getName()) };
        s_dataPoints.insertTreeItem( treeItem, pData, false );
    }

    return s_dataPoints.getChildItems(id);
}

unsigned int DataPointEditorHelper::getItemIdByDataPointName(const std::string& name)
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    unsigned int unId = s_dataPoints.getItemIdByUserData( name, DpNameEqual() );

    if ( unId == s_dataPoints.INVALID_ID )
    {
        try
        {
            std::auto_ptr<TA_Base_Core::IEntityData> ptrData( TA_Base_Core::EntityAccessFactory::getInstance().getEntity( name ));

            unId = ptrData->getKey();
        }
        catch(...)
        {
        }
    }

    return unId;
}

std::string DataPointEditorHelper::getDataPointNameByItemId(const unsigned int id)
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    TA_Base_Core::IEntityData* tmp = s_dataPoints.getUserDataByItemId(id);
    if(NULL != tmp)
    {
        return tmp->getName();
    }
    return "";
}

TA_Base_Core::DataPointEntityData* DataPointEditorHelper::getDataPointEntityById( const unsigned int id )
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
	return dynamic_cast<TA_Base_Core::DataPointEntityData*>(s_dataPoints.getUserDataByItemId(id));
}

void DataPointEditorHelper::getDataPointValues( const unsigned int id, const int nType, const std::string& strValue, std::vector<ListItem>& vecItems, unsigned int& unValueId )
{
    vecItems.clear();

    std::stringstream stmFormator;
    if ( COMBINED_DATA_POINT == nType )
    {
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( id );

        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            stmFormator.clear();
            stmFormator.str( "" );
            stmFormator << itLoop->state << " : " << itLoop->description;
            
            ListItem stItem = { 0u, "" };
            vecItems.push_back( stItem );
            vecItems.back().ID = static_cast<unsigned int>(itLoop->state);
            vecItems.back().Item = stmFormator.str().c_str();

            if ( 0 == strValue.compare( itLoop->description ))
            {
                unValueId = itLoop->state;
            }
        }
    }
    else if ( BOOLEAN_DATA_POINT == nType )
    {
		ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
        TA_Base_Core::DataPointEntityData* pData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(s_dataPoints.getUserDataByItemId( id ));

        if ( NULL != pData )
        {
            ListItem stItem = { 0u, "" };

            stmFormator.clear();
            stmFormator.str( "" );

            std::string strFalseLabel = getLabel( *pData, false );
            stmFormator << "0 : " << strFalseLabel;

            vecItems.push_back(stItem);
            vecItems.back().ID = 0u;
            vecItems.back().Item = stmFormator.str().c_str();

            stmFormator.clear();
            stmFormator.str( "" );

            std::string strTrueLabel = getLabel( *pData, true );
            stmFormator << "1 : " << strTrueLabel;

            vecItems.push_back(stItem);
            vecItems.back().ID = 1u;
            vecItems.back().Item = stmFormator.str().c_str();

            if ( 0 == strValue.compare( strFalseLabel ))
            {
                unValueId = 0u;
            }
            else if ( 0 == strValue.compare( strTrueLabel ))
            {
                unValueId = 1u;
            }
        }
    }
}

void DataPointEditorHelper::getDataPointValues(const std::string& strName, const int nType, const std::string& strValue, std::vector<ListItem>& vecItems, unsigned int& unValueId)
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    unsigned int unId = s_dataPoints.getItemIdByUserData( strName, DpNameEqual() );

    if ( unId != s_dataPoints.INVALID_ID )
    {
        getDataPointValues( unId, nType, strValue, vecItems, unValueId );
    }
    else
    {
        vecItems.clear();

        std::stringstream stmFormator;
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( strName );

        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            stmFormator.clear();
            stmFormator.str( "" );
            stmFormator << itLoop->state << " : " << itLoop->description;
            
            ListItem stItem = { 0u, "" };
            vecItems.push_back( stItem );
            vecItems.back().ID = static_cast<unsigned int>(itLoop->state);
            vecItems.back().Item = stmFormator.str().c_str();

            if ( m_bIdAsValue && ( itLoop->state == TA_Base_Core::DataConversion::toInteger( strValue )))
            {
                unValueId = itLoop->state;
            }
            else if ( !m_bIdAsValue && ( 0 == strValue.compare( itLoop->description )))
            {
                unValueId = itLoop->state;
            }
        }
    }
}

void DataPointEditorHelper::getDataPointValue( const unsigned int id, const int nType, const unsigned int unValueId, std::string& strValue, std::string& strDispValue )
{
    std::stringstream stmFormator;

    if ( COMBINED_DATA_POINT == nType )
    {
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( id );

        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            if ( unValueId == itLoop->state )
            {
                strValue = m_bIdAsValue ? TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(unValueId) ).c_str() : itLoop->description.c_str();
                stmFormator << itLoop->state << " : " << itLoop->description;
                strDispValue = stmFormator.str().c_str();

                break;
            }
        }
    }
    else if ( BOOLEAN_DATA_POINT == nType )
    {
		ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
        TA_Base_Core::DataPointEntityData* pData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(s_dataPoints.getUserDataByItemId( id ));

        if ( NULL != pData )
        {
            strValue = m_bIdAsValue ? TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(unValueId) ).c_str() : getLabel( *pData, ( 0u != unValueId ));

            stmFormator << (( 0u == unValueId ) ? "0 : " : "1 : " ) << strValue;
            
            strDispValue = stmFormator.str().c_str();
        }
    }
}

void DataPointEditorHelper::getDataPointValue(const std::string& strName, const int nType, const unsigned int unValueId, std::string& strValue, std::string& strDispValue)
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    unsigned int unId = s_dataPoints.getItemIdByUserData( strName, DpNameEqual() );

    if ( unId != s_dataPoints.INVALID_ID )
    {
        getDataPointValue( unId, nType, unValueId, strValue, strDispValue );
    }
    else
    {
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( strName );

        std::stringstream stmFormator;
        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            if ( unValueId == itLoop->state )
            {
                strValue = m_bIdAsValue ? TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(unValueId) ).c_str() : itLoop->description.c_str();
                stmFormator << itLoop->state << " : " << itLoop->description;
                strDispValue = stmFormator.str().c_str();

                break;
            }
        }
    }
}

void DataPointEditorHelper::getDataPointDisplayValue( const unsigned int id, const int nType, const std::string& strValue, std::string& strDispValue )
{
    std::stringstream stmFormator;

    if ( COMBINED_DATA_POINT == nType )
    {
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( id );

        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            if (( m_bIdAsValue && ( itLoop->state == TA_Base_Core::DataConversion::toInteger( strValue ))) || 
                ( !m_bIdAsValue && ( 0 == strValue.compare( itLoop->description ))))
            {
                stmFormator << itLoop->state << " : " << itLoop->description;
                strDispValue = stmFormator.str().c_str();

                break;
            }
        }
    }
    else if ( BOOLEAN_DATA_POINT == nType )
    {
		ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
        TA_Base_Core::DataPointEntityData* pData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(s_dataPoints.getUserDataByItemId( id ));

        if ( NULL != pData )
        {
            std::string strFalseLabel = getLabel( *pData, false );
            std::string strTrueLabel = getLabel( *pData, true );

            if (( m_bIdAsValue && ( 0 == TA_Base_Core::DataConversion::toInteger( strValue ))) || 
                ( !m_bIdAsValue && ( 0 == strValue.compare( strFalseLabel ))))
            {
                stmFormator << "0 : " << strFalseLabel;
            }
            else
            {
                stmFormator << "1 : " << strTrueLabel;
            }
                        
            strDispValue = stmFormator.str().c_str();
        }
    }
}

void DataPointEditorHelper::getDataPointDisplayValue( const std::string& strName, const int nType, const std::string& strValue, std::string& strDispValue )
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    unsigned int unId = s_dataPoints.getItemIdByUserData( strName, DpNameEqual() );

    if ( unId != s_dataPoints.INVALID_ID )
    {
        getDataPointDisplayValue( unId, nType, strValue, strDispValue );
    }
    else
    {
        std::stringstream stmFormator;
    
        DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( strName );

        for ( DataPointConfigAccess::StateValueList::iterator itLoop = vecValues.begin(); vecValues.end() != itLoop; ++itLoop )
        {
            if (( m_bIdAsValue && ( itLoop->state == TA_Base_Core::DataConversion::toInteger( strValue ))) || 
                ( !m_bIdAsValue && ( 0 == strValue.compare( itLoop->description ))))
            {
                stmFormator << itLoop->state << " : " << itLoop->description;
                strDispValue = stmFormator.str().c_str();

                break;
            }
        }
    }
}

void DataPointEditorHelper::getDataPointInfo( const unsigned int unId, std::string& strName, std::string& strValue, int& nType )
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(s_mtxDataPoints);
    TA_Base_Core::DataPointEntityData* pData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(s_dataPoints.getUserDataByItemId( unId ));

    strName = "";
    strValue = "";
    nType = UNDEFINED_DATA_POINT;

    if ( NULL != pData )
    {
        strName = pData->getName().c_str();

        std::string strType = pData->getDataType();
        nType = m_oValueTypeHelper.getTypeByScadaType( strType );

        if ( COMBINED_DATA_POINT == nType )
        {
            DataPointConfigAccess::StateValueList vecValues = DataPointConfigAccess::getDataPointStates( pData->getKey() );

            if ( !vecValues.empty() )
            {
                strValue = m_bIdAsValue ? TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(vecValues.front().state) ).c_str() : vecValues.front().description.c_str();
            }
        }
        else if ( BOOLEAN_DATA_POINT == nType )
        {
            // Default false value
            strValue = m_bIdAsValue ? "0" : getLabel(*pData, false);
        }
    }
}

std::string DataPointEditorHelper::getLabel( TA_Base_Core::DataPointEntityData& refData, const bool bValue )
{
    std::string strLabel = "";

    try
    {
        strLabel = bValue ? refData.getTrueLabel() : refData.getFalseLabel();
    }
    catch ( ... )
    {
    }

    // Cannot use custom label, otherwise could not send correct value
    //if ( strLabel.empty() )
    //{
    //    strLabel = bValue ? "True" : "False";
    //}

    return strLabel;
}

std::string DataPointEditorHelper::getTypeDescription(const int nType)
{
    return m_oValueTypeHelper.getTypeDescription( nType );
}


}
