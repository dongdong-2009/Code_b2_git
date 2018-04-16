/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/OperatorAccessFactory.cpp $
  * @author Nick Jardine
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2010/06/25 12:06:02 $
  * Last modified by:  $Author: builder $
  * 
  * OperatorAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IOperator interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif


#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "core/data_access_interface/src/Operator.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ConfigOperator.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
	OperatorAccessFactory::OperatorAccessFactory()
	{
		initializeOperatorProfile();
		initializeOperatorRegion();
	}

    OperatorAccessFactory* OperatorAccessFactory::m_instance = 0;

    OperatorAccessFactory& OperatorAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new OperatorAccessFactory();
        }

        return *m_instance;
    }


	void OperatorAccessFactory::initializeOperatorProfile()
	{
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		
        std::ostringstream sql;
        sql << "select OPERATORKEY, SEPROF_ID from SE_OPERATOR_PROFILE";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("OPERATORKEY");
        columnNames.push_back("SEPROF_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        do
        {
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
					unsigned long profileId  = data->getUnsignedLongData(i, columnNames[1]);

					m_operatorToProfileMap[operatorId].push_back(profileId);
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
	}

	void OperatorAccessFactory::initializeOperatorRegion()
	{
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		
        std::ostringstream sql;
        sql << "select OPERATORKEY, SEREGI_ID from SE_OPERATOR_REGIONS";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("OPERATORKEY");
        columnNames.push_back("SEREGI_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        do
        {
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
					unsigned long regionId  = data->getUnsignedLongData(i, columnNames[1]);

					m_operatorToRegionMap[operatorId].push_back(regionId);
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
	}


    void OperatorAccessFactory::getProfileByOperator(unsigned long operatorId, std::vector<unsigned long>& profiles)
    {
		FUNCTION_ENTRY("OperatorAccessFactory::getProfileByOperator");

		OperatorMap::iterator it = m_operatorToProfileMap.find( operatorId );
		if ( it != m_operatorToProfileMap.end() )
		{
            profiles = it->second;
		}
		FUNCTION_EXIT;
    }
	

    void OperatorAccessFactory::getRegionByOperator(unsigned long operatorId, std::vector<unsigned long>& regions)
    {
		FUNCTION_ENTRY("OperatorAccessFactory::getRegionByOperator");

		OperatorMap::iterator it = m_operatorToRegionMap.find( operatorId );
		if ( it != m_operatorToRegionMap.end() )
		{
            regions = it->second;
		}
		FUNCTION_EXIT;
    }


    void OperatorAccessFactory::getOperatorBySql( 
		const std::string& sql,
		std::vector<IOperator*>& operators,
		const bool readWrite,
		const bool getCfgProfiles,
		const bool getCfgRegions,
		const bool getProfiles,
		const bool getRegions)
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PASSWD");
		columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Bohong++
        std::vector<unsigned long> operatorIds;
        std::vector<IData *>       iDatas;		

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
                operatorIds.push_back(operatorId);
            }

            //delete data;
            iDatas.push_back(data);
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        // Bohong++, we have to get out all the operatorId first, then new Operator one by one.
        // Cos the DatabaseFactory is a singleton, we should not do any db operation until previous
        // operation is finished
        std::vector<IData *>::iterator it = iDatas.begin();
        for( int j = 0; it != iDatas.end(); ++it, ++j )
        {
            for (unsigned long i = 0; i < (*it)->getNumRows() ; ++i )
            {
                std::vector<unsigned long> profiles;
                getProfileByOperator(operatorIds[i+(j*FETCH_MAX_ROWS)], profiles);

                std::vector<unsigned long> regions;
                getRegionByOperator(operatorIds[i+(j*FETCH_MAX_ROWS)], regions);

                if (readWrite) // need a config operator
                {
	                operators.push_back( new ConfigOperator(i, *(*it), profiles, regions) );
                }
                else // need a standard operator
                {
	                operators.push_back( new Operator(i, *(*it), profiles, regions) );
                }
            }

            delete *it;
        }
    }


	void OperatorAccessFactory::getOperatorInfoBySql( 
		const std::string& sql,
		std::vector<IOperator*>& operators )
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PASSWD");
		columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
		
         
        std::vector<unsigned long> operatorIds;
        std::vector<IData *>       iDatas;		
		
        do
        {
			std::auto_ptr<IData> datas( data );
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                unsigned long operatorId = data->getUnsignedLongData(i, columnNames[0]);
				std::string  operatorName = data->getStringData(i, columnNames[1]);                
				bool is_system=data->getBooleanData(i, columnNames[4]);           
				operators.push_back( new Operator(operatorId,operatorName,is_system ) );
            }          
            data = NULL;
        }
        while( databaseConnection->moreData(data) );
		
       
         
    }


    IOperator* OperatorAccessFactory::getOperator(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getOperator");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;
        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where PKEY = " << key;

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return operators[0];
    }

	//TD15069++
	std::string OperatorAccessFactory::getDeletedOperatorName(const unsigned long key)
    {
		FUNCTION_ENTRY("getDeletedOperatorName");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where DELETED <> 0 and PKEY = " << key;

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, false);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }
		
		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

		IOperator* OperatorAccess = operators[0];
		std::string name;
		try
		{
			name = OperatorAccess->getName();
			delete OperatorAccess;
			OperatorAccess = NULL;
		}
        catch(...)
        {
            delete OperatorAccess;
            OperatorAccess = NULL;
            throw;
        }

		FUNCTION_EXIT;
        return name;
    }
	//++TD15069

	//TD15069++
	bool OperatorAccessFactory::getDeletedOperatorIsSystemOperator(const unsigned long key)
    {
		FUNCTION_ENTRY("getDeletedOperatorIsSystemOperator");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR where DELETED <> 0 and PKEY = " << key;

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, false);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == operators.size(), "Operator key unique constraint violated");

		IOperator* OperatorAccess = operators[0];
        bool isSystem = false;
		try
		{
			isSystem = OperatorAccess->isSystemOperator();
			delete OperatorAccess;
			OperatorAccess = NULL;
		}
        catch(...)
        {
            delete OperatorAccess;
            OperatorAccess = NULL;
            throw;
        }

		FUNCTION_EXIT;
        return isSystem;
     }
	//++TD15069

    IOperator* OperatorAccessFactory::getOperator(const std::string& name,const bool readWrite)
    {
		FUNCTION_ENTRY("getOperator");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the operator
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
		std::ostringstream sql;
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR_V where NAME = '"
			<< databaseConnection->escapeQueryString(name) << "'";

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with name " << name;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == operators.size(), "Operator name unique constraint violated");

        // Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
        return operators[0];
    }

    IOperator* OperatorAccessFactory::getOperatorFromSession(const std::string& sessionId, const bool readWrite)
    {
		FUNCTION_ENTRY("getOperatorFromSession");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		std::ostringstream sql;
		sql << " select a.PKEY, a.NAME, a.DESCRIPTION, a.PASSWD, a.IS_SYSTEM,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from OPERATOR a, TA_SESSION b where a.PKEY = b.OPERATORKEY and b.PKEY = '"
			<< databaseConnection->escapeQueryString(sessionId) << "'";

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite);

        if ( 0 == operators.size())
        {
            std::ostringstream message;
			message << "No data found for Operator with SessionId " << sessionId;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }
		
		TA_ASSERT(1 == operators.size(), "Operator SessionKey unique constraint violated");

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators[0];
    }

    std::vector<IOperator*> OperatorAccessFactory::getAllOperators( const bool readWrite )
    {
		FUNCTION_ENTRY("getAllOperators");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;
        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V"; 

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite, true, false, false, false);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


	std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsInfo(   )
    {
		FUNCTION_ENTRY("getAllOperators");
		
        // create the SQL string to retrieve the guis
		std::ostringstream sql;
        
		sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V"; 
		
		std::vector<IOperator*> operators;
		getOperatorInfoBySql(sql.str(), operators);
		
		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsAndRegions( const bool readWrite )
    {
		FUNCTION_ENTRY("getAllOperatorsAndRegions");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V"; 

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite, true, false, false, true);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    std::vector<IOperator*> OperatorAccessFactory::getAllOperatorsAndAttributes()
    {
        return getAllOperators(false);
    }
    
    
    std::vector<IOperator*> OperatorAccessFactory::getAllNonSystemOperators( const bool readWrite, 
                                                                             const bool getProfiles, /*=true*/
                                                                             const bool getRegions /*=false*/ )
    {
		FUNCTION_ENTRY("getAllNonSystemOperators");

        // create the SQL string to retrieve the guis
		std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, PASSWD, IS_SYSTEM,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from OPERATOR_V where IS_SYSTEM = 0"; 		

		std::vector<IOperator*> operators;
		getOperatorBySql(sql.str(), operators, readWrite, getProfiles, false, getProfiles, getRegions);

		// Return the operator pointer. The class that recieves this pointer is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return operators;
    }


    IConfigOperator* OperatorAccessFactory::createOperator()
    {            
        return new ConfigOperator();    
    }


    IConfigOperator* OperatorAccessFactory::copyOperator(const IConfigOperator* operatorToCopy)
    {
        FUNCTION_ENTRY("copyOperator");

        TA_ASSERT(operatorToCopy !=  NULL, "The operator to copy was NULL");
        
        const ConfigOperator* cast = dynamic_cast<const ConfigOperator*>(operatorToCopy);

        TA_ASSERT(cast != NULL, "Operator passed could not be converted into a ConfigOperator");

        FUNCTION_EXIT;
        return new ConfigOperator(*cast);
    }


} // closes TA_Base_Core


