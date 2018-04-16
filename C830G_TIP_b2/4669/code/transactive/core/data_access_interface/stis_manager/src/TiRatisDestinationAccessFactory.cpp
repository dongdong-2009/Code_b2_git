/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/TiRatisDestinationAccessFactory.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisDestinationAccessFactory is a singleton that is used to retrieve TiRatisDestination objects either from the
  * database or newly created. All TiRatisDestination objects returned will adhere to the ITiRatisDestination interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/stis_manager/src/TiRatisDestinationAccessFactory.h"

#include "core/data_access_interface/stis_manager/src/TiRatisDestination.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/stis_manager/src/ConfigTiRatisDestination.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    TiRatisDestinationAccessFactory* TiRatisDestinationAccessFactory::m_instance = 0;

    static const std::string TI_RATIS_DESTINATION_TABLE = "TI_RATIS_DEST";

    static const std::string KEY_COL			= "TIRDES_ID";

    static const std::string DESTINATION_COL    = "DESTINATION";


    TiRatisDestinationAccessFactory& TiRatisDestinationAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new TiRatisDestinationAccessFactory();
        }
        return *m_instance;
    }


    void TiRatisDestinationAccessFactory::removeInstance( )
    {
        if ( m_instance != NULL )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }


    ITiRatisDestination* TiRatisDestinationAccessFactory::getTiRatisDestination(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the TiRatisDestination
        // this is a check to ensure an TiRatisDestination with the specified
        // TIRTAG_ID actually exists.        
        std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << TI_RATIS_DESTINATION_TABLE
            << " where " << KEY_COL << " = " << key;        

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "No data found for pkey = %lu",key);         
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);         
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the TiRatisDestination object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        ITiRatisDestination* theTiRatisDestination;
        
        // decide which TiRatisDestination object to return
        if (readWrite) // need a config TiRatisDestination
        {
            theTiRatisDestination = new ConfigTiRatisDestination(key);
        }
        else // need a standard TiRatisDestination
        {
            theTiRatisDestination = new TiRatisDestination(key);
        }

        // Return the TiRatisDestination pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theTiRatisDestination;
    }


    void TiRatisDestinationAccessFactory::setDestination(std::string newDestination)
    {
        std::ostringstream tableDestination;
        tableDestination << "TI_RATIS_DEST";  
      
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);

        // create the SQL string to retrieve the name of the PaTrainDvaMessage
        // this is a check to ensure an PaTrainDvaMessage with the specified
        // PATDME_ID actually exists.        
        std::ostringstream sql;
        sql << "comment on table " 
            << TI_RATIS_DESTINATION_TABLE << " is '"
            << tableDestination.str() << "'";

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(sql.str());
    }


    ITiRatisDestinations TiRatisDestinationAccessFactory::getAllTiRatisDestinations(const bool readWrite )
    {

        //std::ostringstream sql;       

        return getTiRatisDestinationsReturnedFromStatement("", readWrite);
        //return getTiRatisDestinationsReturnedFromStatement(" ", readWrite);
    }


    ITiRatisDestinations TiRatisDestinationAccessFactory::getTiRatisDestinationsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement <ITiRatisDestinations, ConfigTiRatisDestination, TiRatisDestination> 
                    (TiRatisDestinationHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigTiRatisDestination* TiRatisDestinationAccessFactory::createTiRatisDestination()
    {            
        return new ConfigTiRatisDestination();    
    }


    IConfigTiRatisDestination* TiRatisDestinationAccessFactory::copyTiRatisDestination(const IConfigTiRatisDestination* TiRatisDestinationToCopy)
    {
        FUNCTION_ENTRY("copyTiRatisDestination");

        TA_ASSERT(TiRatisDestinationToCopy !=  NULL, "The TiRatisDestination to copy was NULL");
        
        const ConfigTiRatisDestination* cast = dynamic_cast<const ConfigTiRatisDestination*>(TiRatisDestinationToCopy);

        TA_ASSERT(cast != NULL, "TiRatisDestination passed could not be converted into a ConfigTiRatisDestination");

        FUNCTION_EXIT;
        return new ConfigTiRatisDestination(*cast);

    }

} // closes TA_IRS_Core


