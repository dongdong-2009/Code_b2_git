/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessagePrivateVersionAccessFactory is a singleton that is used to retrieve PaDvaMessagePrivateVersion objects either from the
  * database or newly created. All PaDvaMessagePrivateVersion objects returned will adhere to the IPaDvaMessagePrivateVersion interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionAccessFactory.h"

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersion.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePrivateVersion.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaDvaMessagePrivateVersionAccessFactory* PaDvaMessagePrivateVersionAccessFactory::m_instance = 0;

    static const std::string KEY_COL = "PADMPRVE_ID";
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_PRIVATE_VERSION";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    PaDvaMessagePrivateVersionAccessFactory& PaDvaMessagePrivateVersionAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessagePrivateVersionAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessagePrivateVersionAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaDvaMessagePrivateVersion* PaDvaMessagePrivateVersionAccessFactory::getPaDvaMessagePrivateVersionByLocationKey(const unsigned long locationKey,const bool readWrite)
    {
        return getPaDvaMessagePrivateVersionUsing(LOCATIONKEY_COL, locationKey, readWrite);
    }


    IPaDvaMessagePrivateVersion* PaDvaMessagePrivateVersionAccessFactory::getPaDvaMessagePrivateVersion(const unsigned long key,const bool readWrite)
    {
        return getPaDvaMessagePrivateVersionUsing(KEY_COL, key, readWrite);
    }


    IPaDvaMessagePrivateVersion* PaDvaMessagePrivateVersionAccessFactory::getPaDvaMessagePrivateVersionUsing(const std::string& colName, 
                                                                                            unsigned long colValue,
                                                                                            bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessagePrivateVersion
        // this is a check to ensure an PaDvaMessagePrivateVersion with the specified
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

        // Create the PaDvaMessagePrivateVersion object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaDvaMessagePrivateVersion* thePaDvaMessagePrivateVersion;
        
        // decide which PaDvaMessagePrivateVersion object to return
        if (readWrite) // need a config PaDvaMessagePrivateVersion
        {
            thePaDvaMessagePrivateVersion = new ConfigPaDvaMessagePrivateVersion(key);
        }
        else // need a standard PaDvaMessagePrivateVersion
        {
            thePaDvaMessagePrivateVersion = new PaDvaMessagePrivateVersion(key);
        }

        // Return the PaDvaMessagePrivateVersion pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaDvaMessagePrivateVersion;
    }


    IPaDvaMessagePrivateVersions PaDvaMessagePrivateVersionAccessFactory::getAllPaDvaMessagePrivateVersions( const bool readWrite)
    {
        return getPaDvaMessagePrivateVersionsReturnedFromStatement("", readWrite);
    }


    IPaDvaMessagePrivateVersions PaDvaMessagePrivateVersionAccessFactory::getPaDvaMessagePrivateVersionsReturnedFromStatement(
        const std::string& whereClause, 
        const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaDvaMessagePrivateVersions, ConfigPaDvaMessagePrivateVersion, PaDvaMessagePrivateVersion>
                        (PaDvaMessagePrivateVersionHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaDvaMessagePrivateVersion* PaDvaMessagePrivateVersionAccessFactory::copyPaDvaMessagePrivateVersion(const IConfigPaDvaMessagePrivateVersion* paDvaMessagePrivateVersionToCopy)
    {
        FUNCTION_ENTRY("copyPaDvaMessagePrivateVersion");

        TA_ASSERT(paDvaMessagePrivateVersionToCopy !=  NULL, "The PaDvaMessagePrivateVersion to copy was NULL");
        
        const ConfigPaDvaMessagePrivateVersion* cast = dynamic_cast<const ConfigPaDvaMessagePrivateVersion*>(paDvaMessagePrivateVersionToCopy);

        TA_ASSERT(cast != NULL, "PaDvaMessagePrivateVersion passed could not be converted into a ConfigPaDvaMessagePrivateVersion");

        FUNCTION_EXIT;
        return new ConfigPaDvaMessagePrivateVersion(*cast);

    }

    IConfigPaDvaMessagePrivateVersion* PaDvaMessagePrivateVersionAccessFactory::createPaDvaMessagePrivateVersion()
    {            
        return new ConfigPaDvaMessagePrivateVersion();    
    }

} // closes TA_Base_Core


