/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataReader.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2011/06/21 10:55:37 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "PlansCommon.h"

namespace TA_Base_Bus
{
	PlanDataReadingClass::PlanDataReadingClass() : m_rootNodeId(TA_Base_Core::ROOT_NODE_ID), m_nvlValue(" ")
	{
        
	}

 	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getRootNodeChildren( void ) const
    {
        return getChildren(m_rootNodeId);
    }

	

	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getChildren( TA_Base_Core::NodeId parent ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		// Node summary query
		sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = node_tree.p_key"
			<< " and node_tree.parent_node = " << parent;
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		TA_Base_Core::IData* summaryData = database->executeQuery( sql.str(), summaryColumns );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
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
				sn[rowsFetched + dataIdx].name                    = CORBA::string_dup( summaryDataPtr->getStringData( dataIdx, summaryColumns[2] ).c_str() );
				sn[rowsFetched + dataIdx].type                    = static_cast<TA_Base_Core::ENode>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[3] ) );
				sn[rowsFetched + dataIdx].approvalState           = static_cast<TA_Base_Core::EApprovalState>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[4] ) );
				sn[rowsFetched + dataIdx].hasChildren             = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[5] ) );
				sn[rowsFetched + dataIdx].hasApprovedDescendant   = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[6] ) );
				sn[rowsFetched + dataIdx].hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[7] ) );
			}
			rowsFetched += summaryDataPtr->getNumRows();
			summaryDataPtr = std::auto_ptr<TA_Base_Core::IData>(NULL);		// ensure memory is released
			summaryData = NULL;
		}
		while ( database->moreData( summaryData ) );

		return sn._retn();
	}

	TA_Base_Core::NodeId	PlanDataReadingClass::getParent( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 

		std::ostringstream sql;
		
		sql << "select parent_node from node_tree where p_key = " << node;

		std::vector<std::string> columns;

		columns.push_back("parent_node");
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			TA_THROW( TA_Base_Core::DataException( message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, sql.str() ));
        }

		TA_Base_Core::NodeId parent = data->getUnsignedLongData( 0, columns[0] ); 
		return parent;
	}

	std::string PlanDataReadingClass::getNodeName(TA_Base_Core::NodeId node) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 
		std::ostringstream sql;

		sql << "select name from node_tree where p_key = " << node;
		std::vector<std::string> columns;
		columns.push_back("name");
		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		if ( ( NULL == data.get() ) || ( 1 != data->getNumRows() ) )
		{
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			TA_THROW( TA_Base_Core::DataException (message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, sql.str() ));
		}

		std::string nodeName = data->getStringData(0, columns[0]);

		return nodeName;
	}

    TA_Base_Core::NodeId PlanDataReadingClass::getNodeId(const std::string& path) const
    {
        //
        TA_Base_Core::NodeId node = 0;
        bool foundNode = false;

        TA_Base_Core::NodeId parent = m_rootNodeId;
        std::string nodeName = "";

		TA_Base_Core::IDatabase* database= getDatabase();
 

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

		    std::ostringstream sql;

		    sql << "select p_key from node_tree where parent_node = "
			    << parent << " and name = '"
			    << database->escapeQueryString(nodeName) << "'";

            std::vector<std::string> columns;
		    columns.push_back( "p_key" );
		    
		    std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
			PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

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

	TA_Base_Core::NodeId PlanDataReadingClass::getNodeId( TA_Base_Core::NodeId parent, const std::string& planName )
	{
		TA_Base_Core::NodeId node = 0;

		TA_Base_Core::IDatabase* dataBase = getDatabase();
 

		std::ostringstream sql;
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< planName.c_str() << "'";

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716

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

	TA_Base_Core::NodeSummary*	PlanDataReadingClass::getNodeSummary ( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 

		std::ostringstream sql;

		// Node summary query
		sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = " << node
			<< " and node_tree.p_key = " << node;
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		std::auto_ptr<TA_Base_Core::IData> summaryData( database->executeQuery( sql.str(), summaryColumns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		TA_Base_Core::NodeSummary_var ns = new TA_Base_Core::NodeSummary;
				
		if( (NULL == summaryData.get() ) || (1 != summaryData->getNumRows()) ) 
        { 
			return( ns._retn() );
        }
		
		ns->node                    = summaryData->getUnsignedLongData( 0, summaryColumns[0] );
		ns->parent                  = summaryData->getUnsignedLongData( 0, summaryColumns[1] );
		ns->name                    = CORBA::string_dup( summaryData->getStringData( 0, summaryColumns[2] ).c_str() );
		ns->type                    = static_cast<TA_Base_Core::ENode>( summaryData->getUnsignedLongData( 0, summaryColumns[3] ) );
		ns->approvalState           = static_cast<TA_Base_Core::EApprovalState>( summaryData->getUnsignedLongData( 0, summaryColumns[4] ) );
		ns->hasChildren             = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[5] ) );
		ns->hasApprovedDescendant   = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[6] ) );
		ns->hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[7] ) );

		return( ns._retn() );
	}

	bool PlanDataReadingClass::existsNode(TA_Base_Core::NodeId node)
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 
		std::ostringstream sql;
		
		sql << "select p_key from node_tree "
			<< " where p_key = " << node;
		
        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        {
			return false;
        }
		 
		return true;
	}

	bool PlanDataReadingClass::canUseName(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent, const std::string& name)
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 

		std::ostringstream sql;
		// Node summary query
		sql << "select p_key from node_tree "
			<< " where name = '" << name.c_str() << "' "
			<< " and parent_node = " << parent;
		
        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        { 
			return true;
        }

		if ( data->getNumRows() > 1 )
		{ 
			return false;
		}
		
		TA_Base_Core::NodeId curNode = data->getUnsignedLongData( 0, columns[0] );

		if ( curNode == node )
		{
			return true;
		}

		return false;
	}

	TA_Base_Core::NodeDetail* PlanDataReadingClass::getNodeDetail( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 

		std::ostringstream sql;
		std::vector<std::string> columns;
		
		TA_Base_Core::ENode nodeType = getNodeType( node );

		TA_Base_Core::NodeDetail_var nd = new TA_Base_Core::NodeDetail;

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			{

				sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "'), approval_state,"
                    << " modified_by, to_char(modified_time, 'DD-Mon-YY HH24:MI:SS'), approval_changed_by,"
                    << " to_char(approval_changed_time, 'DD-Mon-YY HH24:MI:SS') "
					<< " from plan_details where node_id = " << node;

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
			{
				sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "')"
                    << " from category_details"
					<< " where node_id = " << node;
				
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
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
					
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        { 
			TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, sql.str() ) );
        }

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			{
				TA_Base_Core::PlanDetail planDetail;

				planDetail.parent		 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[0] ) );
				planDetail.plan			 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[1] ) );
				planDetail.name          = CORBA::string_dup( data->getStringData( 0, columns[2] ).c_str() );
				planDetail.description   = CORBA::string_dup( getStringData(data, 0, columns[3] ).c_str() );
				planDetail.approvalState = static_cast<TA_Base_Core::EApprovalState>( data->getUnsignedLongData( 0, columns[4] ) );
                planDetail.modifiedBy    = CORBA::string_dup( data->getStringData( 0, columns[5] ).c_str() );
                planDetail.modifiedTime  = CORBA::string_dup( data->getStringData( 0, columns[6] ).c_str() );
                planDetail.approvalChangedBy    = CORBA::string_dup( data->getStringData( 0, columns[7] ).c_str() );
                planDetail.approvalChangedTime  = CORBA::string_dup( data->getStringData( 0, columns[8] ).c_str() );
				nd->plan( planDetail );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			{
				TA_Base_Core::CategoryDetail categoryDetail;

				categoryDetail.parent		 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[0] ) );
				categoryDetail.category			 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[1] ) );
				categoryDetail.name          = CORBA::string_dup( data->getStringData( 0, columns[2] ).c_str() );
				categoryDetail.description   = CORBA::string_dup( getStringData(data, 0, columns[3] ).c_str() );

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
		TA_Base_Core::IDatabase* database= getDatabase();

		std::string sql;

		// Step name query
		sql  = "select type_name from step_type order by type_value";
		
        std::vector<std::string> columns;
		columns.push_back( "type_name" );

		TA_Base_Core::IData* data = database->executeQuery( sql, columns ) ;
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		TA_Base_Core::StepNames_var sn = new TA_Base_Core::StepNames;
		
        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "failed to find any step names." );
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
				sn[rowsFetched + dataIdx] = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[0] ).c_str() );
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr = std::auto_ptr<TA_Base_Core::IData>(NULL);		// ensure memory is released
			data = NULL;
		}
		while ( database->moreData( data ) );

		return sn._retn();
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getAllStepDetails( TA_Base_Core::NodeId plan ) const
	{
		return( getStepDetails( plan, "" ) );
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getSpecifiedStepDetails( TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps ) const
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

	TA_Base_Core::ParameterNames* PlanDataReadingClass::getParameterNames( TA_Base_Core::EStep stepType ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		// Step name query
		sql << "select name from parameter_names where step_type = " << stepType;

        std::vector<std::string> columns;
		columns.push_back( "name" );

		TA_Base_Core::IData* data = database->executeQuery( sql.str(), columns );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
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
				pn[rowsFetched + dataIdx] = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[0] ).c_str() );
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr = std::auto_ptr<TA_Base_Core::IData>(NULL);		// ensure memory is released
			data = NULL;
		}
		while ( database->moreData( data ) );

		return pn._retn();
	}

    TA_Base_Core::Steps* PlanDataReadingClass::getAllSteps( TA_Base_Core::NodeId plan ) const
    {
        TA_Base_Core::IDatabase* database= getDatabase();

        std::ostringstream stmSql;
        std::vector<std::string> vecColumns;

        stmSql << "select nvl(step_parameters.position, 1) as ParameterPos,"
            << " nvl(step_parameters.value, '') as ParameterValue,"
            << " step_details.p_key, step_details.position, nvl(step_details.name, ''), step_type.type_value,"
            << " nvl(step_details.description, ''), step_details.skip, step_details.skippable,"
            << " step_details.delay, nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0)" 
            << " from step_type, step_details left join step_parameters on"
            << " step_parameters.fk_step_details = step_details.p_key"
            << " where step_details.fk_step_type = step_type.p_key"
            << " and step_details.fk_node_tree = " << plan;
        // Improve database performance, sort by std::map.
//         stmSql << " order by step_details.position, step_parameters.position";

        vecColumns.push_back( "ParameterPos" );
        vecColumns.push_back( "ParameterValue" );
        vecColumns.push_back( "p_key");
        vecColumns.push_back( "position" );
        vecColumns.push_back( "name" );
        vecColumns.push_back( "type_value" );
        vecColumns.push_back( "description" );
        vecColumns.push_back( "skip" );
        vecColumns.push_back( "skippable" );
        vecColumns.push_back( "delay" );
        vecColumns.push_back( "ignore_failure" );
		vecColumns.push_back( "no_wait" );

        TA_Base_Core::IData* pData = database->executeQuery( stmSql.str(), vecColumns );

		TA_Base_Core::Steps_var varResult = new TA_Base_Core::Steps();

        if( NULL == pData ) 
        {
            return varResult._retn();
		}

        typedef std::map<TA_Base_Core::StepNumber, MapParameters> StepNumToParamatersMap;
        StepNumToParamatersMap mapStepParameters;
        StepNumToDetailMap mapSteps;

        TA_Base_Core::StepNumber ulStepPos = static_cast<TA_Base_Core::StepNumber>(0);
        TA_Base_Core::ParameterNumber ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(0);
        unsigned long ulStepPkey = 0;
        unsigned long ulType = 0;
        unsigned long ulSkip = 0;
        unsigned long ulSkipAble = 0;
        unsigned long ulDelay = 0;
        unsigned long ulIgnoreFailure = 0;
        unsigned long ulNoWait = 0;
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
                    ulStepPos = static_cast<TA_Base_Core::StepNumber>(
                        pData->getUnsignedLongData( nLoop, "position" ));
                    if ( mapSteps.end() == mapSteps.find( ulStepPos ))
                    {
                        ulStepPkey = pData->getUnsignedLongData( nLoop, "p_key" );
                        ulType = pData->getUnsignedLongData( nLoop, "type_value" );
                        ulSkip = pData->getUnsignedLongData( nLoop, "skip" );
                        ulSkipAble = pData->getUnsignedLongData( nLoop, "skippable" );
                        ulDelay = pData->getUnsignedLongData( nLoop, "delay" );
                        ulIgnoreFailure = pData->getUnsignedLongData( nLoop, "ignore_failure" );
                        ulNoWait = pData->getUnsignedLongData( nLoop, "no_wait" );
                        strStepName = pData->getStringData( nLoop, "name" );
                        strDescription = pData->getStringData( nLoop, "description" );
                        bNewStep = true;
                    }

                    ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(
                        pData->getUnsignedLongData( nLoop, "ParameterPos" ));
                    strParamaterValue = pData->getStringData( nLoop, "ParameterValue" );
                }
                catch ( std::exception& expWhat )
                {
                    LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, expWhat.what() );
                    continue;
                }
                catch (...)
                {
                    LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "Caught unknown exception" );
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
                        pirRes.first->second.description = CORBA::string_dup(strDescription.c_str());
                        pirRes.first->second.ignoreFailure = static_cast<CORBA::Boolean>(0l != ulIgnoreFailure);
                        pirRes.first->second.nowait = static_cast<CORBA::Boolean>(0l != ulNoWait);
                        pirRes.first->second.name = CORBA::string_dup(strStepName.c_str());
                        pirRes.first->second.position = ulStepPos;
                        pirRes.first->second.skip = static_cast<CORBA::Boolean>(0l != ulSkip);
                        pirRes.first->second.skippable = static_cast<CORBA::Boolean>(0l != ulSkipAble);
                        pirRes.first->second.step = static_cast<TA_Base_Core::StepId>(ulStepPkey);
                        pirRes.first->second.type = static_cast<TA_Base_Core::EStep>(ulType);
                    }
                }

                StepNumToParamatersMap::iterator itParameter = mapStepParameters.find( ulStepPos );
                if ( mapStepParameters.end() == itParameter )
                {
                    MapParameters mapNewParameters;
                    std::pair<StepNumToParamatersMap::iterator, bool> pirInsertResult;
                    pirInsertResult = mapStepParameters.insert( StepNumToParamatersMap::value_type( ulStepPos, mapNewParameters ));
                    if ( !pirInsertResult.second )
                    {
                        LOG_GENERIC(SOURCE_INFO, FATAL_LOG_LEVEL, "Can not insert item to map");
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
            varResult[nResultLoop].detail.description = CORBA::string_dup( itLoop->second.description );
            varResult[nResultLoop].detail.ignoreFailure = itLoop->second.ignoreFailure;
            varResult[nResultLoop].detail.nowait = itLoop->second.nowait;
            varResult[nResultLoop].detail.name = CORBA::string_dup( itLoop->second.name );
            varResult[nResultLoop].detail.position = itLoop->second.position;
            varResult[nResultLoop].detail.skip = itLoop->second.skip;
            varResult[nResultLoop].detail.skippable = itLoop->second.skippable;
            varResult[nResultLoop].detail.step = itLoop->second.step;
            varResult[nResultLoop].detail.type = itLoop->second.type;

            TA_Base_Core::StepNumber ulTempStepNum = itLoop->second.position;
            StepNumToParamatersMap::iterator itFound = mapStepParameters.find( ulTempStepNum );
            if ( mapStepParameters.end() != itFound )
            {
                try
                {
                    convertStepParameters( varResult[nResultLoop].parameters, mapSteps, 
                        ulTempStepNum, itLoop->second.type, itFound->second );
                }
                catch ( std::exception& expWhat )
                {
                    LOG_EXCEPTION_CATCH( SOURCE_INFO, "std::exception", expWhat.what() );
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unkonwn", "Caught unkonwn exception when convert step parameters" );
                }
            }
            ++nResultLoop;
        }

        return varResult._retn();
    }

	TA_Base_Core::StepParameters* PlanDataReadingClass::getStepParameters( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool isStepNumber ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();


		std::ostringstream sql;
		std::vector<std::string> columns;

		if (isStepNumber)
		{
			sql << "select nvl(step_parameters.value, '" << m_nvlValue.c_str() << "')"
				<< " from step_parameters, step_details"
				<< " where step_parameters.fk_step_details = step_details.p_key"
				<< " and step_details.fk_node_tree = " << plan
				<< " and step_details.position = " << step
				<< " order by step_parameters.position";

            columns.push_back( "value" );
		}
		else
		{
			sql << "select nvl(step_parameters.value, '" << m_nvlValue.c_str() << "'), step_details.fk_node_tree"
				<< " from step_parameters, step_details"
				<< " where step_parameters.fk_step_details = " << step
                << "   and step_parameters.fk_step_details = step_details.p_key"
				<< " order by step_parameters.position";

            columns.push_back( "value" );
            columns.push_back( "plan" );
		}

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
		TA_Base_Core::StepParameters_var sp = new TA_Base_Core::StepParameters;

		// don't check for zero rows becase some steps don't
		// have parameters!
		if( NULL == data.get() ) 
        {
			return( sp._retn() );
		}

        // Where the step is identified by its pkey the plan ID that is passed in to this method is not set,
        // so extract it from the SQL query.
        if (!isStepNumber && data->getNumRows() > 0)
        {
            plan = data->getUnsignedLongData( 0, columns[1] );
        }

		TA_Base_Core::EStep stepType = getStepType( plan, step, isStepNumber );


		switch( stepType)
		{
			case TA_Base_Core::END_STEP:
			{
				TA_Base_Core::EndParameters ep = 0;
				sp->end( ep );
			}
			break;

			case TA_Base_Core::ABORT_STEP:
			{
				TA_Base_Core::AbortParameters ap = 0;
				sp->abort( ap );
			}
			break;

			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
			{
				if( 3 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for Active Plan Check Step with step number " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the plan parameter was stored as a plan ID and the yes/no step parameter as a step ID. 
                //         This has been changed to be the plan path and step position, respectively. For backward compatibility
                //         we continue supporting the old mechanism (getPlanId() and getStepId() determine the format used). 
                //         To avoid changes to Plan Manager, ActivePlanCheckParameters are still expressed in terms of plan ID and step ID.

				TA_Base_Core::ActivePlanCheckParameters apc;
                apc.plan    = getPlanId(getStringData(data, 0, columns[0]));
				apc.yesStep = getStepId(plan, getStringData(data, 1, columns[0]));
				apc.noStep  = getStepId(plan, getStringData(data, 2, columns[0]));                
				sp->activePlanCheck(apc);
			}
			break;

			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
			{
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for Assign Camera to Monitor Step with step number " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::AssignCameraToMonitorParameters cameraToMonitor;
				cameraToMonitor.camera        = data->getUnsignedLongData( 0, columns[0] );
				cameraToMonitor.monitor       = data->getUnsignedLongData( 1, columns[0] );
				sp->cameraMonitor(cameraToMonitor);
			}
			break;	

			case TA_Base_Core::DATA_POINT_CHECK_STEP:
			{
				if( 5 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Found " << data->getNumRows() << " parameters, expecting 5 for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the success/failure step parameter was stored as a step ID. 
                //         This has been changed to be the step position instead. For backward compatibility
                //         we continue supporting the old mechanism (setStepId() determines the format used). 
                //         To avoid changes to Plan Manager, DataPointCheckParameters are still expressed in terms of step ID.

                TA_Base_Core::DataPointCheckParameters dpc;
				dpc.dataPoint   = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );
				dpc.value       = buildDataPointValue( data->getStringData( 1, columns[0] ) );
				dpc.testType    = static_cast<TA_Base_Core::EDataPointTest>( data->getUnsignedLongData( 2, columns[0] ) );
				dpc.successStep = getStepId(plan, getStringData(data, 3, columns[0]));
				dpc.failureStep = getStepId(plan, getStringData(data, 4, columns[0]));
				sp->dataPointCheck(dpc);
			}
			break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
			{
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::DataPointSetParameters dps;
				dps.dataPoint = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );
				dps.newValue  = buildDataPointValue( data->getStringData( 1, columns[0] ) );
				sp->dataPointSet(dps);
			}
			break;

			case TA_Base_Core::DECISION_STEP:
			{
				//if( 4 != data->getNumRows() )
				int numRows = data->getNumRows(); 
				if(( 4 != numRows) &&  (9 != numRows))
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the yes/no step parameter was stored as a step ID. 
                //         This has been changed to be the step position instead. For backward compatibility
                //         we continue supporting the old mechanism (setStepId() determines the format used). 
                //         To avoid changes to Plan Manager, DecisionParameters are still expressed in terms of step ID.

				TA_Base_Core::DecisionParameters dec;
				dec.decisionMessage = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );
				dec.responseTimeout = static_cast<TA_Base_Core::Seconds>( data->getUnsignedLongData( 1, columns[0] ) );
				dec.yesStep         = getStepId(plan, getStringData(data, 2, columns[0]));
				dec.noStep          = getStepId(plan, getStringData(data, 3, columns[0]));
				//lizettejl++ - DP-changes
				if ( 4 == numRows )
				{
					// set default values when the new decision step paramaters are not available					
					dec.yesButtonLabel     = CORBA::string_dup("Yes");
					dec.noButtonLabel      = CORBA::string_dup("No");
					dec.pauseButtonLabel   = CORBA::string_dup("Pause");
					dec.noButtonEnabled    = CORBA::Boolean(true);
					dec.pauseButtonEnabled = CORBA::Boolean(false);
				}
				else
				{	
					dec.yesButtonLabel  = CORBA::string_dup( data->getStringData( 4, columns[0] ).c_str() );
					dec.noButtonLabel   = CORBA::string_dup( data->getStringData( 5, columns[0] ).c_str() );
					dec.pauseButtonLabel  = CORBA::string_dup( data->getStringData( 6, columns[0] ).c_str() );
					
					unsigned long buttonEnabled; 
					buttonEnabled = data->getUnsignedLongData( 7, columns[0] );
					dec.noButtonEnabled = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
					
					buttonEnabled = data->getUnsignedLongData( 8, columns[0] );
					dec.pauseButtonEnabled  = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
				}
				//++lizettejl
				sp->decision(dec);
			}
			break;

			case TA_Base_Core::JUMP_STEP:
			{
				if( 1 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the target step parameter was stored as a step ID. 
                //         This has been changed to be the step position instead. For backward compatibility
                //         we continue supporting the old mechanism (setStepId() determines the format used). 
                //         To avoid changes to Plan Manager, JumpParameters are still expressed in terms of step ID.

				TA_Base_Core::JumpParameters jump;
				jump.step = getStepId(plan, getStringData(data, 0, columns[0]));
				sp->jump(jump);
			}
			break;

			case TA_Base_Core::LOG_STEP:
			{
				if( 3 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::LogParameters log;
				log.message = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );


                unsigned long required = data->getUnsignedLongData( 1, columns[0] );
                if ( required )
                    log.logRequired = CORBA::Boolean ( true );
                else
                    log.logRequired = CORBA::Boolean ( false );

				log.responseTimeout = static_cast<TA_Base_Core::Seconds>( data->getUnsignedLongData( 2, columns[0] ) );
				sp->log(log);
			}
			break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            {
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::ActivateCameraPresetParameters cameraPreset;
				cameraPreset.camera        = data->getUnsignedLongData( 0, columns[0] );
				cameraPreset.preset       = data->getUnsignedLongData( 1, columns[0] );
				sp->cameraPreset(cameraPreset);
            }
            break;


			case TA_Base_Core::PROMPT_STEP:
			{
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::PromptParameters prompt;
				prompt.message        = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );
				prompt.dismissTimeout = static_cast<TA_Base_Core::Seconds>( data->getUnsignedLongData( 1, columns[0] ) );
				sp->prompt(prompt);
			}
			break;

			case TA_Base_Core::RUN_PLAN_STEP:
			{
				if( 3 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed to be the plan path.
                //         For backward compatibility we continue supporting the old mechanism (getPlanId() determines the format used). 
                //         To avoid changes to Plan Manager, RunPlanParameters are still expressed in terms of plan ID.

				TA_Base_Core::RunPlanParameters rp;
                rp.plan              = getPlanId(getStringData(data, 0, columns[0]));
				rp.manualLaunch      = static_cast<CORBA::Boolean>(0 != data->getUnsignedLongData(1, columns[0]));
				rp.waitForCompletion = static_cast<CORBA::Boolean>(0 != data->getUnsignedLongData(2, columns[0])); 
				sp->runPlan(rp);
			}
			break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
			{
				if( 1 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed to be the plan path.
                //         For backward compatibility we continue supporting the old mechanism (getPlanId() determines the format used). 
                //         To avoid changes to Plan Manager, TerminatePlanParameters are still expressed in terms of plan ID.

				TA_Base_Core::TerminatePlanParameters tp;
                tp.plan = getPlanId(getStringData(data, 0, columns[0]));
				sp->terminatePlan(tp);
			}
			break;

			case TA_Base_Core::VMS_SET_SCENE_STEP:
			{
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsSetSceneParameters vms;
				vms.vmsSign    = data->getUnsignedLongData( 0, columns[0] );
				vms.scene   = data->getUnsignedLongData( 1, columns[0] );
				sp->vmsSetScene(vms);
			}
			break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            {
                if ( 3 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchGuiAppParameters guiApp;
				guiApp.app    = data->getUnsignedLongData( 0, columns[0] );
				guiApp.args   = CORBA::string_dup( getStringData(data, 1, columns[0] ).c_str() );
                std::string strPos = data->getStringData( 2, columns[0] );

                guiApp.pos = getWindowPos( strPos );

				sp->guiAppParameters(guiApp);
            }
            break;

            case TA_Base_Core::VMS_BLANKOUT_STEP:
            {
				if( 1 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsBlankoutParameters vms;
				vms.vmsSign    = data->getUnsignedLongData( 0, columns[0] );
				sp->vmsBlankout(vms);

            }
            break;

            case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            {
				if( 1 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsRestoreScheduleParameters vms;
				vms.vmsSign    = data->getUnsignedLongData( 0, columns[0] );
				sp->vmsRestoreSchedule(vms);

            }
                break;

            case TA_Base_Core::VIDEO_WALL_STEP:
            {
				if( 2 != data->getNumRows() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VideoWallParameters videoWall;
				videoWall.wallBoard = data->getUnsignedLongData( 0, columns[0] );
				videoWall.layout    = data->getUnsignedLongData( 1, columns[0] );
				sp->videoWallParams(videoWall);

            }
            break;

            case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            {
	            
	            // Must be at least  8 rows, may be more depending on number of specific zones targetted.
	            if ( 8 > data->getNumRows() )
	            {
		            std::ostringstream reasonMessage;
		            reasonMessage  << "Insufficient parameters found for step " << step; 
		            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
	            }
	            
	            const std::string SEPARATOR(",");
	            TA_Base_Core::StationPublicAddressParameters stationPAParams;
	            std::string destination(getStringData(data, 0, columns[0]));
	            

	            // The station destination parameter is formatted like so:
	            // "[station],all_stations"
	            //
	            // Station may be blank (in which case all_stations should be true).
	            std::string station(destination.substr(0, destination.find(SEPARATOR)));
	            destination.erase(0, station.length() + 1);
	            stationPAParams.station.station = CORBA::string_dup(station.c_str());
	            stationPAParams.station.allStations = (destination.substr(0, destination.find(SEPARATOR)) != "0");
	            
	            // The station PA message is made up of 3 numerics formatted like so:
	            // "library_version,library_section,message_id"
	            //
	            // The separator is ignored, so it need not be a comma.
	            std::istringstream message(getStringData(data, 1, columns[0]));
	            message >> stationPAParams.message.libVersion;
	            message.ignore(1);
	            message >> stationPAParams.message.libSection;
	            message.ignore(1);
	            message >> stationPAParams.message.messageId;
	            
	            //chime
	            unsigned long chime = data->getUnsignedLongData( 2, columns[0] );
	            if ( chime )
                    stationPAParams.chime = CORBA::Boolean ( true );
                else
                    stationPAParams.chime = CORBA::Boolean ( false );


                //cyclic
                unsigned long cyclic = data->getUnsignedLongData( 3, columns[0] );
                if ( cyclic )
                    stationPAParams.cyclic = CORBA::Boolean ( true );
                else
                    stationPAParams.cyclic = CORBA::Boolean ( false );

                //TIS Synch
                unsigned long synch = data->getUnsignedLongData( 4, columns[0] );
                if ( synch )
                    stationPAParams.TISSynch = CORBA::Boolean ( true );
                else
                    stationPAParams.TISSynch = CORBA::Boolean ( false );

                //duration
                 stationPAParams.duration = data->getUnsignedLongData(5, columns[0]);

                //interval
                stationPAParams.interval = data->getUnsignedLongData(6, columns[0]);


                // Finally, the zones. These are stored in batches of 15 per row.
                // The row format is: "zoneid1,zoneid2, ... ,zoneidN"
                TA_Base_Core::IData* zoneData = data.release();

                try
                {
	                stationPAParams.zones.length(0); // Assume there are no zones configured
	                unsigned long row(7); // First batch - start from row #8. It may or may not exist...
	                
	                do
	                {
                        for (; row < zoneData->getNumRows(); row++)
                        {
                            std::string zoneList(zoneData->getStringData(row, columns[0]));

                            while (!zoneList.empty())
                            {
	                            std::string zone(zoneList.substr(0, zoneList.find(SEPARATOR)));
	                            zoneList.erase(0, zone.length() + 1);
	                            
	                            unsigned long i(stationPAParams.zones.length());
	                            
	                            // Expand the sequence to receive the new string.
	                            stationPAParams.zones.length(i + 1);
	                            stationPAParams.zones[i] = atol (zone.c_str());
                            }   
                        }
								
                        // Need to delete the current data object before asking for more.
                        delete zoneData;
                        zoneData = NULL;
								
                        // Next batch of rows we start processing from top.
                        row = 0;
                    }
                    while (database->moreData(zoneData));
                }
                catch (...)
                {
	                delete zoneData;
	                zoneData = NULL;
	                
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Failed to retrieve zone ID parameters for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));                       
                }

                sp->stationPublicAddress(stationPAParams);						
            }
            break;

            case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            {
                // Must be 4 rows
                if ( 4 > data->getNumRows() )
                {
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Insufficient parameters found for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                const std::string SEPARATOR(",");
                TA_Base_Core::TrainPublicAddressParameters trainPAParams;

                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream destination(getStringData(data, 0, columns[0]));
                destination >> trainPAParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainPAParams.destination.trainGroup);

                // The station PA message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream message(getStringData(data, 1, columns[0]));
                message >> trainPAParams.message.libVersion;
                message.ignore(1);
                message >> trainPAParams.message.libSection;
                message.ignore(1);
                message >> trainPAParams.message.messageId;

                //cyclic
                unsigned long cyclic = data->getUnsignedLongData( 2, columns[0] );
                if ( cyclic )
	                trainPAParams.cyclic = CORBA::Boolean ( true );
                else
	                trainPAParams.cyclic = CORBA::Boolean ( false );

                //duration
                trainPAParams.duration = data->getUnsignedLongData(3, columns[0]);

                sp->trainPublicAddress(trainPAParams);
            }
            break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            {
                if ( 2 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                TA_Base_Core::SendSmsMessageParameters sendSmsMessageParams;
                sendSmsMessageParams.recipientIdentifier = CORBA::string_dup( data->getStringData( 0, columns[0] ).c_str() );
                sendSmsMessageParams.textMessage = CORBA::string_dup( data->getStringData( 1, columns[0] ).c_str() );
                sp->sendSmsMessage( sendSmsMessageParams );
            }
            break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            {
                if ( 3 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchSchematicParameters launchSchematicParams;
				launchSchematicParams.schematic = CORBA::string_dup( getStringData(data, 0, columns[0] ).c_str() );
                launchSchematicParams.screen = data->getUnsignedLongData( 1, columns[0] );
				launchSchematicParams.commands = CORBA::string_dup( getStringData(data, 2, columns[0] ).c_str() );

				sp->launchSchematic(launchSchematicParams);
            }
            break;

            case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            {
                if ( 3 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << getStepNumber(step); 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                // TD8736: Originally, the success/failure step parameter was stored as a step ID. 
                //         This has been changed to be the step position instead. For backward compatibility
                //         we continue supporting the old mechanism (setStepId() determines the format used). 
                //         To avoid changes to Plan Manager, EvaluateExpressionParameters are still expressed in terms of step ID.

                TA_Base_Core::EvaluateExpressionParameters evalExp;
                evalExp.expression  = CORBA::string_dup( getStringData(data, 0, columns[0] ).c_str() );
                evalExp.successStep = getStepId(plan, getStringData(data, 1, columns[0]));
                evalExp.failureStep = getStepId(plan, getStringData(data, 2, columns[0]));

                sp->evaluateExpression(evalExp);
            }
            break;
				 
            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            {
                // Must be at least 5 rows, will be more if specific PIDs have been targetted.
                if ( 5 > data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }
                
				TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams;
                std::string destination(getStringData(data, 0, columns[0]));
                stationTisParams.stationLevel = CORBA::string_dup(getStringData(data, 1, columns[0]).c_str());
                std::istringstream message(getStringData(data, 2, columns[0]));
                stationTisParams.priority = static_cast<unsigned short>(data->getUnsignedLongData(3, columns[0]));
                stationTisParams.duration = data->getUnsignedLongData(4, columns[0]);

                // The station TIS message destination parameter is formatted like so:
                // "[station],all_stations"
                //
                // Station may be blank (in which case all_stations should be true).
                const std::string SEPARATOR(",");

                std::string station(destination.substr(0, destination.find(SEPARATOR)));
                destination.erase(0, station.length() + 1);
                stationTisParams.destination.station = CORBA::string_dup(station.c_str());

                stationTisParams.destination.allStations = (destination.substr(0, destination.find(SEPARATOR)) != "0");
                
                // The station TIS message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.
                message >> stationTisParams.message.libraryVersion;
                message.ignore(1);
                message >> stationTisParams.message.librarySection;
                message.ignore(1);
                message >> stationTisParams.message.messageId;

                // Finally, the PIDs. These are stored in batches of several per row.
                // The row format is: "pid1,pid2, ... ,pidN"
                TA_Base_Core::IData* pidData = data.release();

                try
                {
                    stationTisParams.stationPids.length(0); // Assume there are no PIDs configured
                    unsigned long row(5); // First batch - start from row #6. It may or may not exist...

                    do
                    {
                        for (; row < pidData->getNumRows(); row++)
                        {
                            std::string pidList(pidData->getStringData(row, columns[0]));

                            while (!pidList.empty())
                            {
                                std::string pid(pidList.substr(0, pidList.find(SEPARATOR)));
                                pidList.erase(0, pid.length() + 1);

                                unsigned long i(stationTisParams.stationPids.length());

                                // Expand the sequence to receive the new string.
                                stationTisParams.stationPids.length(i + 1);
                                stationTisParams.stationPids[i] = CORBA::string_dup(pid.c_str());
                            }   
                        }

                        // Need to delete the current data object before asking for more.
                        delete pidData;
                        pidData = NULL;

                        // Next batch of rows we start processing from top.
                        row = 0;
                    }
                    while (database->moreData(pidData));
                }
                catch (...)
                {
                    delete pidData;
                    pidData = NULL;

					std::ostringstream reasonMessage;
					reasonMessage  << "Failed to retrieve PID parameters for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));                       
                }

                sp->stationTravellerInfoSystem(stationTisParams);
            }
            break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            {
                if ( 4 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }
                
				TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParams;
                std::istringstream destination(getStringData(data, 0, columns[0]));
                std::istringstream message(getStringData(data, 1, columns[0]));
                trainTisParams.priority = static_cast<unsigned short>(data->getUnsignedLongData(2, columns[0]));
                trainTisParams.duration = data->getUnsignedLongData(3, columns[0]);


                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
 
                destination >> trainTisParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainTisParams.destination.trainGroup);
                
                // The train TIS message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.

                message >> trainTisParams.message.libraryVersion;
                message.ignore(1);
                message >> trainTisParams.message.librarySection;
                message.ignore(1);
                message >> trainTisParams.message.messageId;

                sp->trainTravellerInfoSystem(trainTisParams);
            }
            break;

			//lizettejl++ - failure summary step (DP-changes)
			case TA_Base_Core::FAILURE_SUMMARY_STEP:
			{
				if ( 1 != data->getNumRows() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::FailureSummaryParameters summary;
				summary.failureCategory = (TA_Base_Core::EStepFailureCategory) data->getUnsignedLongData(0, columns[0]);

				sp->failureSummary(summary);
			}
			break;
			//++lizettejl

			default:
				// throw an exception
				break;
		}

		return sp._retn();
	}



	TA_Base_Core::StepNumber PlanDataReadingClass::getStepCount( TA_Base_Core::NodeId plan ) const
	{
		TA_Base_Core::StepNumber rval = 0;

		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		// Step count query
		sql << "select count(position) as steps from step_details where fk_node_tree =  " << plan;

		std::vector<std::string> columns;
		columns.push_back( "steps" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
		
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
		{

			return rval;
		}
		
		rval = data->getUnsignedLongData( 0, columns[0] );

		return( rval );
	}

	TA_Base_Core::EApprovalState PlanDataReadingClass::getPlanApprovalState(TA_Base_Core::NodeId plan) const
	{
		TA_Base_Core::EApprovalState rval = TA_Base_Core::UNDEFINED_APPROVAL_STATE;

		TA_Base_Core::IDatabase* database= getDatabase();


		std::ostringstream sql;
		
		sql << "select state_value from approval_state where p_key = "
			<< "(select node_tree.fk_approval_state from node_tree where p_key =  "
			<< plan << ")";

		std::vector<std::string> columns;
		columns.push_back("state_value");

		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
		if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		{
			return rval;
		}

		rval = (TA_Base_Core::EApprovalState) data->getUnsignedLongData (0, columns[0] );

		return (rval);
	}


	int PlanDataReadingClass::getCategoryLevel(TA_Base_Core::NodeId category)
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;
		
		// Node summary query
		sql << "select parent_node" 
			<< " from node_tree"
			<< " where p_key = "
			<< category;
		
		std::vector<std::string> columns;
		columns.push_back( "parent_node" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
		
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


	TA_Base_Core::StepNumber PlanDataReadingClass::getStepNumber ( TA_Base_Core::StepId step) const
	{
		TA_Base_Core::StepNumber rval = 0;
		TA_Base_Core::IDatabase* database= getDatabase();


		std::ostringstream sql;
		
		sql << "select position from step_details where p_key =  " << step;

		std::vector<std::string> columns;
		columns.push_back("position");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( sql.str(), columns ) );
			PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
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


    TA_Base_Core::StepId PlanDataReadingClass::getStepId( TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber step) const
    {
		TA_Base_Core::StepId rval = 0;
		TA_Base_Core::IDatabase* database= getDatabase();
 
		std::ostringstream sql;
		
		sql << "select p_key from step_details "
            << "where fk_node_tree =  " << plan << " and position = " << step;

		std::vector<std::string> columns;
		columns.push_back("stepId");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( sql.str(), columns ) );
			PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
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


	std::string PlanDataReadingClass::getNodePath( TA_Base_Core::NodeId plan ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::string nodePath;

		TA_Base_Core::NodeId currentNode(plan);

		while( 0 != currentNode )
		{
			std::ostringstream sql;

			// Step details query
			sql << "select name, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;

			std::vector<std::string> columns;
			columns.push_back( "name" );
			columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
			PlanDataBaseHelper dataBaseHelper(database); //  CL-21716 
						
			if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode;  
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Failed at node %lu to find path for plan %lu", currentNode, plan );
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
            return nodePath;
        else
			return( nodePath.substr(2) );
	}

    // Returns the list of node IDs of the nodes making up the specified plan's path. 
    // The first element in the list is the root node's ID and the last element is 
    // the actual plan node's ID.
	TA_Base_Core::NodeIds* PlanDataReadingClass::getNodeIdsInNodePath( TA_Base_Core::NodeId plan ) const
	{

        std::vector<TA_Base_Core::NodeId> nodeIds;

		TA_Base_Core::IDatabase* database= getDatabase();
 

		TA_Base_Core::NodeId currentNode(plan);

		while( 0 != currentNode )
		{
			std::ostringstream sql;

			// Node ID + parent ID query
			sql << "select p_key, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;

			std::vector<std::string> columns;
			columns.push_back( "p_key" );
			columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
			PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
						
			if ( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode; 

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Failed at node %lu to find path for plan %lu", currentNode, plan );
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
			}

            nodeIds.insert( nodeIds.begin(), data->getUnsignedLongData( 0, columns[0] ) );

			currentNode = data->getUnsignedLongData( 0, columns[1] );
		}

		TA_Base_Core::NodeIds_var ni = new TA_Base_Core::NodeIds;

		ni->length(nodeIds.size());

		std::vector<TA_Base_Core::NodeId>::iterator it;

		int i = 0;
		for (it = nodeIds.begin(); it != nodeIds.end(); it++)
		{
			ni.operator [](i++) = *it;
		} 
		return ni._retn();
	}

	TA_Base_Core::ENode PlanDataReadingClass::getNodeType( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();
 
		std::ostringstream sql;

		// Node summary query
		sql << "select node_type.type_value" 
			<< " from node_tree, node_type"
			<< " where node_tree.fk_node_type = node_type.p_key"
			<< " and   node_tree.p_key = " << node;
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

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

			case TA_Base_Core::CATEGORY_NODE:
			{
				return( TA_Base_Core::CATEGORY_NODE );
			}
			break;

			default:
			{
				TA_THROW( TA_Base_Core::DataException( "Invalid node type", TA_Base_Core::DataException::WRONG_TYPE, data->getStringData( 0, columns[0] ) ) );
			}
			break;
		}
	}

	TA_Base_Core::EStep PlanDataReadingClass::getStepType( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool byStepNumber ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		if (byStepNumber)
		{
			sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.fk_node_tree = " << plan
				<< " and step_details.position = " << step;
		}
		else
		{
			sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.p_key = " << step;
		}
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			return( TA_Base_Core::UNDEFINED_STEP );
        } 
		return(  static_cast<TA_Base_Core::EStep>( data->getUnsignedLongData( 0, columns[0] ) ) );
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getStepDetails( TA_Base_Core::NodeId plan, const std::string& stepConstraint ) const
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		// Step details query
		sql << "select step_details.p_key, step_details.position, step_details.name,"
            << " step_type.type_value, nvl(step_details.description, '" << m_nvlValue.c_str() << "'),"
			<< " step_details.skip, step_details.skippable, step_details.delay,"
            << " nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0) "
			<< " from step_details, node_tree, step_type"
			<< " where step_details.fk_node_tree = node_tree.p_key"
			<< " and step_details.fk_step_type = step_type.p_key"
			<< " and step_details.fk_node_tree = " << plan;

		if( stepConstraint.length() > 0 )
		{
			sql << " and step_details.position in (" << stepConstraint << ")";
		}
		
		sql << " order by step_details.position";

        std::vector<std::string> columns;
		columns.push_back( "p_key");
		columns.push_back( "position" );
		columns.push_back( "name" );
        columns.push_back( "type" );
        columns.push_back( "description" );
		columns.push_back( "skip" );
		columns.push_back( "skippable" );
		columns.push_back( "delay" ); 
		columns.push_back( "ignore_failure" ); 
		columns.push_back( "no_wait" ); 

		TA_Base_Core::IData* data = database->executeQuery( sql.str(), columns ) ;
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
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
				sd[rowsFetched + dataIdx].step        = static_cast<TA_Base_Core::StepId> (dataPtr->getUnsignedLongData( dataIdx, columns[0] ) );
				sd[rowsFetched + dataIdx].position    = dataPtr->getUnsignedLongData( dataIdx, columns[1] );
				sd[rowsFetched + dataIdx].name        = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[2] ).c_str() );
				sd[rowsFetched + dataIdx].type        = static_cast<TA_Base_Core::EStep>( dataPtr->getUnsignedLongData( dataIdx, columns[3] ) );
				sd[rowsFetched + dataIdx].description = CORBA::string_dup( getStringData(dataPtr, dataIdx, columns[4] ).c_str() );
				sd[rowsFetched + dataIdx].skip        = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[5] ) );
				sd[rowsFetched + dataIdx].skippable   = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[6] ) );
				sd[rowsFetched + dataIdx].delay       = static_cast<TA_Base_Core::Seconds>( dataPtr->getUnsignedLongData( dataIdx, columns[7] ) );
				sd[rowsFetched + dataIdx].ignoreFailure= static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[8] ) );
				sd[rowsFetched + dataIdx].nowait= static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[9] ) );
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr = std::auto_ptr<TA_Base_Core::IData>(NULL);		// ensure memory is released
			data = NULL;
		}
		while ( database->moreData( data ) );

		return sd._retn();
	}


	TA_Base_Core::Plan* PlanDataReadingClass::getPlan( TA_Base_Core::NodeId plan )
	{
		TA_Base_Core::Plan_var thePlan = new TA_Base_Core::Plan;
		TA_Base_Core::NodeDetail_var nodeDetail = getNodeDetail(plan);

		thePlan->detail = nodeDetail->plan();

		TA_Base_Core::StepDetails_var stepDetails = getAllStepDetails(plan);

		thePlan->stepseq.length( stepDetails->length() );

		for( unsigned long i=0; i<stepDetails->length(); i++ )
		{
			thePlan->stepseq[i].detail = stepDetails[i];

			TA_Base_Core::StepParameters_var stepParams = getStepParameters( plan, stepDetails[i].step);
			thePlan->stepseq[i].parameters = stepParams;
		}

		return( thePlan._retn() );
	}

	TA_Base_Core::DataPointValue PlanDataReadingClass::buildDataPointValue( const std::string& dbString ) const
	{
		TA_Base_Core::DataPointValue rval;

		std::istringstream st(dbString);

		
		CORBA::ULong dataType;
		st >> dataType;
		st.ignore();
		
		switch( dataType )
		{
			case TA_Base_Core::BOOLEAN_DATA_POINT:
				CORBA::Boolean tmpBool;
				st >> tmpBool;
				rval.booleanValue( tmpBool );
				break;

			case TA_Base_Core::NUMERIC_DATA_POINT:
				TA_Base_Core::NumericDataPoint tmpNd;
				st >> tmpNd.value;
				st.ignore();
				st >> tmpNd.precision;
				rval.numericValue(tmpNd);
				break;

			case TA_Base_Core::TEXT_DATA_POINT:
				{

					std::string tmpStr = st.str().substr(2, st.str().length());
					rval.textValue( tmpStr.c_str() );
				}
				break;
		}
		return rval;
	}

	TA_Base_Core::WindowPosition PlanDataReadingClass::getWindowPos( const std::string& dbString ) const
	{
		TA_Base_Core::WindowPosition rval;

		std::istringstream st(dbString);
		st >> rval.top;
		st.ignore();
        st >> rval.left;
        st.ignore();
        st >> rval.width;
        st.ignore();
        st >> rval.height;
		return rval;
	}

	unsigned long PlanDataReadingClass::getNextPlanSeqNumber(void)
	{
		TA_Base_Core::IDatabase* database= getDatabase();

		std::ostringstream sql;

		sql << "select plans_seq.nextval Next_Step_Id from dual";
		
        std::vector<std::string> columns;
		columns.push_back( "Next_Step_Id" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			std::cout << "No next step id returned." << std::endl;
        }

		return(  data->getUnsignedLongData( 0, columns[0] ) );
	}

	TA_Base_Core::IDatabase* PlanDataReadingClass::getDatabase() const
	{   
		TA_Base_Core::IDatabase* database = NULL;
		try
		{
            database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);//CL-21557
		}
		catch ( ... )
		{
			std::string message = "Failed to connect to the database.";

			//Error using 'EXCEPTION_CATCH_LOG_TYPE'. cuilike++

			//LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, ERROR_LOG_LEVEL, message.c_str() );
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", message.c_str() );
			//++cuilike

			TA_THROW(  TA_Base_Core::DatabaseException(message.c_str()) );
		}
		return database;
	}

    std::string PlanDataReadingClass::getStringData(std::auto_ptr<class TA_Base_Core::IData>& data, unsigned long row, const std::string& columnName) const
    {
        std::string retValue = data->getStringData(row, columnName);
        
        if ( retValue.compare(m_nvlValue) == 0 )
            return "";

        return retValue;
    }

    TA_Base_Core::NodeId PlanDataReadingClass::getPlanId(const std::string& planReference) const
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
                    LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Failed to retrieve plan ID for plan %s", planReference.c_str());
                }
            }
        }

        return 0;
    }

    TA_Base_Core::StepId PlanDataReadingClass::getStepId(TA_Base_Core::NodeId plan, const std::string& stepReference) const
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
                    LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, 
                        "Failed to retrieve step ID for plan %lu, step %s", plan, stepReference.c_str());
                }
            }
        }

        return 0;
    }
    
    TA_Base_Core::StepId PlanDataReadingClass::getStepId( const StepNumToDetailMap& refStepDetails, const std::string& stepReference ) const
    {
        if (!stepReference.empty())
        {
            if (stepReference.find_first_not_of(" 0123456789") == std::string::npos)
            {
                return ::strtoul(stepReference.c_str(), NULL, 10);
            }
            else
            {
                // When the step is referenced by its relative position, the format used is "#<step number".
                std::string stepNumber(stepReference.substr(stepReference.find_first_of("0123456789")));
                TA_Base_Core::StepNumber ulStepNum = 
                    static_cast<TA_Base_Core::StepNumber>(::strtoul(stepNumber.c_str(), NULL, 10));
                StepNumToDetailMap::const_iterator itFound = refStepDetails.find( ulStepNum );
                if ( refStepDetails.end() != itFound )
                {
                    return itFound->second.step;
                }
            }
        }
        return 0;
    }
    
    void PlanDataReadingClass::convertStepParameters(  TA_Base_Core::StepParameters& refStepParameters,
        const StepNumToDetailMap& refStepDetails, const TA_Base_Core::StepId step, 
        const TA_Base_Core::EStep eType, MapParameters& mapParameters ) const
    {
        TA_Base_Core::StepNumber nPos = static_cast<TA_Base_Core::StepNumber>(1);

		switch( eType )
		{
			case TA_Base_Core::END_STEP:
			{
				TA_Base_Core::EndParameters ep = 0;
				refStepParameters.end( ep );
			}
			break;

			case TA_Base_Core::ABORT_STEP:
			{
				TA_Base_Core::AbortParameters ap = 0;
				refStepParameters.abort( ap );
			}
			break;

			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for Active Plan Check Step with step number " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::ActivePlanCheckParameters apc;
                apc.plan    = getPlanId(mapParameters[nPos++]);
				apc.yesStep = getStepId(refStepDetails, mapParameters[nPos++]);
				apc.noStep  = getStepId(refStepDetails, mapParameters[nPos++]);                
				refStepParameters.activePlanCheck(apc);
			}
			break;

			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for Assign Camera to Monitor Step with step number " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::AssignCameraToMonitorParameters cameraToMonitor;
				cameraToMonitor.camera = stringToUlong( mapParameters[nPos++] );
				cameraToMonitor.monitor = stringToUlong( mapParameters[nPos++] );
				refStepParameters.cameraMonitor(cameraToMonitor);
			}
			break;

			case TA_Base_Core::DATA_POINT_CHECK_STEP:
			{
				if( 5 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 5 for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                TA_Base_Core::DataPointCheckParameters dpc;
				dpc.dataPoint   = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dpc.value       = buildDataPointValue( mapParameters[nPos++] );
				dpc.testType    = static_cast<TA_Base_Core::EDataPointTest>( stringToUlong( mapParameters[nPos++] ));
				dpc.successStep = getStepId(refStepDetails, mapParameters[nPos++]);
				dpc.failureStep = getStepId(refStepDetails, mapParameters[nPos++]);
				refStepParameters.dataPointCheck(dpc);
			}
			break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::DataPointSetParameters dps;
				dps.dataPoint = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dps.newValue  = buildDataPointValue( mapParameters[nPos++] );
				refStepParameters.dataPointSet(dps);
			}
			break;

			case TA_Base_Core::DECISION_STEP:
			{
				int numRows = mapParameters.size(); 
				if(( 4 != numRows) &&  (9 != numRows))
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::DecisionParameters dec;
				dec.decisionMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dec.responseTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
				dec.yesStep         = getStepId(refStepDetails, mapParameters[nPos++]);
				dec.noStep          = getStepId(refStepDetails, mapParameters[nPos++]);

				if ( 4 == numRows )
				{
					// set default values when the new decision step paramaters are not available					
					dec.yesButtonLabel     = CORBA::string_dup("Yes");
					dec.noButtonLabel      = CORBA::string_dup("No");
					dec.pauseButtonLabel   = CORBA::string_dup("Pause");
					dec.noButtonEnabled    = CORBA::Boolean(true);
					dec.pauseButtonEnabled = CORBA::Boolean(false);
				}
				else
				{	
					dec.yesButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );
					dec.noButtonLabel   = CORBA::string_dup( mapParameters[nPos++].c_str() );
					dec.pauseButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );
					
					unsigned long buttonEnabled; 
					buttonEnabled = stringToUlong( mapParameters[nPos++] );
					dec.noButtonEnabled = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
					
					buttonEnabled = stringToUlong( mapParameters[nPos++] );
					dec.pauseButtonEnabled  = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
				}
				refStepParameters.decision(dec);
			}
			break;

			case TA_Base_Core::JUMP_STEP:
			{
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::JumpParameters jump;
				jump.step = getStepId(refStepDetails, mapParameters[nPos++]);
				refStepParameters.jump(jump);
			}
			break;

			case TA_Base_Core::LOG_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::LogParameters log;
				log.message = CORBA::string_dup( mapParameters[nPos++].c_str() );

                unsigned long required = stringToUlong( mapParameters[nPos++] );
                if ( required )
                    log.logRequired = CORBA::Boolean ( true );
                else
                    log.logRequired = CORBA::Boolean ( false );

				log.responseTimeout = static_cast<TA_Base_Core::Seconds>( stringToUlong( mapParameters[nPos++] ));
				refStepParameters.log(log);
			}
			break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            {
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::ActivateCameraPresetParameters cameraPreset;
				cameraPreset.camera        = stringToUlong( mapParameters[nPos++] );
				cameraPreset.preset       = stringToUlong( mapParameters[nPos++] );
				refStepParameters.cameraPreset(cameraPreset);
            }
            break;


			case TA_Base_Core::PROMPT_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::PromptParameters prompt;
				prompt.message        = CORBA::string_dup( mapParameters[nPos++].c_str() );
				prompt.dismissTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
				refStepParameters.prompt(prompt);
			}
			break;

			case TA_Base_Core::RUN_PLAN_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::RunPlanParameters rp;
                rp.plan              = getPlanId(mapParameters[nPos++]);
				rp.manualLaunch      = static_cast<CORBA::Boolean>(0 != stringToUlong( mapParameters[nPos++] ));
				rp.waitForCompletion = static_cast<CORBA::Boolean>(0 != stringToUlong( mapParameters[nPos++] )); 
				refStepParameters.runPlan(rp);
			}
			break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
			{
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::TerminatePlanParameters tp;
                tp.plan = getPlanId(mapParameters[nPos++]);
				refStepParameters.terminatePlan(tp);
			}
			break;

			case TA_Base_Core::VMS_SET_SCENE_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsSetSceneParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				vms.scene   = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsSetScene(vms);
			}
			break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchGuiAppParameters guiApp;
				guiApp.app    = stringToUlong( mapParameters[nPos++] );
				guiApp.args   = CORBA::string_dup( mapParameters[nPos++].c_str() );
                std::string strPos = mapParameters[nPos++];

                guiApp.pos = getWindowPos( strPos );

				refStepParameters.guiAppParameters(guiApp);
            }
            break;

            case TA_Base_Core::VMS_BLANKOUT_STEP:
            {
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsBlankoutParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsBlankout(vms);

            }
            break;

            case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            {
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsRestoreScheduleParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsRestoreSchedule(vms);

            }
                break;

            case TA_Base_Core::VIDEO_WALL_STEP:
            {
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VideoWallParameters videoWall;
				videoWall.wallBoard = stringToUlong( mapParameters[nPos++] );
				videoWall.layout    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.videoWallParams(videoWall);

            }
            break;

            case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            {
	            
	            // Must be at least  8 rows, may be more depending on number of specific zones targetted.
	            if ( 8 > mapParameters.size() )
	            {
		            std::ostringstream reasonMessage;
		            reasonMessage  << "Insufficient parameters found for step " << step; 
		            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
	            }
	            
	            const std::string SEPARATOR(",");
	            TA_Base_Core::StationPublicAddressParameters stationPAParams;
	            std::string destination(mapParameters[nPos++]);
	            

	            // The station destination parameter is formatted like so:
	            // "[station],all_stations"
	            //
	            // Station may be blank (in which case all_stations should be true).
	            std::string station(destination.substr(0, destination.find(SEPARATOR)));
	            destination.erase(0, station.length() + 1);
	            stationPAParams.station.station = CORBA::string_dup(station.c_str());
	            stationPAParams.station.allStations = (destination.substr(0, destination.find(SEPARATOR)) != "0");
	            
	            // The station PA message is made up of 3 numerics formatted like so:
	            // "library_version,library_section,message_id"
	            //
	            // The separator is ignored, so it need not be a comma.
	            std::istringstream message(mapParameters[nPos++]);
	            message >> stationPAParams.message.libVersion;
	            message.ignore(1);
	            message >> stationPAParams.message.libSection;
	            message.ignore(1);
	            message >> stationPAParams.message.messageId;
	            
	            //chime
	            unsigned long chime = stringToUlong( mapParameters[nPos++] );
	            if ( chime )
                    stationPAParams.chime = CORBA::Boolean ( true );
                else
                    stationPAParams.chime = CORBA::Boolean ( false );


                //cyclic
                unsigned long cyclic = stringToUlong( mapParameters[nPos++] );
                if ( cyclic )
                    stationPAParams.cyclic = CORBA::Boolean ( true );
                else
                    stationPAParams.cyclic = CORBA::Boolean ( false );

                //TIS Synch
                unsigned long synch = stringToUlong( mapParameters[nPos++] );
                if ( synch )
                    stationPAParams.TISSynch = CORBA::Boolean ( true );
                else
                    stationPAParams.TISSynch = CORBA::Boolean ( false );

                //duration
                 stationPAParams.duration = stringToUlong( mapParameters[nPos++] );

                //interval
                stationPAParams.interval = stringToUlong( mapParameters[nPos++] );


                // Finally, the zones. These are stored in batches of 15 per row.
                // The row format is: "zoneid1,zoneid2, ... ,zoneidN"

                try
                {
	                stationPAParams.zones.length(0); // Assume there are no zones configured

                    while ( nPos <= mapParameters.size() )
                    {
                        std::string zoneList( mapParameters[nPos++] );

                        while (!zoneList.empty())
                        {
	                        std::string zone(zoneList.substr(0, zoneList.find(SEPARATOR)));
	                        zoneList.erase(0, zone.length() + 1);
	                        
	                        unsigned long i(stationPAParams.zones.length());
	                        
	                        // Expand the sequence to receive the new string.
	                        stationPAParams.zones.length(i + 1);
	                        stationPAParams.zones[i] = atol (zone.c_str());
                        }   
                    }
                }
                catch (...)
                {
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Failed to retrieve zone ID parameters for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));                       
                }

                refStepParameters.stationPublicAddress(stationPAParams);						
            }
            break;

            case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            {
                // Must be 4 rows
                if ( 4 > mapParameters.size() )
                {
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Insufficient parameters found for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                const std::string SEPARATOR(",");
                TA_Base_Core::TrainPublicAddressParameters trainPAParams;

                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream destination( mapParameters[nPos++] );
                destination >> trainPAParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainPAParams.destination.trainGroup);

                // The station PA message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream message( mapParameters[nPos++] );
                message >> trainPAParams.message.libVersion;
                message.ignore(1);
                message >> trainPAParams.message.libSection;
                message.ignore(1);
                message >> trainPAParams.message.messageId;

                //cyclic
                unsigned long cyclic = stringToUlong( mapParameters[nPos++] );
                if ( cyclic )
	                trainPAParams.cyclic = CORBA::Boolean ( true );
                else
	                trainPAParams.cyclic = CORBA::Boolean ( false );

                //duration
                trainPAParams.duration = stringToUlong( mapParameters[nPos++] );

                refStepParameters.trainPublicAddress(trainPAParams);
            }
            break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            {
                if ( 2 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                TA_Base_Core::SendSmsMessageParameters sendSmsMessageParams;
                sendSmsMessageParams.recipientIdentifier = CORBA::string_dup( mapParameters[nPos++].c_str() );
                sendSmsMessageParams.textMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
                refStepParameters.sendSmsMessage( sendSmsMessageParams );
            }
            break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchSchematicParameters launchSchematicParams;
				launchSchematicParams.schematic = CORBA::string_dup( mapParameters[nPos++].c_str() );
                launchSchematicParams.screen = stringToUlong( mapParameters[nPos++] );
				launchSchematicParams.commands = CORBA::string_dup( mapParameters[nPos++].c_str() );

				refStepParameters.launchSchematic(launchSchematicParams);
            }
            break;

            case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::EvaluateExpressionParameters evalExp;
                evalExp.expression  = CORBA::string_dup( mapParameters[nPos++].c_str() );
                evalExp.successStep = getStepId(refStepDetails, mapParameters[nPos++]);
                evalExp.failureStep = getStepId(refStepDetails, mapParameters[nPos++]);

                refStepParameters.evaluateExpression(evalExp);
            }
            break;
				 
            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            {
                // Must be at least 5 rows, will be more if specific PIDs have been targetted.
                if ( 5 > mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }
                
				TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams;
                std::string destination(mapParameters[nPos++]);
                stationTisParams.stationLevel = CORBA::string_dup(mapParameters[nPos++].c_str());
                std::istringstream message(mapParameters[nPos++]);
                stationTisParams.priority = static_cast<unsigned short>(stringToUlong(mapParameters[nPos++]));
                stationTisParams.duration = stringToUlong(mapParameters[nPos++]);

                // The station TIS message destination parameter is formatted like so:
                // "[station],all_stations"
                //
                // Station may be blank (in which case all_stations should be true).
                const std::string SEPARATOR(",");

                std::string station(destination.substr(0, destination.find(SEPARATOR)));
                destination.erase(0, station.length() + 1);
                stationTisParams.destination.station = CORBA::string_dup(station.c_str());

                stationTisParams.destination.allStations = (destination.substr(0, destination.find(SEPARATOR)) != "0");
                
                // The station TIS message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.
                message >> stationTisParams.message.libraryVersion;
                message.ignore(1);
                message >> stationTisParams.message.librarySection;
                message.ignore(1);
                message >> stationTisParams.message.messageId;

                // Finally, the PIDs. These are stored in batches of several per row.
                // The row format is: "pid1,pid2, ... ,pidN"

                try
                {
                    stationTisParams.stationPids.length(0); // Assume there are no PIDs configured

                    while ( nPos <= mapParameters.size() )
                    {
                        std::string pidList(mapParameters[nPos++]);

                        while (!pidList.empty())
                        {
                            std::string pid(pidList.substr(0, pidList.find(SEPARATOR)));
                            pidList.erase(0, pid.length() + 1);

                            unsigned long i(stationTisParams.stationPids.length());

                            // Expand the sequence to receive the new string.
                            stationTisParams.stationPids.length(i + 1);
                            stationTisParams.stationPids[i] = CORBA::string_dup(pid.c_str());
                        }   
                    }
                }
                catch (...)
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Failed to retrieve PID parameters for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));                       
                }

                refStepParameters.stationTravellerInfoSystem(stationTisParams);
            }
            break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            {
                if ( 4 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }
                
				TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParams;
                std::istringstream destination(mapParameters[nPos++]);
                std::istringstream message(mapParameters[nPos++]);
                trainTisParams.priority = static_cast<unsigned short>(stringToUlong(mapParameters[nPos++]));
                trainTisParams.duration = stringToUlong(mapParameters[nPos++]);


                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
 
                destination >> trainTisParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainTisParams.destination.trainGroup);
                
                // The train TIS message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.

                message >> trainTisParams.message.libraryVersion;
                message.ignore(1);
                message >> trainTisParams.message.librarySection;
                message.ignore(1);
                message >> trainTisParams.message.messageId;

                refStepParameters.trainTravellerInfoSystem(trainTisParams);
            }
            break;

			case TA_Base_Core::FAILURE_SUMMARY_STEP:
			{
				if ( 1 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::FailureSummaryParameters summary;
				summary.failureCategory = static_cast<TA_Base_Core::EStepFailureCategory>(stringToUlong(mapParameters[nPos++]));

				refStepParameters.failureSummary(summary);
			}
			break;

			default:
				// throw an exception
				break;
		}
    }

    unsigned long PlanDataReadingClass::stringToUlong( const std::string& strSrc ) const
    {
        std::stringstream stmConverter;
        stmConverter << strSrc.c_str();

        unsigned long ulResult = 0;
        stmConverter >> ulResult;

        return ulResult;
    }
} // TA_Base_Bus
