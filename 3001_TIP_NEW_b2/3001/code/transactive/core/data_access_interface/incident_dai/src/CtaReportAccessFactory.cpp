/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/CtaReportAccessFactory.cpp $
  * @author:  Wu Min Tao
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/12/24 15:15:18 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * The class provide the access matrix for the incident report access
  */

#include "core/data_access_interface/incident_dai/src/CtaReportAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/IncidentException.h"
#include <sstream>

using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Core
{
	
	//Constants for IN_REPORT_ACCESS table
	const std::string CtaReportAccessFactory::IN_REPORT_ACCESS     = "IN_REPORT_ACCESS";
	const std::string CtaReportAccessFactory::CREATE_SEPGRO_ID_COL = "CREATE_SEPGRO_ID";
	const std::string CtaReportAccessFactory::ACCESS_SEPGRO_ID_COL = "ACCESS_SEPGRO_ID";
	const std::string CtaReportAccessFactory::ACCESS_LEVEL_COL     = "ACCESS_LEVEL";

	CtaReportAccessFactory::CtaReportAccessFactory()
	{
		FUNCTION_ENTRY("Construtor");
		FUNCTION_EXIT;
	}
	
	CtaReportAccessFactory::~CtaReportAccessFactory()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}
	
	AccessMatrix CtaReportAccessFactory::getAccessMetrix(bool isIncident)
	{
		FUNCTION_ENTRY("getAccessMatrix");
		
		AccessMatrix matrix;
		
		//Get Connection to Database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Cd, TA_Base_Core::Write);
		
		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		// create the SQL string to retrieve the data 
		/*ostringstream sql;
		sql << "select " << CREATE_SEPGRO_ID_COL <<", " 
			<< ACCESS_SEPGRO_ID_COL <<", "
			<< ACCESS_LEVEL_COL
			<< " from "<< IN_REPORT_ACCESS
			<< " where IS_INCIDENT = " << isIncident
			<< " order by 2";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_REPORT_ACCESS_STD_SELECT_53001, isIncident);
//		std::string strSql  = databaseConnection->prepareSQLStatement(IN_REPORT_ACCESS_SELECT_53001, isIncident);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_REPORT_ACCESS_SELECT_53001, isIncident);
		
		// Set up the columnNames vector to be passed to executeQuery()
		vector<string> columnNames;
		columnNames.push_back(CREATE_SEPGRO_ID_COL);
		columnNames.push_back(ACCESS_SEPGRO_ID_COL);
		columnNames.push_back(ACCESS_LEVEL_COL);
		
		IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		//Bugzilla Bug 183: Mintao
		if (data == NULL)
		{
			//cannot connect to get the dat, throw IncidentException
			TA_THROW( IncidentException("Cannot connect to get the data", IncidentException::DATABASE_ERROR) );
		}
		
		ProfileAccess theMap;
		unsigned long prevAccessorId = 0xFFFF; // make this different from first accessor id in database
		
		do
		{
			int iTotalRows = data->getNumRows();
			
			unsigned long accessorId;
			unsigned long creatorId;
			int accessLevel;
			
			//Put the data into the map
			for(int count = 0; count < iTotalRows; ++count)
			{
				creatorId = data->getUnsignedLongData( count, CREATE_SEPGRO_ID_COL );    
				accessorId = data->getUnsignedLongData( count, ACCESS_SEPGRO_ID_COL );
				accessLevel = data->getIntegerData(count, ACCESS_LEVEL_COL);
				
				if (prevAccessorId != accessorId)
				{
					if (!theMap.empty())
					{
						//Set ProfileAccess 
						matrix[prevAccessorId] = theMap;
						theMap.clear();
					}
					prevAccessorId = accessorId;
				}
				
				theMap[creatorId] = (EAccessType)accessLevel;			 
			}
			
			// Now that we're done with the IData object, it's our responsibility to delete it
			delete data;
			data = NULL;
		}while( databaseConnection->moreData(data) );
		
		if (!theMap.empty())
		{
			matrix[prevAccessorId] = theMap;	
		} 
		
		FUNCTION_EXIT;
		return matrix;
	}
	
	// TD15203 - access matrix can be updated using the incident rights config plugin
	void CtaReportAccessFactory::updateAccessMatrix(const bool isIncident, const AccessMatrix& newMatrix)
	{
		FUNCTION_ENTRY("updateAccessMatrix");	
		
		AccessMatrix matrix = getAccessMetrix(isIncident);
		
		//Get Connection to Database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
		
		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		// create the SQL string to retrieve the data 
		//ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
		
		unsigned long newCreatorKey;
		unsigned long newAccessorKey;
		EAccessType newAccessLevel;
		
		unsigned long curCreatorKey;
		unsigned long curAccessorKey;
		EAccessType curAccessLevel;
		
		for (AccessMatrix::const_iterator newMatIter = newMatrix.begin(); newMatIter != newMatrix.end(); ++newMatIter )
		{
			newAccessorKey = newMatIter->first;		
			// search if the new accessing key in new matrix already exists
			AccessMatrix::iterator curMatIter = matrix.find(newAccessorKey);
			if(curMatIter != matrix.end())
			{	// if such accessing key already exists in the current matrix, 
				// check for any new or updated profile
				curAccessorKey  = curMatIter->first;
				ProfileAccess curCreatorProfile = curMatIter->second;			
				ProfileAccess newCreatorProfile = newMatIter->second;
				ProfileAccess::iterator newProfIter;			
				for (newProfIter = newCreatorProfile.begin(); newProfIter != newCreatorProfile.end(); ++newProfIter )
				{	
					newCreatorKey = newProfIter->first;
					newAccessLevel = (EAccessType) newProfIter->second;				
					// search if new creator profile already exists
					ProfileAccess::iterator curProfIter = curCreatorProfile.find(newCreatorKey);
					if (curProfIter != curCreatorProfile.end())
					{	// it exists, now check if the access level has changed	
						curCreatorKey = curProfIter->first;
						curAccessLevel = (EAccessType) curProfIter->second;
						if (  curAccessLevel != newAccessLevel )
						{	// need to update the profile in the database
							/*sql.str("");
							sql << "update " << IN_REPORT_ACCESS 
								<< " set " << ACCESS_LEVEL_COL << " = " << newAccessLevel
								<< " where " << ACCESS_SEPGRO_ID_COL << " = " << curAccessorKey
								<< " and " << CREATE_SEPGRO_ID_COL << " = " << curCreatorKey
								<< " and IS_INCIDENT = " << isIncident;*/
							//strSql = defPrepareSQLStatement(nDbType, IN_REPORT_ACCESS_STD_UPDATE_53151,newAccessLevel,curAccessorKey,curCreatorKey,isIncident);
//							strSql = databaseConnection->prepareSQLStatement(IN_REPORT_ACCESS_UPDATE_53151, newAccessLevel,curAccessorKey,curCreatorKey,isIncident);

		databaseConnection->prepareSQLStatement(strSql, IN_REPORT_ACCESS_UPDATE_53151, newAccessLevel,curAccessorKey,curCreatorKey,isIncident);
							databaseConnection->executeModification(strSql);	
							
						}
						// else there's no update needed					
					}
					else
					{	
						// only need to insert in the DB if an access control is set for
						// a profile group 
						if (newAccessLevel != NO_CONTROL)
						{	
							//sql.str("");
							// need to insert into database a new profile access map entry				 
							/*sql << "insert into " << IN_REPORT_ACCESS << " (INRACC_ID, " 
								<< CREATE_SEPGRO_ID_COL << ", "
								<< ACCESS_SEPGRO_ID_COL << ", "
								<< "IS_INCIDENT, "
								<< ACCESS_LEVEL_COL 
								<< ") values (INREPO_SEQ.NEXTVAL, "
								<< newCreatorKey << ", "
								<< newAccessorKey << ", "
								<< isIncident << ", "
								<< newAccessLevel <<") ";*/
							//strSql = defPrepareSQLStatement(nDbType, IN_REPORT_ACCESS_Oracle_INSERT_53351,newCreatorKey,newAccessorKey,isIncident,newAccessLevel);
//							strSql = databaseConnection->prepareSQLStatement(IN_REPORT_ACCESS_INSERT_53351, newCreatorKey,newAccessorKey,isIncident,newAccessLevel);

		databaseConnection->prepareSQLStatement(strSql, IN_REPORT_ACCESS_INSERT_53351,
            databaseConnection->getNextPkey("INREPO_SEQ"), newCreatorKey,newAccessorKey,isIncident,newAccessLevel);
							// send insert/update modification into the database
							databaseConnection->executeModification(strSql);					 
						}
					}
				}		
			}
			else
			{ 
				// the accessing key does not exist, must add all the profile access maps
				// for this new accessing key to the database
				ProfileAccess newCreatorProfile = newMatIter->second;
				ProfileAccess::iterator newProfIter;						
				for (newProfIter = newCreatorProfile.begin(); newProfIter != newCreatorProfile.end(); ++newProfIter )
				{
					newCreatorKey = newProfIter->first;
					newAccessLevel = (EAccessType) newProfIter->second;
					// only need to insert in the DB if an Access control is set for
					// a profile group 
					if (newAccessLevel != NO_CONTROL)
					{	// need to insert into database a new profile access map entry
						//sql.str("");
						// need to insert into database a new profile access map entry				 
						/*sql << "insert into " << IN_REPORT_ACCESS << " (INRACC_ID, " 
							<< CREATE_SEPGRO_ID_COL << ", "
							<< ACCESS_SEPGRO_ID_COL << ", "
							<< "IS_INCIDENT, "
							<< ACCESS_LEVEL_COL 
							<< ") values (INREPO_SEQ.NEXTVAL, "
							<< newCreatorKey << ", "
							<< newAccessorKey << ", "
							<< isIncident << ", "
							<< newAccessLevel <<") ";*/
						//strSql = defPrepareSQLStatement(nDbType, IN_REPORT_ACCESS_Oracle_INSERT_53351,newCreatorKey,newAccessorKey,isIncident,newAccessLevel);
//						strSql = databaseConnection->prepareSQLStatement(IN_REPORT_ACCESS_INSERT_53351, newCreatorKey,newAccessorKey,isIncident,newAccessLevel);

		databaseConnection->prepareSQLStatement(strSql, IN_REPORT_ACCESS_INSERT_53351,
                        databaseConnection->getNextPkey("INREPO_SEQ"), newCreatorKey,newAccessorKey,isIncident,newAccessLevel);
						// send insert/update modification into the database
						databaseConnection->executeModification(strSql);
						
					}		 
				}
			}
		}
		FUNCTION_EXIT;
	}

} // end TA_IRS_Core
