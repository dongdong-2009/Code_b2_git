/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.cpp $
  * @author:  Ross Tucker
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


#include "core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.h"
#include "core/data_access_interface/interlocking/src/IInterlockRule.h"
#include "core/data_access_interface/interlocking/src/IConfigInterlockRule.h"

#include "core/data_access_interface/interlocking/src/InterlockRule.h"
#include "core/data_access_interface/interlocking/src/InterlockAlias.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/interlocking/src/ConfigInterlockRule.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    InterlockRuleAccessFactory* InterlockRuleAccessFactory::m_instance = 0;
    TA_Base_Core::NonReEntrantThreadLockable InterlockRuleAccessFactory::m_singletonLock;

    InterlockRuleAccessFactory& InterlockRuleAccessFactory::getInstance()
    {
        ThreadGuard guard(m_singletonLock);

        if( m_instance == 0 )
        {
            m_instance = new InterlockRuleAccessFactory();
        }
        return *m_instance;
    }


    IInterlockRule* InterlockRuleAccessFactory::getInterlockRule(const unsigned long key, const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the key of the rule
        // this is a check to ensure an rule with the specified
        // LKIRUL_ID actually exists.
        std::stringstream sql;
		sql << "select LKIRUL_ID from LK_INTERLOCKING_RULES where LKIRUL_ID = " << key;	

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "LKIRUL_ID";
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

            std::stringstream reasonMessage;
			reasonMessage << "No data found for pkey = " << key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::stringstream reasonMessage;
			reasonMessage << "More than one entry found for pkey = " << key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "" ) );
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the Operator object to represent this object. Initially only populate it
        // with key and name. We will populate the rest of the data on demand.
        IInterlockRule* theInterlockRule = NULL;
        
        // decide which operator object to return
        if (readWrite) // need a config operator
        {
            theInterlockRule = new ConfigInterlockRule(key);
        }
        else // need a standard operator
        {
            theInterlockRule = new InterlockRule(key);
        }

        // Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theInterlockRule;
    }


    std::vector<IInterlockRule*> InterlockRuleAccessFactory::getInterlockRules( const unsigned long entity, const std::string& entityType, const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
        sql << "select LKIRUL_ID from LK_INTERLOCKING_RULES ";
        sql << "where  ENTITY_KEY = " << entity;
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "LKIRUL_ID";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
        // create the vector of pointers to IEntityData
        std::vector<IInterlockRule*> rulePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                unsigned long key = 0;
                try
                {
                    key = data->getUnsignedLongData(i,keyColumn);
                }
                catch(...)
                {
                    delete data;
                    data = NULL;
                    throw;
                }
                if (readWrite) // need a config operator
                {
                    rulePointerVector.push_back( new ConfigInterlockRule(key));
                }
                else // need a standard operator
                {
                    rulePointerVector.push_back( new InterlockRule(key) );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return rulePointerVector;
    }
    
    
    std::vector<IInterlockRule*> InterlockRuleAccessFactory::getAllInterlockRules( const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the guis
        std::string sql("select LKIRUL_ID from LK_INTERLOCKING_RULES"); 

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "LKIRUL_ID";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // create the vector of pointers to IEntityData
        std::vector<IInterlockRule*> rulePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {       
                unsigned long key = 0;
                try
                {
                    key = data->getUnsignedLongData(i,keyColumn);
                }
                catch(...)
                {
                    delete data;
                    data = NULL;
                    throw;
                }
                if (readWrite) // need a config operator
                {
                    rulePointerVector.push_back( new ConfigInterlockRule(key));
                }
                else // need a standard operator
                {
                    rulePointerVector.push_back( new InterlockRule(key) );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return rulePointerVector;
    }
    
    
    IInterlockRule* InterlockRuleAccessFactory::createInterlockRule()
    {            
        return new ConfigInterlockRule();    
    }


    IInterlockRule* InterlockRuleAccessFactory::copyInterlockRule(const IConfigInterlockRule* ruleToCopy)
    {
        TA_ASSERT(ruleToCopy !=  NULL, "The rule to copy was NULL");
        
        const ConfigInterlockRule* cast = dynamic_cast<const ConfigInterlockRule*>(ruleToCopy);

        TA_ASSERT(cast != NULL, "Interlock Rule passed could not be converted into a ConfigInterlockRule");

        return new ConfigInterlockRule(*cast);
    }



} // closes TA_Base_Core


