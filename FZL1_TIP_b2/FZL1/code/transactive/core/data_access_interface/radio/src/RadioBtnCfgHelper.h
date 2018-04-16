/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfgHelper.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(RadioBtnCfgHelper_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define RadioBtnCfgHelper_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_
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
    class RadioBtnCfgHelper 
    {
    public:

	    /**
	      * RadioBtnCfgHelper
	      * 
	      */
        RadioBtnCfgHelper();

	    /**
	      * RadioBtnCfgHelper
	      * 
	      * @param : const RadioBtnCfgHelper& helper
	      */
        RadioBtnCfgHelper(const RadioBtnCfgHelper& helper);
        
	    /**
	      * RadioBtnCfgHelper
	      * 
	      * Constructor
	      */
        RadioBtnCfgHelper(unsigned long key);

	    /**
	      * RadioBtnCfgHelper
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData &data
	      */
        RadioBtnCfgHelper(unsigned long row, TA_Base_Core::IData &data);

        /**
        * RadioBtnOperatorCfgHelper
        * 
        * create helper object with particular profile id ,button ref id and resource
        */
        RadioBtnCfgHelper(bool isProfile, unsigned long profileid, unsigned long btnref,  const RadioResource&  resource);

	    /**
	      * ~RadioBtnCfgHelper
	      */
        virtual ~RadioBtnCfgHelper();

	    /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    unsigned long getKey();

	    /**
	      * getProfile
	      * 
	      * @return unsigned long
	      */
	    unsigned long getOperatorProfile();

        bool isProfile();

	    /**
	      * getButtonRef
	      * 
	      * @return unsigned long
	      */
	    unsigned long getButtonRef();

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
	      * isNew
	      * 
	      * @return bool 
	      */
        bool isNew() const;

        /**
	      * invalidate
	      * 
	      * @return void 
	      */
        void invalidate();

        /**
          * writeButtonData
          * 
          * Write this button to the database. If this is a new button, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the Button object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writeButtonData();


    private:

        //Disabled Method
        RadioBtnCfgHelper& operator=(const RadioBtnCfgHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this train was initially loaded from the database
          *      OR     - writeTrainData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the Train Id record cannot be found for the helper 
          *             constructed with an Train Id.
          *             - NOT_UNIQUE if the Train Id returns multiple records 
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
        void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

        /**
          * modifyExistingButton
          *
          * This will update an existing Button in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the Button object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the subscriber id references a non-existent RA_SUBSCRIBER record. 
          */
        void modifyExistingButton();
        
        /**
          * addNewButton
          *
          * This will add a new Button in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the Button object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the subscriber id references a non-existent RA_SUBSCRIBER record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewButton();

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
        unsigned long   m_key;
        unsigned long   m_operator_profile; // either the operator id / profile id
        bool            m_isProfile;
        unsigned long   m_buttonRef;
        char            m_resourceType;
        unsigned long   m_resourceId;

        bool            m_isNew;
        bool            m_isValidData;

    };
}
#endif // !defined(RadioBtnCfgHelper_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
