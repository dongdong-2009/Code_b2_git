/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TIRatisTagAccessFactory.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisTagAccessFactory is a singleton that is used to retrieve TiRatisTag objects either from the
  * database or newly created. All TiRatisTag objects returned will adhere to the ITiRatisTag interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/stis_manager/src/TiRatisTagAccessFactory.h"

#include "core/data_access_interface/stis_manager/src/TiRatisTag.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/stis_manager/src/ConfigTiRatisTag.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    TiRatisTagAccessFactory* TiRatisTagAccessFactory::m_instance = 0;

    static const std::string KEY_COL			= "TIRTAG_ID";

    static const std::string TI_RATIS_TAG_TABLE = "TI_RATIS_TAG";

    TiRatisTagAccessFactory& TiRatisTagAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new TiRatisTagAccessFactory();
        }
        return *m_instance;
    }


    void TiRatisTagAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    ITiRatisTag* TiRatisTagAccessFactory::getTiRatisTag(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the TiRatisTag
        // this is a check to ensure an TiRatisTag with the specified
        // TIRTAG_ID actually exists.        
        /*std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << TI_RATIS_TAG_TABLE
            << " where " << KEY_COL << " = " << key;*/
		/*TI_RATIS_TAG_SELECT_56001=select TIRTAG_ID from ti_ratis_tag where TIRTAG_ID = %s*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_RATIS_TAG_SELECT_56001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
        
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

        // Create the TiRatisTag object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        ITiRatisTag* theTiRatisTag;
        
        // decide which TiRatisTag object to return
        if (readWrite) // need a config TiRatisTag
        {
            theTiRatisTag = new ConfigTiRatisTag(key);
        }
        else // need a standard TiRatisTag
        {
            theTiRatisTag = new TiRatisTag(key);
        }

        // Return the TiRatisTag pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theTiRatisTag;
    }


    ITiRatisTags TiRatisTagAccessFactory::getAllTiRatisTags(const bool readWrite )
    {

		//std::ostringstream sql;		

        return getTiRatisTagsReturnedFromStatement("", readWrite);
		//return getTiRatisTagsReturnedFromStatement(" ", readWrite);
    }


    ITiRatisTags TiRatisTagAccessFactory::getTiRatisTagsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement <ITiRatisTags, ConfigTiRatisTag, TiRatisTag> 
                    (TiRatisTagHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigTiRatisTag* TiRatisTagAccessFactory::createTiRatisTag()
    {            
        return new ConfigTiRatisTag();    
    }


    IConfigTiRatisTag* TiRatisTagAccessFactory::copyTiRatisTag(const IConfigTiRatisTag* TiRatisTagToCopy)
    {
        FUNCTION_ENTRY("copyTiRatisTag");

        TA_ASSERT(TiRatisTagToCopy !=  NULL, "The TiRatisTag to copy was NULL");
        
        const ConfigTiRatisTag* cast = dynamic_cast<const ConfigTiRatisTag*>(TiRatisTagToCopy);

        TA_ASSERT(cast != NULL, "TiRatisTag passed could not be converted into a ConfigTiRatisTag");

        FUNCTION_EXIT;
        return new ConfigTiRatisTag(*cast);

    }

} // closes TA_IRS_Core


