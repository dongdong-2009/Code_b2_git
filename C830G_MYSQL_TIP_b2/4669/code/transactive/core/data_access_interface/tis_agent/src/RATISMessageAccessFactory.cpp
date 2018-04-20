/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * RATISMessageAccessFactory is a singleton that is used to generate and retrieve 
  * IRATISMessage objects.
  */

#ifdef _MSC_VER
  #pragma warning ( disable : 4786 )
#endif

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/RATISMessageData.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageTable.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{

	TA_Base_Core::ReEntrantThreadLockable RATISMessageAccessFactory::m_singletonLock;
	RATISMessageAccessFactory* RATISMessageAccessFactory::m_theClassInstance = NULL;

	RATISMessageAccessFactory& RATISMessageAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			
            m_theClassInstance = new RATISMessageAccessFactory();
		}
		
		return *m_theClassInstance;
	}

	std::vector<IRATISMessage*> RATISMessageAccessFactory::getRATISMessages()
    {
        return getRATISMessagesWhere();
    }


    std::vector<IRATISMessage*> RATISMessageAccessFactory::getVettingRATISMessages()
    {
        std::string whereSQL = " where " + TA_TISAgentDAI::REQUIRES_VETTING_COLUMN + " = 1 ";
        return getRATISMessagesWhere(whereSQL);
    }


    std::vector<IRATISMessage*> RATISMessageAccessFactory::getRATISMessagesWhere(std::string whereSQL /* = "" */)
    {
      TA_Base_Core::IDatabase* databaseConnection = 
          TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);

        // Get all the RATIS MESSAGE primary keys that exist in the table.

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;
        selectSql << " " << whereSQL;
        selectSql << " order by " << TA_TISAgentDAI::CREATION_DATE_COLUMN;

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN );

		TA_Base_Core::IData* data;

		try
		{
			data = databaseConnection->executeQuery( selectSql.str(), columnNames );
		}
		catch( const TA_Base_Core::DatabaseException& )
		{
			throw;
		}
        
        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no RATIS Messages in the database table ";
            error << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to IDisplayRequest.
        std::vector< IRATISMessage* > ratisMessages;

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        ratisMessages.push_back( new RATISMessageData( 
				        data->getUnsignedLongData( i, TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN ) ) );
		        }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return ratisMessages; 
    }

	IRATISMessage* RATISMessageAccessFactory::getRATISMessage( long id )
	{
		std::stringstream whereSQL;
        whereSQL << "where " << TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN
            << " = '" << id << "'";

		std::vector< IRATISMessage* > ratisMessages; // There should be no more than one

		try
		{
			ratisMessages = getRATISMessagesWhere(whereSQL.str());
		}
		catch ( const TA_Base_Core::DataException& )
		{
			std::stringstream error;
            error << "No RATIS messages exist in the database with the specified key";
            error << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
		}
		catch( const TA_Base_Core::DatabaseException& )
		{
			throw;
		}

		return ratisMessages[0];
	}



	IRATISMessage* RATISMessageAccessFactory::createNewRATISMessage()
	{

		return new TA_IRS_Core::RATISMessageData();
	}



} //end namespace TA_IRS_Core


