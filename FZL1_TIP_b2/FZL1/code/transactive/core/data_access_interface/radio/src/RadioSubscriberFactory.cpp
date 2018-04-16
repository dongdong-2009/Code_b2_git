/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriberFactory.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: Changed the const varibles difinition
  *
  * Implementation of the Class RadioSubscriberFactory
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/SQLCode.h"
#include <algorithm>

#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/RadioSubscriber.h"
#include "core/data_access_interface/radio/src/ConfigRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioSubscriberHelper.h"

namespace
{
    //
    // Constants
    //
     
    // Table name
    static const std::string RASUBSCRIBER_TBL   = "ra_subscriber";

    // Column names in RA_SUBSCRIBER
    static const std::string PKEY_COL           = "PKEY";
    static const std::string RESOURCETYPE_COL   = "RESOURCE_TYPE";
    static const std::string RESOURCEID_COL     = "RESOURCE_ID";
    static const std::string SSI_COL            = "SSI";
    static const std::string ALIAS_COL          = "ALIAS";
    static const std::string DATE_CREATED_COL   = "DATE_CREATED";   //TD17157
    static const std::string DATE_MODIFIED_COL  = "DATE_MODIFIED"; //TD17157

}

using namespace TA_Base_Core;

namespace TA_IRS_Core
{


    RadioSubscriberFactory* RadioSubscriberFactory::m_instance = NULL;


    RadioSubscriberFactory::RadioSubscriberFactory()
    {
    }

    RadioSubscriberFactory::~RadioSubscriberFactory()
    {
    }

    RadioSubscriberFactory& RadioSubscriberFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if(NULL == m_instance)
        {
            m_instance = new RadioSubscriberFactory();
        }
        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioSubscriberFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    IRadioSubscriber* RadioSubscriberFactory::getRadioSubscriber(const unsigned long key, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getRadioSubscriber");
    
        TA_ASSERT(key > 0, "Invalid Radio Subscriber Id index.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the name of the radio subscriber
        // this is a check to ensure an radio subscriber with the specified key actually exists.        
        /*std::ostringstream sql;
        sql << "select " << PKEY_COL 
            << " from "  << RASUBSCRIBER_TBL
            << " where " << PKEY_COL << " = " << key;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_SUBSCRIBER_SELECT_70001, key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_SELECT_70001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);	

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        if (0 == data->getNumRows())
        {
            // radio subscriber record with the specified primary key not found
            std::ostringstream reason;
		    reason << "No record found for Radio Subscriber with key = " << key;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Invalid Radio Subscriber primary key." ) );
        }

        // Don't need to do anything with the dataa, only needed to check if it exists
        TA_ASSERT(1 == data->getNumRows(), "Invalid number of records.");        

        // Create the radio subscriber object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioSubscriber* theSubscriber = NULL;
        
        // decide the type of radio subscriber object to return based on the readWrite param
        if (readWrite == true)
        {
           // theSubscriber = new ConfigRadioSubscriber(key);
        }
        else
        {
            theSubscriber = new RadioSubscriber(key);
        }

        // Return the radio subscriber pointer. The caller is responsible for deleting it.
        FUNCTION_EXIT;
        return theSubscriber;
    }

    std::vector<IRadioSubscriber*> RadioSubscriberFactory::getAllRadioSubscribers(const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getAllRadioSubscribers");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve all the records from the table 
       /* std::ostringstream sql;
        sql << "select " << PKEY_COL << ", " << RESOURCETYPE_COL << ", "
                         << RESOURCEID_COL <<", " << SSI_COL << ", "
                         << ALIAS_COL << ", "
						 << "TO_CHAR(" << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
						 << "TO_CHAR(nvl(" << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
			             << " from " << RASUBSCRIBER_TBL;*/
//		std::string strSql = databaseConnection->prepareSQLStatement( RA_SUBSCRIBER_SELECT_70002 );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_SUBSCRIBER_SELECT_70002 );
         
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
        columnNames.push_back(RESOURCETYPE_COL);       
        columnNames.push_back(RESOURCEID_COL);       
        columnNames.push_back(SSI_COL);       
        columnNames.push_back(ALIAS_COL);       
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioSubscriber
        std::vector<IRadioSubscriber*> subscriberPtrVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);

                TA_ASSERT(key > 0, "Invalid Subscriber Id index."); 

                if (key > 0)
                {
                    if (readWrite == true)
                    {
					    //fixed TD13419
                        subscriberPtrVector.push_back(new ConfigRadioSubscriber(i, *data));
                    }
                    else
                    {
                        subscriberPtrVector.push_back(new RadioSubscriber(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return subscriberPtrVector;
    }

	//TD 14848
	//zhou yuan++
	std::vector<IRadioSubscriber*> RadioSubscriberFactory::getRadioSubscribersByResourseType(const char resourseType, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getRadioSubscribersByResourseType");
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
		
        // create the SQL string to retrieve all the records from the table 
      /*  std::ostringstream sql;
        sql << "select " << PKEY_COL << ", " << RESOURCETYPE_COL << ", "
			<< RESOURCEID_COL <<", " << SSI_COL << ", "
			<< ALIAS_COL << ", "
	    	<< "TO_CHAR(" << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
   		    << "TO_CHAR(nvl(" << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << RASUBSCRIBER_TBL
			<< " where " << RESOURCETYPE_COL << " = '" << resourseType << "' ";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_SUBSCRIBER_SELECT_70003, resourseType);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_SELECT_70003, resourseType);
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(RESOURCETYPE_COL);       
        columnNames.push_back(RESOURCEID_COL);       
        columnNames.push_back(SSI_COL);       
        columnNames.push_back(ALIAS_COL);   
		//TD18654 marc_bugfix
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);  		
		//TD18654 marc_bugfix
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioSubscriber
        std::vector<IRadioSubscriber*> subscriberPtrVector;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);
				
                TA_ASSERT(key > 0, "Invalid Subscriber Id index."); 
				
                if (key > 0)
                {
                    if (readWrite == true)
                    {
						// subscriberPtrVector.push_back(new ConfigRadioSubscriber(i, *data));
                    }
                    else
                    {
                        subscriberPtrVector.push_back(new RadioSubscriber(i, *data));
                    }
                }
            }
			
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);
		
        FUNCTION_EXIT;
        return subscriberPtrVector;
    }
	//++zhou yuan

	
	std::vector<IRadioSubscriber*> RadioSubscriberFactory::getRadioSubscribersByResourceAlias(const std::string resourceAlias, const bool readWrite)
    {
        FUNCTION_ENTRY("getRadioSubscribersByResourceAlias");

		std::string alias(resourceAlias);
		std::transform(alias.begin(), alias.end(), alias.begin(), toupper);
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
		
        // create the SQL string to retrieve all the records from the table 
        //delete by lin
        //std::ostringstream sql;
        //sql << "select " << PKEY_COL << ", " << RESOURCETYPE_COL << ", "
		//	<< RESOURCEID_COL <<", " << SSI_COL << ", "
		//	<< ALIAS_COL << ", "
	    //	<< "TO_CHAR(" << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
   		//    << "TO_CHAR(nvl(" << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
        //    << " from " << RASUBSCRIBER_TBL
		//	<< " where upper(" << ALIAS_COL << ") = '" << databaseConnection->escapeQueryString(alias) << "' ";
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(RESOURCETYPE_COL);       
        columnNames.push_back(RESOURCEID_COL);       
        columnNames.push_back(SSI_COL);       
        columnNames.push_back(ALIAS_COL);   
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);  		
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        //TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		//strSql.strCommon = sql.str();
		
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_SELECT_70012,
			  databaseConnection->escapeQueryString(alias));
		
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        
        // create the vector of pointers to IRadioSubscriber
        std::vector<IRadioSubscriber*> subscriberPtrVector;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);
				
                TA_ASSERT(key > 0, "Invalid Subscriber Id index."); 
				
                if (key > 0)
                {
                    if (readWrite == true)
                    {
						// subscriberPtrVector.push_back(new ConfigRadioSubscriber(i, *data));
                    }
                    else
                    {
                        subscriberPtrVector.push_back(new RadioSubscriber(i, *data));
                    }
                }
            }
			
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);
		
        FUNCTION_EXIT;
        return subscriberPtrVector;
    }

    void RadioSubscriberFactory::changeRadioSubscriber(const RadioResource&  resource)
    {
        FUNCTION_ENTRY("changeRadioSubscriber");
        try
        {
            RadioSubscriberHelper helper(resource);
            helper.writeSubscriberData();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to change radio subscriber with resource id: %d.", resource.id);
            throw;  
        }
        
        FUNCTION_EXIT;
    }

    void RadioSubscriberFactory::deleteRadioSubscriber(const RadioResource&  resource)
    {
        FUNCTION_ENTRY("deleteRadioSubscriber");
        try
        {
            RadioSubscriberHelper helper(resource);
            helper.deleteSubscriber();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to delete radio subscriber with resource id: %d.", resource.id);
            throw;  
        }
        
        FUNCTION_EXIT;

    }

    IConfigRadioSubscriber* RadioSubscriberFactory::createNew(const RadioResource&  resource)
    {
        ConfigRadioSubscriber* data = new ConfigRadioSubscriber(resource);
        data->save();

        return data;
    }

	IRadioSubscriber* RadioSubscriberFactory::getRadioSubscriberBySSI(unsigned long ssi)
	{
		FUNCTION_ENTRY("getRadioSubscriberBySSI");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the name of the radio subscriber
        // this is a check to ensure an radio subscriber with the specified key actually exists.        
       /* std::ostringstream sql;
        sql << "select " 
                    << PKEY_COL << ", "
                    << RESOURCETYPE_COL << ", "
                    << RESOURCEID_COL << ", "
                    << SSI_COL << ", "
                    << ALIAS_COL << ", " 
	     	    	<< "TO_CHAR(" << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
   		            << "TO_CHAR(nvl(" << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')" << " from " 
                    << RASUBSCRIBER_TBL << " where " 
                    << SSI_COL << " = " << ssi;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_SUBSCRIBER_SELECT_70004, ssi);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBER_SELECT_70004, ssi);
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(RESOURCETYPE_COL);
		columnNames.push_back(RESOURCEID_COL);
		columnNames.push_back(SSI_COL);
		columnNames.push_back(ALIAS_COL);	
		//TD18654 marc_bugfix
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);  		
		//TD18654 marc_bugfix


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        if (0 == data->getNumRows())
        {
            // radio subscriber record with the specified SSI not found
            std::ostringstream reason;
		    reason << "No record found for Radio Subscriber with SSI = " << ssi;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Invalid Radio Subscriber SSI." ) );
        }

        // Don't need to do anything with the data, only needed to check if it exists
        TA_ASSERT(1 == data->getNumRows(), "Invalid number of records.");        

        // Create the radio subscriber object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioSubscriber* theSubscriber = NULL;
        
        // decide the type of radio subscriber object to return based on the readWrite param
        theSubscriber = new RadioSubscriber(0,*data.get());
		
		FUNCTION_EXIT;
		return theSubscriber;
	
	}

    IConfigRadioSubscriber* RadioSubscriberFactory::copyRadioSubscriber(const IConfigRadioSubscriber* radioSubscriberToCopy)
    {
        FUNCTION_ENTRY("copyRadioSubscriber");

        TA_ASSERT(radioSubscriberToCopy !=  NULL, "The ConfigRadioSubscriber to copy was NULL");
        
        const ConfigRadioSubscriber* cast = dynamic_cast<const ConfigRadioSubscriber*>(radioSubscriberToCopy);

        TA_ASSERT(cast != NULL, "VideoTriggeringEvent passed could not be converted into a ConfigRadioSubscriber");

        FUNCTION_EXIT;
        return new ConfigRadioSubscriber(*cast);

    }

	IConfigRadioSubscriber* RadioSubscriberFactory::createNewSubscriber()
	{
		return new ConfigRadioSubscriber();
	}

	void RadioSubscriberFactory::updateRadioSubscriber(unsigned long key, const RadioResource& newResource)
	{
		ConfigRadioSubscriber* data = new ConfigRadioSubscriber(key);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating resource (old value -> new value): id=%u->%u, ssi=%u->%u, alias=%s->%s, type=%c->%c",\
			data->getResourceId(), newResource.id, data->getSSI(), newResource.ssi, data->getAlias().c_str(), newResource.alias.c_str(), data->getResourceType(), newResource.type);

		data->setAlias(newResource.alias);
		data->setResourceId(newResource.id);
		data->setResourceType(newResource.type);
		data->setSSI(newResource.ssi);

		data->applyChanges();
	}
}

