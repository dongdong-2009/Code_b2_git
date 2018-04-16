/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriberHelper.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * RadioSubscriberHelper is an object that is held by Subscriber and ConfigSubscriber objects.
  * It contains all data used by Subscribers, and manipulation methods for the data. 
  * It helps avoid re-writing code for both Subscriber and ConfigSubscriber.
  */

#if !defined(AFX_RADIO_SUBSCRIBER_HELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define AFX_RADIO_SUBSCRIBER_HELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_

#include <string>
#include "bus/radio/radio_bus/src/radiotypes.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

// forward declarations
namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class RadioSubscriberHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * RadioSubscriberHelper
          *
          * The default constructor creates a new RadioSubscriberHelper for configuration, 
          * initializing the key to 0 - it will be specified once this subscriber is 
          * written to the database for the first time.
          */
        RadioSubscriberHelper();

	    /**
	      * RadioSubscriberHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */
        RadioSubscriberHelper(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * RadioSubscriberHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const RadioSubscriberHelper& theRadioSubscriberHelper
	      */
        RadioSubscriberHelper(const RadioSubscriberHelper& theRadioSubscriberHelper);

	    /**
	      * RadioSubscriberHelper
	      * 
	      * This constructor creates a new Subscriber object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        RadioSubscriberHelper(const unsigned long key);
       
        /**
        * RadioSubscriberHelper
        * 
        * this constructor creates a new subscriber object for the specific radio resource
        * 
        * @return 
        * @param : const RadioResource resource
        * 
        * @exception <exceptions> Optional
        */
        RadioSubscriberHelper(const RadioResource&  resource);

         /** 
          * ~RadioSubscriberHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~RadioSubscriberHelper();

        //
        // RadioSubscriberHelper class methods
        //

        /**
          * writeSubscriberData
          * 
          * Write this subscriber to the database. If this is a new subscriber, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the Subscriber object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writeSubscriberData();

        /**
          * deleteSubscriber
          *
          * Remove this subscriber from the database. Once this method has been executed, the
          * RadioSubscriberHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this subscriber was initially loaded from the database
          *      OR     - writeSubscriberData() has already been called
          */
        void deleteSubscriber();

        //
        // IRadioSubscriber interface methods
        //

	    /**
	      * getResourceType
	      * 
	      * @return char
	      */
	    char getResourceType();

	    /**
	      * getResourceId
	      * 
	      * @return unsigned long
	      */
	    unsigned long getResourceId();

	    /**
	      * getSSI
	      * 
	      * @return unsigned long
	      */
	    unsigned long getSSI();

	    /**
	      * getAlias
	      * 
	      * @return std::string
	      */
	    std::string getAlias();

        //
        // IConfigTrain interface methods
        //
        
        //todo: need to add the sets methods later



	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        unsigned long getKey();


		virtual void setResourceType(RadioResourceType type);

		virtual void setResourceId(unsigned long rid);

		virtual void setSSI(unsigned long ssi);

		virtual void setAlias(const std::string& alias);		

        /**
          * invalidate
          *
          * Make the data contained by this subscriber as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database. This includes associated profiles and regions if required.
          * It is a precondition that writeSubscriberData() has been called prior to calling
          * this method on a new subscriber, as it does not make any sense to invalidate a
          * subscriber that has not yet been written to the database.
          *
          * pre: Either - this subscriber was initially loaded from the database
          *      OR     - writeSubscriberData() has already been called
          */
        void invalidate();

        //
        // IConfigItem interface methods
        //

        /**
          * isNew
          *
          * This returns whether this is a new Subscriber.  A new Subscriber is created by the default
          * constructor.  Once written to the database, it is no longer considered new.
          *
          * @return true if this is a new Subscriber
          */
        bool isNew() const
        {
            return m_isNew;
        };


		time_t getDateModified();
		time_t getDateCreated();
    private:

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
          *
          * @return RadioSubscriberHelper& 
	      * @param : const RadioSubscriberHelper&
	      */
		RadioSubscriberHelper& operator=(const RadioSubscriberHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this subscriber was initially loaded from the database
          *      OR     - writeSubscriberData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the Subscriber Id record cannot be found for the helper 
          *             constructed with an Subscriber Id.
          *             - NOT_UNIQUE if the Subscriber Id returns multiple records 
          */
        void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);

        /**
          * modifyExistingSubscriber
          *
          * This will update an existing Subscriber in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the Subscriber object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the subscriber id references a non-existent RA_SUBSCRIBER record. 
          */
        void modifyExistingSubscriber();
        
        /**
          * addNewSubscriber
          *
          * This will add a new Subscriber in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the Subscriber object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the subscriber id references a non-existent RA_SUBSCRIBER record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewSubscriber();

	    /**
	      * validateKeyExistence
	      * 
	      * This checks if the primary key is in the db.
	      * 
	      * @return void 
	      * 
	      * @exception TA_Base_Core::TransactiveException 
          *            If primary key is not in the database.
	      */
        void validateKeyExistence();


        /**
        * validateExistenceWithResource
        * 
        * This checks if the entry is in the db already giving the current set of 
        * member variable (except primary key)
        * 
        * @return bool: true if it exists, otherwise false 
        * 
        * @exception <exceptions> Optional
        */
        bool validateExistenceWithPkeyUnknown();
        
	    /**
	      * getNextSequenceNumber
	      * 
	      * Returns the next number in the primary key sequence (from database).
	      * 
	      * @return unsigned long 
	      * 
	      * @exception  DatabaseException
          *             - The database has is closed or unable to obtain the next sequence
          *             number from the database.
          *             DataException
          *             - If the next sequence number cannot be converted to an unsigned 
		  *             long after obtaining it from the database.
	      */
        unsigned long getNextSequenceNumber();

		/**
		  * getLocalDatabaseName
		  *
		  * Returns the database server name that it will connect to
		  *
		  * @return std::string
		  * 
		  * @exception DatabaseException
		  *            - Unable to find the connection string file
		  *            - Unable to find the data type
		  */
		static std::string getLocalDatabaseName();

    private:	    

        bool            m_isNew;
        bool            m_isValidData;
        
        /**
	      * RA_SUBSCRIBER fields
          */

        unsigned long   m_key;      
        char m_resourceType;
        unsigned long m_resourceId;
        unsigned long m_ssi;
        std::string m_alias;

        time_t m_dateCreated; 
        time_t m_dateModified; 
    };
}
#endif // !defined(AFX_RADIO_SUBSCRIBER_HELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
