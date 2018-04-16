/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * CtaLocationAccessFactory is a singleton that is used to retrieve Location objects from the
  * database. 
  */
#include <sstream>

#include "core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/IncidentException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

namespace TA_IRS_Core
{
	CtaLocationAccessFactory::CtaLocationAccessFactory()
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	
	
	CtaLocationAccessFactory::~CtaLocationAccessFactory()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}
	
	std::vector<std::string *> CtaLocationAccessFactory::getAllLocations()
	{
		FUNCTION_ENTRY("getAllLocations");
		
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);
		
		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot connect to database to access LOCATION table";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
		}
		
		// create the SQL string to retrieve the data of the IncidentReportData
		std::ostringstream sql;
		sql << "select NAME from LOCATION order by ORDER_ID";
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back("NAME");
		
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
		
		//Bugzilla Bug 153: Mintao
		//check for null data pointer
		if ( NULL == data)
		{
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot get the required data in CtaLocationAccessFactory::getAllLocations.";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
			
		}
		
		std::vector<std::string *> toReturn;
		try
		{
			do
			{
				for (unsigned long i = 0; i < data->getNumRows() ; ++i )
				{   
					
					std::string location = data->getStringData(i,"NAME");
					if (!location.empty())
					{            
						toReturn.push_back(new std::string(location));
					}
				}
				
				delete data;
				data = NULL;
			}while( databaseConnection->moreData(data) );
		}
		catch (...)
		{
			delete data;
			data = NULL;
		}
		
		//Bugzilla Bug 153: Mintao
		//delete the data pointer after used
		delete data;
		data = NULL;
		
		FUNCTION_EXIT;
		return toReturn;;
	}
} // end TA_IRS_Core