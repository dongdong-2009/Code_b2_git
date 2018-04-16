/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataReader.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #6 $
  *
  *  Last modification: $DateTime: 2018/03/13 14:25:58 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"

#include "bus/response_plans/PlanDataAccess/src/MLSerialization.h"


namespace TA_Base_Bus
{
	PlanDataReadingClass::PlanDataReadingClass() : m_rootNodeId(TA_Base_Core::ROOT_NODE_ID), m_nvlValue(" ")
	{
	}

    std::map<unsigned long, int> PlanDataReadingClass::getPlanTypeMap()
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        //std::string strSql  = database->prepareSQLStatement(PLAN_TYPE_MAP_SELECT_80001);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_TYPE_MAP_SELECT_80001);

        std::vector<std::string> planTypeColumns;
        planTypeColumns.push_back( "FK_NODE_ID" );
        planTypeColumns.push_back( "FK_PLAN_TYPE" );

        TA_Base_Core::IData* planTyepData = database->executeQuery( strSql, planTypeColumns );
        PlanDataBaseHelper dataBaseHelper(database);

        std::map<unsigned long, int> typeMap;

        if( (NULL == planTyepData ) || (0 == planTyepData->getNumRows()) )
        {
            return typeMap;
        }

        do
        {
            std::auto_ptr<TA_Base_Core::IData> planTyepDataPtr(planTyepData);
            for( unsigned long dataIdx = 0; dataIdx < planTyepData->getNumRows(); dataIdx++ )
            {
                typeMap[planTyepDataPtr->getUnsignedLongData(dataIdx, planTypeColumns[0])] = static_cast<TA_Base_Core::EPlanType>(planTyepDataPtr->getUnsignedLongData(dataIdx, planTypeColumns[1]));
            }
            planTyepDataPtr.reset();
        }
        while ( database->moreData( planTyepData ) );

        return typeMap;
    }

 	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getRootNodeChildren( void ) const
    {
        return getChildren(m_rootNodeId);
    }

	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getChildren( unsigned long parent ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = node_tree.p_key"
			<< " and node_tree.parent_node = " << parent;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_SUMMARY_STD_SELECT_64001, parent);
//		std::string strSql  = database->prepareSQLStatement(NODE_SUMMARY_SELECT_64001, parent);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_SUMMARY_SELECT_64001, parent);
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		TA_Base_Core::IData* summaryData = database->executeQuery( strSql, summaryColumns );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::SiblingNodes_var sn = new TA_Base_Core::SiblingNodes;
				
		if( (NULL == summaryData ) || (0 == summaryData->getNumRows()) )
        {
			sn->length( 0 );
			return( sn._retn() );
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{

			std::auto_ptr<TA_Base_Core::IData> summaryDataPtr(summaryData);
			sn->length( summaryData->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < summaryData->getNumRows(); dataIdx++ )
			{

				sn[rowsFetched + dataIdx].node                    = summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[0] );
				sn[rowsFetched + dataIdx].parent                  = summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[1] );
				sn[rowsFetched + dataIdx].name                    = summaryDataPtr->getStringData( dataIdx, summaryColumns[2] ).c_str();
				sn[rowsFetched + dataIdx].type                    = static_cast<TA_Base_Core::ENode>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[3] ) );
				sn[rowsFetched + dataIdx].approvalState           = static_cast<TA_Base_Core::EApprovalState>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[4] ) );
				sn[rowsFetched + dataIdx].hasChildren             = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[5] ) );
				sn[rowsFetched + dataIdx].hasApprovedDescendant   = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[6] ) );
				sn[rowsFetched + dataIdx].hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[7] ) );
			}
			rowsFetched += summaryDataPtr->getNumRows();
            summaryDataPtr.reset();
		}
		while ( database->moreData( summaryData ) );

		return sn._retn();
	}

	unsigned long	PlanDataReadingClass::getParent( unsigned long node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select parent_node from node_tree where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33508, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33508, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33508, node);

		std::vector<std::string> columns;

		columns.push_back("parent_node");
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			//TA_THROW( TA_Base_Core::DataException( message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, strSql ));
			

			TA_THROW( TA_Base_Core::DataException( message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "p_key" ));

        }

		unsigned long parent = data->getUnsignedLongData( 0, columns[0] ); 
		return parent;
	}

	std::string PlanDataReadingClass::getNodeName(unsigned long node) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		/*std::ostringstream sql;

		sql << "select name from node_tree where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33509, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33509, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33509, node);
		std::vector<std::string> columns;
		columns.push_back("name");
		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if ( ( NULL == data.get() ) || ( 1 != data->getNumRows() ) )
		{
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			//TA_THROW( TA_Base_Core::DataException (message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, strSql ));
			TA_THROW( TA_Base_Core::DataException (message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "p_key" ));
		}

		std::string nodeName = data->getStringData(0, columns[0]);

		return nodeName;
	}

    unsigned long PlanDataReadingClass::getNodeId(const std::string& path) const
    {
        //
        unsigned long node = 0;
        bool foundNode = false;

        unsigned long parent = m_rootNodeId;
        std::string nodeName = "";

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		const std::string delimiter = "/"; 
			
		std::string::size_type beginPos = 0;

        //Remove the terminating "/" if any

        beginPos = path.find_last_of(delimiter);

        std::string newPath;

        if ( beginPos == (path.size() -1)  )
        {
            newPath = path.substr(0, beginPos );
        }
        else
        {
            newPath = path;
        }

        beginPos = 0;
		while(beginPos < newPath.size())
		{
			beginPos = newPath.find_first_of(delimiter, beginPos);
			if (beginPos == std::string::npos)
			{
                break;
			}
			
			std::string::size_type endPos = newPath.find_first_of(delimiter, beginPos+1);

			if (endPos == std::string::npos) 
			{
				nodeName = newPath.substr(beginPos+1);
			} 
			else
            {
				nodeName = newPath.substr(beginPos+1, endPos - 1 - beginPos);
            }

		    /*std::ostringstream sql;

		    sql << "select p_key from node_tree where parent_node = "
			    << parent << " and name = '"
			    << database->escapeQueryString(nodeName) << "'";*/
//			std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33505, parent, 
//			std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33505, parent, 
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33505, parent,
				database->escapeQueryString(nodeName));

            std::vector<std::string> columns;
		    columns.push_back( "p_key" );
		    
		    std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);

		    if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
            {
			    break;
            }

		    node = data->getUnsignedLongData( 0, columns[0] );

            if ( endPos == std::string::npos )
            {
                foundNode = true;
                break;
            }

            parent = node;

			// set up for next loop
			beginPos = endPos;
	    }

        if ( foundNode )
        { 
            return node;
        }

        std::string message = "The node '";
        message += path;
        message += "' does not exist!";
        
        throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(TA_Base_Core::PNFE_NODE_NOT_EXIST, 0, 0, message.c_str()));
    }

	unsigned long PlanDataReadingClass::getNodeId( unsigned long parent, const std::string& planName )
	{
		unsigned long node = 0;

		TA_Base_Core::IDatabase* dataBase = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< planName.c_str() << "'";*/
//		std::string strSql = defPrepareSQLStatement(dataBase->GetDbServerType(), NODE_TREE_STD_SELECT_33505, parent, planName);
//		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_SELECT_33505, parent, planName);
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33505, parent, planName);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(dataBase);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
		{
			std::ostringstream message;
			message << "The plan " << planName << " with parent " << parent << " does not exist.";
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(
				TA_Base_Core::PNFE_NODE_NOT_EXIST, 0, 0, message.str().c_str()));
		}

		node = data->getUnsignedLongData( 0, columns[0] );

		return( node );
	}

    int PlanDataReadingClass::getPlanType( unsigned long plan ) const
    {
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        //std::string strSql  = database->prepareSQLStatement(PLAN_TYPE_MAP_SELECT_80002, plan);

		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_TYPE_MAP_SELECT_80002, plan);

        std::vector<std::string> planTypeColumns;
        planTypeColumns.push_back( "fk_plan_type" );

        std::auto_ptr<TA_Base_Core::IData> planTypeData( database->executeQuery( strSql, planTypeColumns ) );
        PlanDataBaseHelper dataBaseHelper(database);

        if( (NULL == planTypeData.get() ) || (1 != planTypeData->getNumRows()) ) 
        {
            return TA_Base_Core::UNDEFINED_TYPE;
        }
        
        return planTypeData->getIntegerData(0, planTypeColumns[0]);
    }

	TA_Base_Core::NodeSummary*	PlanDataReadingClass::getNodeSummary ( unsigned long node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = " << node
			<< " and node_tree.p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_SUMMARY_STD_SELECT_64002, node, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_SUMMARY_SELECT_64002, node, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_SUMMARY_SELECT_64002, node, node);
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		std::auto_ptr<TA_Base_Core::IData> summaryData( database->executeQuery( strSql, summaryColumns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::NodeSummary_var ns = new TA_Base_Core::NodeSummary;
				
		if( (NULL == summaryData.get() ) || (1 != summaryData->getNumRows()) ) 
        {
            ns->node = TA_Base_Core::DatabaseKey::getInvalidKey();
            ns->parent = TA_Base_Core::DatabaseKey::getInvalidKey();
            ns->name = m_nvlValue.c_str();
            ns->type = TA_Base_Core::UNDEFINED_NODE;
            ns->approvalState = TA_Base_Core::UNDEFINED_APPROVAL_STATE;
            ns->hasChildren = false;
            ns->hasApprovedDescendant = false;
            ns->hasUnapprovedDescendant = false;
        }
        else
        {
		    ns->node = summaryData->getUnsignedLongData( 0, summaryColumns[0] );
		    ns->parent = summaryData->getUnsignedLongData( 0, summaryColumns[1] );
		    ns->name = summaryData->getStringData( 0, summaryColumns[2] ).c_str();
		    ns->type = static_cast<TA_Base_Core::ENode>( summaryData->getUnsignedLongData( 0, summaryColumns[3] ) );
		    ns->approvalState = static_cast<TA_Base_Core::EApprovalState>( summaryData->getUnsignedLongData( 0, summaryColumns[4] ) );
		    ns->hasChildren = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[5] ) );
		    ns->hasApprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[6] ) );
		    ns->hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[7] ) );
        }

		return( ns._retn() );
	}

	bool PlanDataReadingClass::existsNode(unsigned long node)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

	/*	std::ostringstream sql;
		
		sql << "select p_key from node_tree "
			<< " where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33510, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33510, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33510, node);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        {
			return false;
        }

		return true;
	}

	bool PlanDataReadingClass::canUseName(unsigned long node, unsigned long parent, const std::string& name)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		// Node summary query
		/*sql << "select p_key from node_tree "
			<< " where name = '" << name.c_str() << "' "
			<< " and parent_node = " << parent;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33511, name, parent);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33511, name, parent);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33511, name, parent);
		
        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        {
			return true;
        }

		if ( data->getNumRows() > 1 )
		{
			return false;
		}
		
		unsigned long curNode = data->getUnsignedLongData( 0, columns[0] );

		if ( curNode == node )
		{
			return true;
		}

		return false;
	}

	TA_Base_Core::NodeDetail* PlanDataReadingClass::getNodeDetail( unsigned long node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;
		std::vector<std::string> columns;
		
		TA_Base_Core::ENode nodeType = getNodeType( node );

		TA_Base_Core::NodeDetail_var nd = new TA_Base_Core::NodeDetail;

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			case TA_Base_Core::WORKFLOW_NODE:
			case TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE:
			{

				/*sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "'), approval_state,"
                    << " modified_by, to_char(modified_time, 'DD-Mon-YY HH24:MI:SS'), approval_changed_by,"
                    << " to_char(approval_changed_time, 'DD-Mon-YY HH24:MI:SS') "
					<< " from plan_details where node_id = " << node;*/
//				strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_Oracle_SELECT_33554, m_nvlValue, node);
//				strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33554, m_nvlValue, node);

		        database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33554, m_nvlValue, node);

				columns.push_back( "parent" );
				columns.push_back( "node_id" );
				columns.push_back( "name" );
				columns.push_back( "description" );
				columns.push_back( "approval_state" );
                columns.push_back( "modified_by" );
                columns.push_back( "modified_time" );
                columns.push_back( "approval_changed_by" );
                columns.push_back( "approval_changed_time" );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			case TA_Base_Core::PLANTEMPLATE_NODE:
			{
			/*	sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "')"
                    << " from category_details"
					<< " where node_id = " << node;*/
//				strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_Oracle_SELECT_33555, m_nvlValue, node);
//				strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33555, m_nvlValue, node);

		        database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33555, m_nvlValue, node);
				
				columns.push_back( "parent" );
				columns.push_back( "node_id" );
				columns.push_back( "name" );
				columns.push_back( "description" );
			}
			break;

			default:
			{
				TA_ASSERT( false, "Invalid node type" );
			}
			break;
		}
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
					
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			//TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, strSql ) );
			TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, "node_id" ) );

        }

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			case TA_Base_Core::WORKFLOW_NODE:
			case TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE:
			{
				TA_Base_Core::PlanDetail planDetail;
                planDetail.plan = node;
				planDetail.parent = static_cast<unsigned long>( data->getUnsignedLongData( 0, columns[0] ) );
				planDetail.plan = static_cast<unsigned long>( data->getUnsignedLongData( 0, columns[1] ) );
                planDetail.nodeType = nodeType;
				planDetail.name = data->getStringData( 0, columns[2] ).c_str();
				planDetail.description = getNvlString( data->getStringData( 0, columns[3] )).c_str();
				planDetail.approvalState = static_cast<TA_Base_Core::EApprovalState>( data->getUnsignedLongData( 0, columns[4] ) );
                planDetail.modifiedBy = data->getStringData( 0, columns[5] ).c_str();
                planDetail.modifiedTime = data->getStringData( 0, columns[6] ).c_str();
                planDetail.approvalChangedBy = data->getStringData( 0, columns[7] ).c_str();
                planDetail.approvalChangedTime = data->getStringData( 0, columns[8] ).c_str();
				nd->plan( planDetail );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			case TA_Base_Core::PLANTEMPLATE_NODE:
			{
				TA_Base_Core::CategoryDetail categoryDetail;

				categoryDetail.parent = static_cast<unsigned long>( data->getUnsignedLongData( 0, columns[0] ) );
				categoryDetail.category = static_cast<unsigned long>( data->getUnsignedLongData( 0, columns[1] ) );
				categoryDetail.name = data->getStringData( 0, columns[2] ).c_str();
				categoryDetail.description = getNvlString(data->getStringData( 0, columns[3] )).c_str();

				nd->category( categoryDetail );
			}
			break;

			default:
			{
				TA_ASSERT( false, "Invalid node type" );
			}
			break;
		}

		return nd._retn();
	}

	TA_Base_Core::StepNames* PlanDataReadingClass::getStepNames( void ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::string sql;

		// Step name query
		/*sql  = "select type_name from step_type order by type_value";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64501);
//		std::string strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64501);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_TYPE_SELECT_64501);
		
        std::vector<std::string> columns;
		columns.push_back( "type_name" );

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns ) ;
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::StepNames_var sn = new TA_Base_Core::StepNames;
		
        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "failed to find any step names." );
			sn->length( 0 );
			return( sn._retn() );
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do 
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			sn->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < data->getNumRows(); dataIdx++ )
			{
				sn[rowsFetched + dataIdx] = dataPtr->getStringData( dataIdx, columns[0] ).c_str();
			}
			rowsFetched += dataPtr->getNumRows();
            dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return sn._retn();
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getAllStepDetails( unsigned long plan ) const
	{
		return( getStepDetails( plan, "" ) );
    }

	TA_Base_Core::StepDetails* PlanDataReadingClass::getSpecifiedStepDetails( unsigned long plan, const TA_Base_Core::StepIds& steps ) const
	{
		std::ostringstream stepConstraint;
		
		for( CORBA::ULong i = 0; i < steps.length(); i++ )
		{
			stepConstraint << steps[i];

			// if it isn't the last one, add a comma
			if( (steps.length()-1) != i )
			{
				stepConstraint << ",";
			}
		}

		return( getStepDetails( plan, stepConstraint.str() ) );
	}

	TA_Base_Core::ParameterNames* PlanDataReadingClass::getParameterNames( const int stepType ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Step name query
	/*	sql << "select name from parameter_names where step_type = " << stepType;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64502, stepType);
//		std::string strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64502, stepType);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_TYPE_SELECT_64502, stepType);

        std::vector<std::string> columns;
		columns.push_back( "name" );

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::ParameterNames_var pn = new TA_Base_Core::ParameterNames;

        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			pn->length( 0 );
			return pn._retn();
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			pn->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < data->getNumRows(); dataIdx++ )
			{
				pn[rowsFetched + dataIdx] = dataPtr->getStringData( dataIdx, columns[0] ).c_str();
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return pn._retn();
    }

    TA_Base_Core::SequenceFlows* PlanDataReadingClass::getSequenceFlows( unsigned long plan ) const
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, PLAN_SEQUENCE_FLOW_SELECT_81001, plan);

        std::vector<std::string> columns;
        columns.push_back( "P_KEY" );
        columns.push_back( "ID" );
        columns.push_back( "INCOMING_STEP" );
        columns.push_back( "OUTGOING_STEP" );
        columns.push_back( "CONDITION" );
        columns.push_back( "GRAPH" );

        TA_Base_Core::IData* data = database->executeQuery( strSql, columns );
        PlanDataBaseHelper dataBaseHelper( database );

        TA_Base_Core::SequenceFlows_var varSeqs = new TA_Base_Core::SequenceFlows;

        if ( NULL == data )
        {
            varSeqs->length( 0u );
            return varSeqs._retn();
        }

        unsigned long rowsFetched = 0u;
        do
        {
            std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
            varSeqs->length( data->getNumRows() + rowsFetched );

            for( unsigned long dataIdx = 0; dataIdx < data->getNumRows(); dataIdx++ )
            {
                int nColLoop = 0;
                varSeqs[rowsFetched + dataIdx].planKey = plan;
                varSeqs[rowsFetched + dataIdx].pkey = dataPtr->getUnsignedLongData( dataIdx, columns[nColLoop++] );
                varSeqs[rowsFetched + dataIdx].id = dataPtr->getUnsignedLongData( dataIdx, columns[nColLoop++] );
                varSeqs[rowsFetched + dataIdx].incomingStep = dataPtr->getUnsignedLongData( dataIdx, columns[nColLoop++] );
                varSeqs[rowsFetched + dataIdx].outgoingStep = dataPtr->getUnsignedLongData( dataIdx, columns[nColLoop++] );
                varSeqs[rowsFetched + dataIdx].condition = dataPtr->getStringData( dataIdx, columns[nColLoop++] ).c_str();
                varSeqs[rowsFetched + dataIdx].graph = dataPtr->getStringData( dataIdx, columns[nColLoop++] ).c_str();
            }

            rowsFetched += dataPtr->getNumRows();
            dataPtr.reset();
        }
        while ( database->moreData( data ) );

        return varSeqs._retn();
    }

    TA_Base_Core::Steps* PlanDataReadingClass::getAllSteps( unsigned long plan ) const
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

//        std::ostringstream stmSql;
        std::vector<std::string> vecColumns;

//         stmSql << "select nvl(step_parameters.position, 1) as ParameterPos,"
//             << " nvl(step_parameters.value, '') as ParameterValue,"
//             << " step_details.p_key, step_details.position, nvl(step_details.name, ''), step_type.type_value,"
//             << " nvl(step_details.description, ''), step_details.skip, step_details.skippable,"
//             << " step_details.delay, nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0)" 
//             << " from step_type, step_details left join step_parameters on"
//             << " step_parameters.fk_step_details = step_details.p_key"
//             << " where step_details.fk_step_type = step_type.p_key"
//             << " and step_details.fk_node_tree = " << plan;
        // Improve database performance, sort by std::map.
//         stmSql << " order by step_details.position, step_parameters.position";

//		std::string strSql  = database->prepareSQLStatement(STEP_PARAMETERS_SELECT_63053, plan);
		TA_Base_Core::SQLStatement strSql;		

		database->prepareSQLStatement(strSql, PLAN_STEP_PARAMETERS_SELECT_63053, plan);

        vecColumns.push_back( "ParameterPos" );
        vecColumns.push_back( "ParameterValue" );
        vecColumns.push_back( "p_key");
        vecColumns.push_back( "position" );
		vecColumns.push_back( "id" );
        vecColumns.push_back( "name" );
        vecColumns.push_back( "type_value" );
        vecColumns.push_back( "description" );
        vecColumns.push_back( "skip" );
        vecColumns.push_back( "skippable" );
        vecColumns.push_back( "delay" );
        vecColumns.push_back( "ignore_failure" );

        TA_Base_Core::IData* pData = database->executeQuery( strSql, vecColumns );
        PlanDataBaseHelper dataBaseHelper( database );

		TA_Base_Core::Steps_var varResult = new TA_Base_Core::Steps();

        if( NULL == pData ) 
        {
            return varResult._retn();
		}

        typedef std::map<unsigned long, MapParameters> StepNumToParamatersMap;
        StepNumToParamatersMap mapStepParameters;
        StepNumToDetailMap mapSteps;
        StepNumToStepIdMap mapStepNumToIds;

        unsigned long ulStepPos = 0u;
        unsigned long ulParamaterPos = 0u;
        unsigned long ulStepPkey = 0;
		unsigned long ulId = 0;
        unsigned long ulType = 0;
        unsigned long ulSkip = 0;
        unsigned long ulSkipAble = 0;
        unsigned long ulDelay = 0;
        unsigned long ulIgnoreFailure = 0;
        std::string strParamaterValue = "";
        std::string strStepName = "";
        std::string strDescription = "";
        bool bNewStep = false;

        do
        {
            for ( unsigned long nLoop = 0; nLoop < pData->getNumRows(); ++nLoop )
            {
                try
                {
                    ulStepPos = static_cast<unsigned long>(pData->getUnsignedLongData( nLoop, "position" ));
                    if ( mapSteps.end() == mapSteps.find( ulStepPos ))
                    {
                        ulStepPkey = pData->getUnsignedLongData( nLoop, "p_key" );
						ulId = pData->getUnsignedLongData( nLoop, "id" );
                        ulType = pData->getUnsignedLongData( nLoop, "type_value" );
                        ulSkip = pData->getUnsignedLongData( nLoop, "skip" );
                        ulSkipAble = pData->getUnsignedLongData( nLoop, "skippable" );
                        ulDelay = pData->getUnsignedLongData( nLoop, "delay" );
                        ulIgnoreFailure = pData->getUnsignedLongData( nLoop, "ignore_failure" );
                        strStepName = pData->getStringData( nLoop, "name" );
                        strDescription = pData->getStringData( nLoop, "description" );
                        bNewStep = true;
                    }

                    ulParamaterPos = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "ParameterPos"));
                    strParamaterValue = pData->getStringData( nLoop, "ParameterValue" );
                }
                catch ( std::exception& expWhat )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
                    continue;
                }
                catch (...)
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
                    continue;
                }

                if ( bNewStep )
                {
                    TA_Base_Core::StepDetail stNewStepDetail;

                    std::pair<StepNumToDetailMap::iterator, bool> pirRes = mapSteps.insert( 
                        StepNumToDetailMap::value_type( ulStepPos, stNewStepDetail ));

                    if ( pirRes.second )
                    {
                        pirRes.first->second.delay = static_cast<TA_Base_Core::Seconds>(ulDelay);
                        pirRes.first->second.description = strDescription.c_str();
                        pirRes.first->second.ignoreFailure = static_cast<CORBA::Boolean>(0l != ulIgnoreFailure);
                        pirRes.first->second.name = strStepName.c_str();
                        pirRes.first->second.position = ulStepPos;
                        pirRes.first->second.skip = static_cast<CORBA::Boolean>(0l != ulSkip);
                        pirRes.first->second.skippable = static_cast<CORBA::Boolean>(0l != ulSkipAble);
                        pirRes.first->second.pkey = static_cast<unsigned long>(ulStepPkey);
                        pirRes.first->second.type = static_cast<int>(ulType);
						pirRes.first->second.id = static_cast<unsigned long>(ulId);
                    }

                    mapStepNumToIds.insert( StepNumToStepIdMap::value_type( ulStepPos, ulStepPkey ));
                }

                StepNumToParamatersMap::iterator itParameter = mapStepParameters.find( ulStepPos );
                if ( mapStepParameters.end() == itParameter )
                {
                    MapParameters mapNewParameters;
                    std::pair<StepNumToParamatersMap::iterator, bool> pirInsertResult;
                    pirInsertResult = mapStepParameters.insert( StepNumToParamatersMap::value_type( ulStepPos, mapNewParameters ));
                    if ( !pirInsertResult.second )
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Can not insert item to map");
                        continue;
                    }
                    else
                    {
                        itParameter = pirInsertResult.first;
                    }
                }

                if ( mapStepParameters.end() != itParameter )
                {
                    (itParameter->second)[ulParamaterPos] = strParamaterValue;
                }
            }

            delete pData;
            pData = NULL;

        } while ( database->moreData( pData ));

        if ( 0u == mapSteps.size() )
        {
            return varResult._retn();
        }

        varResult->length( mapSteps.size() );

        int nResultLoop = 0;
        for ( StepNumToDetailMap::iterator itLoop = mapSteps.begin(); mapSteps.end() != itLoop; ++itLoop )
        {
            varResult[nResultLoop].detail.delay = itLoop->second.delay;
            varResult[nResultLoop].detail.description = itLoop->second.description;
            varResult[nResultLoop].detail.ignoreFailure = itLoop->second.ignoreFailure;
            varResult[nResultLoop].detail.name =  itLoop->second.name;
            varResult[nResultLoop].detail.position = itLoop->second.position;
            varResult[nResultLoop].detail.skip = itLoop->second.skip;
            varResult[nResultLoop].detail.skippable = itLoop->second.skippable;
            varResult[nResultLoop].detail.pkey = itLoop->second.pkey;
            varResult[nResultLoop].detail.type = itLoop->second.type;
			varResult[nResultLoop].detail.id = itLoop->second.id;

            unsigned long ulTempStepNum = itLoop->second.position;
            StepNumToParamatersMap::iterator itFound = mapStepParameters.find( ulTempStepNum );
            if ( mapStepParameters.end() != itFound )
            {
                // Pull out parameters from map and populate the parameter sequences
                varResult[nResultLoop].parameters.length( itFound->second.size() );

                int nParasLoop = 0;
                for (MapParameters::iterator parameterIt = itFound->second.begin(); parameterIt != itFound->second.end(); ++parameterIt)
                {
                    varResult[nResultLoop].parameters[nParasLoop++] = parameterIt->second.c_str();
                }
            }

            ++nResultLoop;
        }

        return varResult._retn();
    }
    
	PlanTemplateStepParameterList PlanDataReadingClass::getMLParametersByDataPtType() const
	{
		TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		
		TA_Base_Core::SQLStatement strSql;
		std::vector<std::string> columns;

		database->prepareSQLStatement(strSql, PLAN_TEMPLATE_INS_STEP_PARAS_SELECT_63051, m_nvlValue, DATAPOINT_VECTOR);

		columns.push_back("fk_plan_template_instance_id"); 
		columns.push_back("StepId"); 
		columns.push_back("position");
		columns.push_back("type_value");
		columns.push_back("ParameterPos");
		columns.push_back("ParameterValue");

		TA_Base_Core::IData* pData = database->executeQuery(strSql, columns);
		PlanDataBaseHelper dataBaseHelper(database);

		PlanTemplateStepParameterList ptspList;

		//MapParameters mapParameters;
		// don't check for zero rows becase some steps don't have parameters!
		if (NULL == pData)
		{
			return ptspList;
		}

		std::string strParamValue;
		std::vector<std::string> strVector;		

		do
		{
			for (unsigned long nLoop = 0; nLoop < pData->getNumRows(); ++nLoop)
			{
				try
				{
					if (strVector.empty())
					{
						strVector.push_back(pData->getStringData(nLoop, "ParameterValue"));
					}

					Serialization iSerialization(strVector);
					MLSerialization ioSzn(&iSerialization);
					ML_Plan::DataPointVectorPara* dpv = dynamic_cast<ML_Plan::DataPointVectorPara*>(ioSzn.deserialize());

					if (dpv != NULL)
					{
						boost::shared_ptr<PlanTemplateStepParameterInfo> ptspInfo(new PlanTemplateStepParameterInfo);
						ptspInfo->planId = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "fk_plan_template_instance_id"));
						ptspInfo->stepId = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "StepId"));
						ptspInfo->stepPos = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "position"));
						ptspInfo->type = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "type_value"));
						ptspInfo->paramPos = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "ParameterPos"));
						ptspInfo->dataPointInfo.reset(dpv);						
						ptspList.push_back(ptspInfo);
					}
					
				}
				catch (std::exception& expWhat)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what());
					continue;
				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception");
					continue;
				}
			}

			delete pData;
			pData = NULL;

		} while (database->moreData(pData));

		return ptspList;
	}

    TA_Base_Core::StepParameters* PlanDataReadingClass::getStepParameters( unsigned long plan, unsigned long step, bool isStepNumber /*= false */ ) const
    {
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance( ).getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

        //std::ostringstream sql;
        TA_Base_Core::SQLStatement strSql;
        std::vector<std::string> columns;

        if ( isStepNumber )
        {
            database->prepareSQLStatement( strSql, PLAN_STEP_PARAMETERS_SELECT_63051, m_nvlValue, plan, step );

            columns.push_back( "p_key" );
            columns.push_back( "position" );
            columns.push_back( "type_value" );
            columns.push_back( "ParameterPos" );
            columns.push_back( "ParameterValue" );
        }
        else
        {
            database->prepareSQLStatement( strSql, PLAN_STEP_PARAMETERS_SELECT_63052, m_nvlValue, step );

            columns.push_back( "p_key" );
            columns.push_back( "position" );
            columns.push_back( "type_value" );
            columns.push_back( "ParameterPos" );
            columns.push_back( "ParameterValue" );
        }

        TA_Base_Core::IData* pData = database->executeQuery( strSql, columns );
        PlanDataBaseHelper dataBaseHelper( database );

        TA_Base_Core::StepParameters* pStepParams = new TA_Base_Core::StepParameters();
        //MapParameters mapParameters;
        // don't check for zero rows becase some steps don't have parameters!
        if ( NULL == pData )
        {
            return NULL;
        }

        StepNumToStepIdMap mapStepNumToIds;

        unsigned long ulStepPos = 0u;
        unsigned long ulParamaterPos = 0u;
        unsigned long ulStepPkey = 0;
        int eType = 0;
        unsigned long ulStepId = 0;
        bool bGetStep = false;
		unsigned long ulParameterCount = 0;

        if ( 0u <= pData->getNumRows() )
        {
            pStepParams->length( pData->getNumRows() - 1 );
        }

        do
        {
            for ( unsigned long nLoop = 0; nLoop < pData->getNumRows(); ++nLoop )
            {
                try
                {
                    ulStepPos = static_cast<unsigned long>(pData->getUnsignedLongData( nLoop, "position" ));
                    ulStepPkey = pData->getUnsignedLongData( nLoop, "p_key" );
                    if ( mapStepNumToIds.end() == mapStepNumToIds.find( ulStepPos ))
                    {
                        mapStepNumToIds.insert( StepNumToStepIdMap::value_type( ulStepPos, ulStepPkey ) );
                    }

                    bGetStep = (isStepNumber && (ulStepPos == step)) || (!isStepNumber && (ulStepPkey == step));
                    if ( bGetStep )
                    {
                        ulStepId = static_cast<unsigned long>(ulStepPkey);
                        eType = static_cast<int>(pData->getUnsignedLongData( nLoop, "type_value" ));
                        ulParamaterPos = static_cast<unsigned long>(pData->getUnsignedLongData(nLoop, "ParameterPos"));

                        //mapParameters[ulParamaterPos] = pData->getStringData( nLoop, "ParameterValue" );
                        (*pStepParams)[ulParamaterPos - 1] = pData->getStringData( nLoop, "ParameterValue" ).c_str( );

						if(ulParamaterPos > ulParameterCount)
						{
							ulParameterCount = ulParamaterPos;
						}
                    }
                }
                catch ( std::exception& expWhat )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
                    continue;
                }
                catch ( ... )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
                    continue;
                }
            }

            delete pData;
            pData = NULL;

        } while ( database->moreData( pData ) );

		// Set length to actual parameter length
        pStepParams->length(ulParameterCount);

        return pStepParams;
    }

	unsigned long PlanDataReadingClass::getStepCount( const unsigned long plan ) const
	{
		unsigned long rval = 0;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Step count query
	/*	sql << "select count(position) as steps from step_details where fk_node_tree =  " << plan;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63506, plan);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63506, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63506, plan);

		std::vector<std::string> columns;
		columns.push_back( "steps" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
		{
			return rval;
		}
		
		rval = data->getUnsignedLongData( 0, columns[0] );

		return( rval );
	}

	TA_Base_Core::EApprovalState PlanDataReadingClass::getPlanApprovalState(unsigned long plan) const
	{
		TA_Base_Core::EApprovalState rval = TA_Base_Core::UNDEFINED_APPROVAL_STATE;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select state_value from approval_state where p_key = "
			<< "(select node_tree.fk_approval_state from node_tree where p_key =  "
			<< plan << ")";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33512, plan);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33512, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33512, plan);

		std::vector<std::string> columns;
		columns.push_back("state_value");

		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		{
			return rval;
		}

		rval = (TA_Base_Core::EApprovalState) data->getUnsignedLongData (0, columns[0] );

		return (rval);
	}


	int PlanDataReadingClass::getCategoryLevel(unsigned long category)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		
		//std::ostringstream sql;
		
		// Node summary query
		/*sql << "select parent_node" 
			<< " from node_tree"
			<< " where p_key = "
			<< category;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33508, category);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33508, category);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33508, category);
		
		std::vector<std::string> columns;
		columns.push_back( "parent_node" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		
		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
		{
			std::ostringstream msg;
			msg << "The node " << category << " does not exist.";
			TA_THROW( TA_Base_Core::DataException( msg.str().c_str(), TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
		}
		
		int parent = data->getUnsignedLongData( 0, columns[0] );
		
		if ( parent == 1 )
		{
			return 1;
		}
		else if (getParent (parent) == 1)
		{
			return 2;
		}
		else
		{
			return 3;
		}
		
	}


	unsigned long PlanDataReadingClass::getStepNumber( const unsigned long step ) const
	{
		unsigned long rval = 0;
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select position from step_details where p_key =  " << step;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63507, step);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63507, step);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63507, step);

		std::vector<std::string> columns;
		columns.push_back("position");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
		    if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		    {
			    return rval;
		    }

		    rval = data->getUnsignedLongData (0, columns[0] );
        }
        catch ( ... )
        {
            rval = 0;
        }

		return (rval);
	}

	unsigned long PlanDataReadingClass::getStepPosition( const unsigned long plan, const unsigned long step ) const
	{
		unsigned long rval = 0;
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select position from step_details where fk_node_tree = " << plan << " and id =  " << step;*/
		TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63509, plan, step);

		std::vector<std::string> columns;
		columns.push_back("position");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
		    if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		    {
			    return rval;
		    }

		    rval = data->getUnsignedLongData (0, columns[0] );
        }
        catch ( ... )
        {
            rval = 0;
        }

		return (rval);
	}


    unsigned long PlanDataReadingClass::getStepId( const unsigned long plan, const unsigned long position ) const
    {
		unsigned long rval = 0;
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

	/*	std::ostringstream sql;
		
		sql << "select p_key from step_details "
            << "where fk_node_tree =  " << plan << " and position = " << step;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63508, plan, step);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63508, plan, step);
		TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63508, plan, position);

		std::vector<std::string> columns;
		columns.push_back("stepId");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
		    if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		    {
			    return rval;
		    }

		    rval = data->getUnsignedLongData (0, columns[0] );
        }
        catch ( ... )
        {
            rval = 0;
        }

		return (rval);        
    }


	std::string PlanDataReadingClass::getNodePath( unsigned long plan ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		//int nDbServerType = database->GetDbServerType();

		//std::string nodePath, strSql;
		std::string nodePath;
		TA_Base_Core::SQLStatement strSql;

		unsigned long currentNode(plan);

		while( 0 != currentNode )
		{
			//std::ostringstream sql;

			// Step details query
			/*sql << "select name, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;*/
//			strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33556, currentNode);

		    database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33556, currentNode);

			std::vector<std::string> columns;
			columns.push_back( "name" );
			columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
						
			if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode; 

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed at node %lu to find path for plan %lu", currentNode, plan );
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
			}

			nodePath.insert( 0, data->getStringData( 0, columns[0] ) );
			nodePath.insert( 0, "/" );

			currentNode = data->getUnsignedLongData( 0, columns[1] );
		}
		
		// the substr removes the unwanted leading /'s
        //MP: The nodePath is returned if its length is less than 3.
        //    Fix for PropWeb Issue #1736
        if ( nodePath.length() < 3 )
        {
            return nodePath;
        }
        else
        {
			return( nodePath.substr(2) );
        }
	}

    // Returns the list of node IDs of the nodes making up the specified plan's path. 
    // The first element in the list is the root node's ID and the last element is 
    // the actual plan node's ID.
	TA_Base_Core::NodeIds* PlanDataReadingClass::getNodeIdsInNodePath(unsigned long plan) const
	{

        std::vector<unsigned long> nodeIds;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		unsigned long currentNode(plan);
		//int nDbServerType = database->GetDbServerType();
		TA_Base_Core::SQLStatement strSql;

		while( 0 != currentNode )
		{
			//std::ostringstream sql;

			// Node ID + parent ID query
		/*	sql << "select p_key, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;*/
//			strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33557, currentNode);

            database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33557, currentNode);

            std::vector<std::string> columns;
            columns.push_back( "p_key" );
            columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
						
			if ( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode; 

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed at node %lu to find path for plan %lu", currentNode, plan );
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
			}

            nodeIds.insert( nodeIds.begin(), data->getUnsignedLongData( 0, columns[0] ) );

			currentNode = data->getUnsignedLongData( 0, columns[1] );
		}

        TA_Base_Core::NodeIds_var ni = new TA_Base_Core::NodeIds;

		ni->length(nodeIds.size());
		
        int i = 0;
        for (std::vector<unsigned long>::iterator it = nodeIds.begin(); it != nodeIds.end(); it++)
		{
			ni.operator [](i++) = *it;
		}

		return ni._retn();
	}

	TA_Base_Core::ENode PlanDataReadingClass::getNodeType( unsigned long node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_type.type_value" 
			<< " from node_tree, node_type"
			<< " where node_tree.fk_node_type = node_type.p_key"
			<< " and   node_tree.p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33513, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33513, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_NODE_TREE_SELECT_33513, node);
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
            std::ostringstream message;
            message << "Category or Plan Node with NodeId[" << node << "] was not found.";
            std::string strMessage = message.str();
			TA_THROW( TA_Base_Core::DataException( strMessage.c_str(), TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
        }


		switch( data->getUnsignedLongData( 0, columns[0] ) )
		{
			case TA_Base_Core::PLAN_NODE:
			{
				return( TA_Base_Core::PLAN_NODE );
			}
			break;

			case TA_Base_Core::WORKFLOW_NODE:
			{
				return( TA_Base_Core::WORKFLOW_NODE );
			}
			break;

			case TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE:
			{
				return(TA_Base_Core::PLANTEMPLATE_INSTANCE_NODE);
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			{
				return( TA_Base_Core::CATEGORY_NODE );
			}
			break;

			case TA_Base_Core::PLANTEMPLATE_NODE:
			{
				return(TA_Base_Core::PLANTEMPLATE_NODE);
			}
			break;

			default:
			{
				TA_THROW( TA_Base_Core::DataException( "Invalid node type", TA_Base_Core::DataException::WRONG_TYPE, data->getStringData( 0, columns[0] ) ) );
			}
			break;
		}
	}

	int PlanDataReadingClass::getStepType( unsigned long plan, unsigned long step, bool byStepNumber ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;

		if (byStepNumber)
		{
		/*	sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.fk_node_tree = " << plan
				<< " and step_details.position = " << step;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64503, plan, step);
//			strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64503, plan, step);

		    database->prepareSQLStatement(strSql, PLAN_STEP_TYPE_SELECT_64503, plan, step);
		}
		else
		{
			/*sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.p_key = " << step;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64504, step);
//			strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64504, step);

		    database->prepareSQLStatement(strSql, PLAN_STEP_TYPE_SELECT_64504, step);
		}
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			return 0;
        }

		return static_cast<int>(data->getUnsignedLongData( 0, columns[0] ));
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getStepDetails( unsigned long plan, const std::string& stepConstraint ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		std::ostringstream sql;

		// Step details query
	/*	sql << "select step_details.p_key, step_details.position, step_details.name,"
            << " step_type.type_value, nvl(step_details.description, '" << m_nvlValue.c_str() << "'),"
			<< " step_details.skip, step_details.skippable, step_details.delay,"
            << " nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0) "
			<< " from step_details, node_tree, step_type"
			<< " where step_details.fk_node_tree = node_tree.p_key"
			<< " and step_details.fk_step_type = step_type.p_key"
			<< " and step_details.fk_node_tree = " << plan;*/

		if( stepConstraint.length() > 0 )
		{
			sql << " and step_details.position in (" << stepConstraint << ")";
		}
		
		//sql << " order by step_details.position";
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_Oracle_SELECT_63552,
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63552,
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63552,
			m_nvlValue, plan, sql.str());

        std::vector<std::string> columns;
		columns.push_back( "p_key");
		columns.push_back( "position" );
		columns.push_back( "id" );
		columns.push_back( "name" );
        columns.push_back( "type" );
        columns.push_back( "description" );
		columns.push_back( "skip" );
		columns.push_back( "skippable" );
		columns.push_back( "delay" ); 
		columns.push_back( "ignore_failure" ); 
        columns.push_back( "graph" );

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns ) ;
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::StepDetails_var sd = new TA_Base_Core::StepDetails;

        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			sd->length( 0 );
			return sd._retn();
        }

        // TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			sd->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < dataPtr->getNumRows(); dataIdx++ )
			{
				sd[rowsFetched + dataIdx].pkey = static_cast<unsigned long> (dataPtr->getUnsignedLongData( dataIdx, columns[0] ) );
				sd[rowsFetched + dataIdx].position = dataPtr->getUnsignedLongData( dataIdx, columns[1] );
                sd[rowsFetched + dataIdx].id = dataPtr->getUnsignedLongData( dataIdx, columns[2] );
				sd[rowsFetched + dataIdx].name = dataPtr->getStringData( dataIdx, columns[3] ).c_str();
				sd[rowsFetched + dataIdx].type = static_cast<int>( dataPtr->getUnsignedLongData( dataIdx, columns[4] ) );
				sd[rowsFetched + dataIdx].description = getNvlString(dataPtr->getStringData( dataIdx, columns[5] )).c_str();
				sd[rowsFetched + dataIdx].skip = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[6] ) );
				sd[rowsFetched + dataIdx].skippable = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[7] ) );
				sd[rowsFetched + dataIdx].delay = static_cast<TA_Base_Core::Seconds>( dataPtr->getUnsignedLongData( dataIdx, columns[8] ) );
				sd[rowsFetched + dataIdx].ignoreFailure = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[9] ) );
                sd[rowsFetched + dataIdx].graph = getNvlString( dataPtr->getStringData( dataIdx, columns[10] ) ).c_str( );
            }
			rowsFetched += dataPtr->getNumRows();
			dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return sd._retn();
	}


	TA_Base_Core::Plan* PlanDataReadingClass::getPlan( unsigned long plan )
	{
		TA_Base_Core::Plan_var thePlan = new TA_Base_Core::Plan;
		TA_Base_Core::NodeDetail_var nodeDetail = getNodeDetail(plan);

		thePlan->detail = nodeDetail->plan();

        TA_Base_Core::Steps_var varSteps = getAllSteps( plan );

        thePlan->stepseq.length( varSteps->length() );

		for( unsigned long i=0; i < varSteps->length(); i++ )
		{
			thePlan->stepseq[i].detail = varSteps[i].detail;
			thePlan->stepseq[i].parameters = varSteps[i].parameters;
		}

		return( thePlan._retn() );
	}
    
	unsigned long PlanDataReadingClass::getNextPlanSeqNumber(void)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;

		sql << "select plans_seq.nextval Next_Step_Id from dual";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_Oracle_SELECT_63553);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63553);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_STEP_DETAILES_SELECT_63553);
		
        std::vector<std::string> columns;
		columns.push_back( "Next_Step_Id" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			std::cout << "No next step id returned." << std::endl;
        }

		return(  data->getUnsignedLongData( 0, columns[0] ) );
	}

    std::string PlanDataReadingClass::getNvlString( const std::string& strSrc ) const
    {
        if ( strSrc.compare(m_nvlValue) == 0 )
        {
            return "";
        }

        return strSrc;
    }

    unsigned long PlanDataReadingClass::getPlanId(const std::string& planReference) const
    {
        // This method is called when extracting from the database plan step parameters that reference a plan
        // (such as the child plan parameter of a Run Plan step). The specified plan reference can contain a 
        // numeric plan ID or it can be a fully qualified plan path.
        // If the plan reference contains only numeric characters it must be a plan ID, otherwise it is 
        // interpreted to be a plan path.

        if (!planReference.empty())
        {
            if (planReference.find_first_not_of(" 0123456789") == std::string::npos)
            {
                return ::strtoul(planReference.c_str(), NULL, 10);
            }
            else
            {
                try
                {
                    return getNodeId(planReference);
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to retrieve plan ID for plan %s", planReference.c_str());
                }
            }
        }

        return 0;
    }

    unsigned long PlanDataReadingClass::getStepId(unsigned long plan, const std::string& stepReference) const
    {
        // This method is called when extracting from the database plan step parameters that reference a plan
        // step (such as the target step parameter of a Jump step). The specified step reference can contain a 
        // numeric step ID or it can be a relative step position.
        // If the plan reference contains only numeric characters it must be a step ID, otherwise it is 
        // interpreted to be a step number.

        if (!stepReference.empty())
        {
            if (stepReference.find_first_not_of(" 0123456789") == std::string::npos)
            {
                return ::strtoul(stepReference.c_str(), NULL, 10);
            }
            else
            {
                try
                {
                    // When the step is referenced by its relative position, the format used is "#<step number".
                    std::string stepNumber(stepReference.substr(stepReference.find_first_of("0123456789")));

                    return getStepId(plan, ::strtoul(stepNumber.c_str(), NULL, 10));
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                        "Failed to retrieve step ID for plan %lu, step %s", plan, stepReference.c_str());
                }
            }
        }

        return 0;
    }

    unsigned long PlanDataReadingClass::getStepId( const StepNumToStepIdMap& refStepNumToIds, const std::string& stepReference ) const
    {
        if ( !stepReference.empty() )
        {
            if ( stepReference.find_first_not_of(" 0123456789") == std::string::npos )
            {
                return stringToUlong( stepReference );
            }
            else
            {
                // When the step is referenced by its relative position, the format used is "#<step number".
                std::string stepNumber(stepReference.substr(stepReference.find_first_of("0123456789")));
                unsigned long ulStepNum = static_cast<unsigned long>(stringToUlong( stepNumber ));
                StepNumToStepIdMap::const_iterator itFound = refStepNumToIds.find( ulStepNum );
                if ( refStepNumToIds.end() != itFound )
                {
                    return itFound->second;
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Step number [%lu] could not be found", ulStepNum );
                }
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Step ID is empty string" );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not convert Step ID from [%s]", stepReference.c_str() );
        return 0u;
    }

    unsigned long PlanDataReadingClass::stringToUlong( const std::string& strSrc ) const
    {
        std::stringstream stmConverter;
        stmConverter << strSrc.c_str();

        unsigned long ulResult = 0;
        stmConverter >> ulResult;

        return ulResult;
    }

    void PlanDataReadingClass::getStepTypes( std::map<unsigned long, StepTypeInfo>& mapTypes )
    {
        mapTypes.clear();

        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, PLAN_STEP_TYPE_SELECT_64505);

        std::vector<std::string> stepTypeColumns;
        stepTypeColumns.push_back("TYPE_VALUE");
        stepTypeColumns.push_back("TYPE_NAME");
        stepTypeColumns.push_back("STEP_USE");
        stepTypeColumns.push_back("SERVER_DLL_NAME");
        stepTypeColumns.push_back("MFT_DLL_NAME");

        TA_Base_Core::IData* stepUseData = database->executeQuery(strSql, stepTypeColumns);
        PlanDataBaseHelper dataBaseHelper(database);

        if ((NULL == stepUseData) || (0 == stepUseData->getNumRows()))
        {
            return;
        }

        do
        {
            std::auto_ptr<TA_Base_Core::IData> stepUseDataPtr(stepUseData);
            for (unsigned long dataIdx = 0; dataIdx < stepUseData->getNumRows(); dataIdx++)
            {
                int nColIdx = 0;
                StepTypeInfo typeInfo;
                unsigned long ulTypeId = stepUseDataPtr->getUnsignedLongData(dataIdx, stepTypeColumns[nColIdx++]);
                std::map<unsigned long, StepTypeInfo>::iterator itInserted = mapTypes.insert(std::make_pair(ulTypeId, typeInfo)).first;
                if ( mapTypes.end() != itInserted )
                {
                    itInserted->second.typeId = ulTypeId;
                    itInserted->second.typeName = stepUseDataPtr->getStringData(dataIdx, stepTypeColumns[nColIdx++]);
                    itInserted->second.typeAccess = stepUseDataPtr->getUnsignedLongData(dataIdx, stepTypeColumns[nColIdx++]);
                    itInserted->second.serverDllName = stepUseDataPtr->getStringData(dataIdx, stepTypeColumns[nColIdx++]);
                    itInserted->second.clientDllName = stepUseDataPtr->getStringData(dataIdx, stepTypeColumns[nColIdx++]);
                }
            }
            stepUseDataPtr.reset();
        } while (database->moreData(stepUseData));
    }

    void PlanDataReadingClass::getStepShortcuts( std::map<unsigned long, std::map<unsigned long, StepShortcut> >& mapShortcuts )
    {
        mapShortcuts.clear();

        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, PLAN_WORKFLOW_SHORTCUT_SELECT_81001);

        std::vector<std::string> stepTypeColumns;
        stepTypeColumns.push_back("STEP");
        stepTypeColumns.push_back("ORDER_ID");
        stepTypeColumns.push_back("ICON");
        stepTypeColumns.push_back("SHORTCUT");
        stepTypeColumns.push_back("SHORTCUT_ORDER");

        TA_Base_Core::IData* stepUseData = database->executeQuery(strSql, stepTypeColumns);
        PlanDataBaseHelper dataBaseHelper(database);

        if ((NULL == stepUseData) || (0 == stepUseData->getNumRows()))
        {
            return;
        }

        unsigned long ulCurOrder = 0u;
        int nCurStep = 0;
        int nShortcut = 0;
        std::string strIcon = "";
        std::map<unsigned long, StepShortcut> mapShortcut;

        do
        {
            std::auto_ptr<TA_Base_Core::IData> stepUseDataPtr(stepUseData);
            for (unsigned long dataIdx = 0; dataIdx < stepUseData->getNumRows(); dataIdx++)
            {
                int nColIdx = 0;
                nCurStep = stepUseDataPtr->getUnsignedLongData(dataIdx, stepTypeColumns[nColIdx++]);
                ulCurOrder = stepUseDataPtr->getIntegerData(dataIdx, stepTypeColumns[nColIdx++]);
                strIcon = stepUseDataPtr->getStringData(dataIdx, stepTypeColumns[nColIdx++]);
                nShortcut = stepUseDataPtr->getIntegerData(dataIdx, stepTypeColumns[nColIdx++]);

                std::map<unsigned long, std::map<unsigned long, StepShortcut> >::iterator itFound = mapShortcuts.find( nCurStep );
                if ( itFound == mapShortcuts.end() )
                {
                    itFound = mapShortcuts.insert( std::make_pair( nCurStep, mapShortcut )).first;
                }

                if ( itFound == mapShortcuts.end() )
                {
                    continue;
                }

                itFound->second[ulCurOrder].icon = strIcon.c_str();
                itFound->second[ulCurOrder].shortcuts.push_back( nShortcut );
            }
            stepUseDataPtr.reset();
        } while (database->moreData(stepUseData));
    }


} // TA_Base_Bus
