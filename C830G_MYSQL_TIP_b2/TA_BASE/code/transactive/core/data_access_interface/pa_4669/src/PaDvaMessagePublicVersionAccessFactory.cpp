/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersionAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessagePublicVersionAccessFactory is a singleton that is used to retrieve PaDvaMessagePublicVersion objects either from the
  * database or newly created. All PaDvaMessagePublicVersion objects returned will adhere to the IPaDvaMessagePublicVersion interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersionAccessFactory.h"

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersion.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePublicVersion.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaDvaMessagePublicVersionAccessFactory* PaDvaMessagePublicVersionAccessFactory::m_instance = 0;

    static const std::string KEY_COL = "PADMPUVE_ID";
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_PUBLIC_VERSION";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    PaDvaMessagePublicVersionAccessFactory& PaDvaMessagePublicVersionAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessagePublicVersionAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessagePublicVersionAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaDvaMessagePublicVersion* PaDvaMessagePublicVersionAccessFactory::getPaDvaMessagePublicVersionByLocationKey(const unsigned long locationKey,const bool readWrite)
    {
        return getPaDvaMessagePublicVersionUsing(LOCATIONKEY_COL, locationKey, readWrite);
    }


    IPaDvaMessagePublicVersion* PaDvaMessagePublicVersionAccessFactory::getPaDvaMessagePublicVersion(const unsigned long key,const bool readWrite)
    {
        return getPaDvaMessagePublicVersionUsing(KEY_COL, key, readWrite);
    }


    IPaDvaMessagePublicVersion* PaDvaMessagePublicVersionAccessFactory::getPaDvaMessagePublicVersionUsing(const std::string& colName, 
                                                                                            unsigned long colValue,
                                                                                            bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessagePublicVersion
        // this is a check to ensure an PaDvaMessagePublicVersion with the specified
        // PATDME_ID actually exists.        
        std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME
            << " where " << colName.c_str() << " = " << colValue;        

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
			sprintf(reasonMessage, 
                            "No data found for item where %s = %lu", 
                            colName.c_str(), 
                            colValue);			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, 
                            "No data found for item where %s = %lu", 
                            colName.c_str(), 
                            colValue);			
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        unsigned long key = data->getUnsignedLongData(0, KEY_COL);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaDvaMessagePublicVersion object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaDvaMessagePublicVersion* thePaDvaMessagePublicVersion;
        
        // decide which PaDvaMessagePublicVersion object to return
        if (readWrite) // need a config PaDvaMessagePublicVersion
        {
            thePaDvaMessagePublicVersion = new ConfigPaDvaMessagePublicVersion(key);
        }
        else // need a standard PaDvaMessagePublicVersion
        {
            thePaDvaMessagePublicVersion = new PaDvaMessagePublicVersion(key);
        }

        // Return the PaDvaMessagePublicVersion pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaDvaMessagePublicVersion;
    }


    IPaDvaMessagePublicVersions PaDvaMessagePublicVersionAccessFactory::getAllPaDvaMessagePublicVersions( const bool readWrite)
    {
        return getPaDvaMessagePublicVersionsReturnedFromStatement("", readWrite);
    }


    IPaDvaMessagePublicVersions PaDvaMessagePublicVersionAccessFactory::getPaDvaMessagePublicVersionsReturnedFromStatement(
        const std::string& whereClause, 
        const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaDvaMessagePublicVersions, ConfigPaDvaMessagePublicVersion, PaDvaMessagePublicVersion>
                        (PaDvaMessagePublicVersionHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaDvaMessagePublicVersion* PaDvaMessagePublicVersionAccessFactory::copyPaDvaMessagePublicVersion(const IConfigPaDvaMessagePublicVersion* paDvaMessagePublicVersionToCopy)
    {
        FUNCTION_ENTRY("copyPaDvaMessagePublicVersion");

        TA_ASSERT(paDvaMessagePublicVersionToCopy !=  NULL, "The PaDvaMessagePublicVersion to copy was NULL");
        
        const ConfigPaDvaMessagePublicVersion* cast = dynamic_cast<const ConfigPaDvaMessagePublicVersion*>(paDvaMessagePublicVersionToCopy);

        TA_ASSERT(cast != NULL, "PaDvaMessagePublicVersion passed could not be converted into a ConfigPaDvaMessagePublicVersion");

        FUNCTION_EXIT;
        return new ConfigPaDvaMessagePublicVersion(*cast);

    }

    IConfigPaDvaMessagePublicVersion* PaDvaMessagePublicVersionAccessFactory::createPaDvaMessagePublicVersion()
    {            
        return new ConfigPaDvaMessagePublicVersion();    
    }

} // closes TA_Base_Core


