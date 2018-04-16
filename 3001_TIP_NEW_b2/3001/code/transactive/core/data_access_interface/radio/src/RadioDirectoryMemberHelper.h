/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * RadioDirectoryMemberHelper is an object that is held by RadioDirectoryMember and ConfigRadioDirectoryMember objects.
  * It contains all data used by RadioDirectoryMembers, and manipulation methods for the data. 
  * It helps avoid re-writing code for both RadioDirectoryMember and ConfigRadioDirectoryMember.
  */

#if !defined(AFX_RADIO_DIRECTORY_MEMBER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define AFX_RADIO_DIRECTORY_MEMBER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_

// forward declarations
namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class RadioDirectoryMemberHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * RadioDirectoryMemberHelper
          *
          * The default constructor creates a new RadioDirectoryMemberHelper for configuration, 
          * initializing the key to 0 - it will be specified once this RadioDirectoryMember is 
          * written to the database for the first time.
          */
        RadioDirectoryMemberHelper();

	    /**
	      * RadioDirectoryMemberHelper
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
        RadioDirectoryMemberHelper(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * RadioDirectoryMemberHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long parent 
          *          - the parent id from the RA_DIRECTORY_ENTRY PKEY
	      * @param : const char resource_type
          *          - the resource type          
	      * @param : const char resource_id
          *          - the resource id
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
        RadioDirectoryMemberHelper(unsigned long parent, char resource_type, unsigned long resource_id);

	    /**
	      * RadioDirectoryMemberHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const RadioDirectoryMemberHelper& theRadioDirectoryMemberHelper
	      */
        RadioDirectoryMemberHelper(const RadioDirectoryMemberHelper& theRadioDirectoryMemberHelper);

	    /**
	      * RadioDirectoryMemberHelper
	      * 
	      * This constructor creates a new RadioDirectoryMember object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        RadioDirectoryMemberHelper(const unsigned long key);
       
        /** 
          * ~RadioDirectoryMemberHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~RadioDirectoryMemberHelper();

        //
        // RadioDirectoryMemberHelper class methods
        //

        /**
          * writePredefinedMessageData
          * 
          * Write this RadioDirectoryMember to the database. If this is a new RadioDirectoryMember, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the RadioDirectoryMember object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writeData();

        /**
          * deleteRadioDirectoryMember
          *
          * Remove this RadioDirectoryMember from the database. Once this method has been executed, the
          * RadioDirectoryMemberHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this RadioDirectoryMember was initially loaded from the database
          *      OR     - writeRadioDirectoryMemberData() has already been called
          */
        void deleteData();

        //
        // IRadioDirectoryMember interface methods
        //
	    /**
	      * getParent
	      * 
	      * @return unsigned long
	      */
	    unsigned long getParent();

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

        //
        // IConfigRadioDirectoryMember interface methods
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

        /**
          * invalidate
          *
          * Make the data contained by this RadioDirectoryMember as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database. This includes associated profiles and regions if required.
          * It is a precondition that writeRadioDirectoryMemberData() has been called prior to calling
          * this method on a new RadioDirectoryMember, as it does not make any sense to invalidate a
          * RadioDirectoryMember that has not yet been written to the database.
          *
          * pre: Either - this RadioDirectoryMember was initially loaded from the database
          *      OR     - writeRadioDirectoryMemberData() has already been called
          */
        void invalidate();

        //
        // IConfigItem interface methods
        //

        /**
          * isNew
          *
          * This returns whether this is a new RadioDirectoryMember.  A new RadioDirectoryMember is created by the default
          * constructor.  Once written to the database, it is no longer considered new.
          *
          * @return true if this is a new RadioDirectoryMember
          */
        bool isNew() const
        {
            return m_isNew;
        };

    private:
	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
          *
          * @return RadioDirectoryMemberHelper& 
	      * @param : const RadioDirectoryMemberHelper&
	      */
		RadioDirectoryMemberHelper& operator=(const RadioDirectoryMemberHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this RadioDirectoryMember was initially loaded from the database
          *      OR     - writeRadioDirectoryMemberData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the RadioDirectoryMember Id record cannot be found for the helper 
          *             constructed with an RadioDirectoryMember Id.
          *             - NOT_UNIQUE if the RadioDirectoryMember Id returns multiple records 
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
          * modifyExistingPredefinedMessage
          *
          * This will update an existing RadioDirectoryMember in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the RadioDirectoryMember object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the RadioDirectoryMember id references a non-existent RA_DIRECTORY_MEMBER record. 
          */
        void modifyExistingDirectoryMember();
        
        /**
          * addNewPredefinedMessage
          *
          * This will add a new RadioDirectoryMember in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the RadioDirectoryMember object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the RadioDirectoryMember id references a non-existent RA_DIRECTORY_MEMBER record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewDirectoryMember();

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

    private:	    

        bool            m_isNew;
        bool            m_isValidData;
        
        /**
	      * RA_DIRECTORY_MEMBER fields
          */

        unsigned long   m_parent;      
        char m_resourceType;
        unsigned long m_resourceId;
    };
}
#endif // !defined(AFX_RADIO_DIRECTORY_MEMBER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
