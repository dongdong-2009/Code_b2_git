/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_variable/src/InterlockVariableAccessFactory.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif


#include "core/data_access_interface/interlocking_variable/src/InterlockVariableAccessFactory.h"
#include "core/data_access_interface/interlocking_variable/src/InterlockVariable.h"
#include "core/data_access_interface/interlocking_variable/src/IInterlockVariable.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    InterlockVariableAccessFactory* InterlockVariableAccessFactory::m_instance = 0;
    TA_Base_Core::NonReEntrantThreadLockable InterlockVariableAccessFactory::m_singletonLock;

    InterlockVariableAccessFactory& InterlockVariableAccessFactory::getInstance()
    {
        ThreadGuard guard(m_singletonLock);

        if( m_instance == 0 )
        {
            m_instance = new InterlockVariableAccessFactory();
        }
        return *m_instance;
    }


    IInterlockVariable* InterlockVariableAccessFactory::getInterlockVariable(const std::string& key)
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the variable key
        // this is a check to ensure a variable with the specified
        // ENTITY_KEY actually exists.
        std::stringstream sql;
        sql << "select ENTITY_TYPE from LK_VARIABLE_TYPES where ENTITY_TYPE = '" << key << "'";	

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "ENTITY_TYPE";
        std::vector<std::string> columnNames;
        columnNames.push_back( keyColumn );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( sql.str(), columnNames );
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::string reasonMessage( "No data found for variable type = " );
            reasonMessage += key;		
            TA_THROW( DataException( reasonMessage.c_str(), DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::string reasonMessage( "More than one entry found for variable type = " );
			reasonMessage += key;			
            TA_THROW( DataException( reasonMessage.c_str(), DataException::NOT_UNIQUE, "" ));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the Operator object to represent this object. Initially only populate it
        // with key and name. We will populate the rest of the data on demand.
        IInterlockVariable* theInterlockVariable = new InterlockVariable(key);

        // Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theInterlockVariable;
    }

     
    std::vector<IInterlockVariable*> InterlockVariableAccessFactory::getInterlockVariables()
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the key of the rule
        // this is a check to ensure an rule with the specified
        // LKIRUL_ID actually exists.
        std::string sql( "select ENTITY_TYPE from LK_VARIABLE_TYPES" );	

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "ENTITY_TYPE";
        std::vector<std::string> columnNames;
        columnNames.push_back( keyColumn );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( sql.c_str(), columnNames );
        
        // create the vector of pointers to IInterlockVariable
        std::vector<IInterlockVariable*> variablePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {                
                std::string key = data->getStringData( i, keyColumn );
                variablePointerVector.push_back( new InterlockVariable( key ) );
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        // Return the pointer vector. The class that receives these pointers is responsible
        // for deleting them.
        return variablePointerVector;
    }

} // closes TA_Base_Core


