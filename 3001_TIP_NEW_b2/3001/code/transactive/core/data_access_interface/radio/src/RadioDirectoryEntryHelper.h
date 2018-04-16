/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntryHelper.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * RadioDirectoryEntryHelper is an object that is held by RadioDirectoryEntry and ConfigRadioDirectoryEntry objects.
  * It contains all data used by RadioDirectoryEntrys, and manipulation methods for the data. 
  * It helps avoid re-writing code for both RadioDirectoryEntry and ConfigRadioDirectoryEntry.
  */

#if !defined(AFX_RADIO_DIRECTORY_ENTRY_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define AFX_RADIO_DIRECTORY_ENTRY_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_

#include <string>

// forward declarations
namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class RadioDirectoryEntryHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * RadioDirectoryEntryHelper
          *
          * The default constructor creates a new RadioDirectoryEntryHelper for configuration, 
          * initializing the key to 0 - it will be specified once this RadioDirectoryEntry is 
          * written to the database for the first time.
          */
        RadioDirectoryEntryHelper();

	    /**
	      * RadioDirectoryEntryHelper
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
        RadioDirectoryEntryHelper(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * RadioDirectoryEntryHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const RadioDirectoryEntryHelper& theRadioDirectoryEntryHelper
	      */
        RadioDirectoryEntryHelper(const RadioDirectoryEntryHelper& theRadioDirectoryEntryHelper);

	    /**
	      * RadioDirectoryEntryHelper
	      * 
	      * This constructor creates a new RadioDirectoryEntry object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        RadioDirectoryEntryHelper(const unsigned long key);
        
        RadioDirectoryEntryHelper(const unsigned long owner,const unsigned long category, const std::string name,bool bNewData=false);
       
        /** 
          * ~RadioDirectoryEntryHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~RadioDirectoryEntryHelper();

        //
        // RadioDirectoryEntryHelper class methods
        //

        /**
          * writePredefinedMessageData
          * 
          * Write this RadioDirectoryEntry to the database. If this is a new RadioDirectoryEntry, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the RadioDirectoryEntry object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writeDirectoryEntryData();

        /**
          * deleteRadioDirectoryEntry
          *
          * Remove this RadioDirectoryEntry from the database. Once this method has been executed, the
          * RadioDirectoryEntryHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this RadioDirectoryEntry was initially loaded from the database
          *      OR     - writeRadioDirectoryEntryData() has already been called
          */
        void deleteDirectoryEntry();

        //
        // IRadioDirectoryEntry interface methods
        //
	    /**
	      * getOwner
	      * 
	      * @return unsigned long
	      */
	    unsigned long getOwner();

	    /**
	      * getCategory
	      * 
	      * @return unsigned long
	      */
	    unsigned long getCategory();

        //
        // IConfigRadioDirectoryEntry interface methods
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
        * getName
        * 
        * Returns the name for this item
        * 
        */
        std::string getName();

        /**
          * invalidate
          *
          * Make the data contained by this RadioDirectoryEntry as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database. This includes associated profiles and regions if required.
          * It is a precondition that writeRadioDirectoryEntryData() has been called prior to calling
          * this method on a new RadioDirectoryEntry, as it does not make any sense to invalidate a
          * RadioDirectoryEntry that has not yet been written to the database.
          *
          * pre: Either - this RadioDirectoryEntry was initially loaded from the database
          *      OR     - writeRadioDirectoryEntryData() has already been called
          */
        void invalidate();

        //
        // IConfigItem interface methods
        //

        /**
          * isNew
          *
          * This returns whether this is a new RadioDirectoryEntry.  A new RadioDirectoryEntry is created by the default
          * constructor.  Once written to the database, it is no longer considered new.
          *
          * @return true if this is a new RadioDirectoryEntry
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
          * @return RadioDirectoryEntryHelper& 
	      * @param : const RadioDirectoryEntryHelper&
	      */
		RadioDirectoryEntryHelper& operator=(const RadioDirectoryEntryHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this RadioDirectoryEntry was initially loaded from the database
          *      OR     - writeRadioDirectoryEntryData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the RadioDirectoryEntry Id record cannot be found for the helper 
          *             constructed with an RadioDirectoryEntry Id.
          *             - NOT_UNIQUE if the RadioDirectoryEntry Id returns multiple records 
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
          * This will update an existing RadioDirectoryEntry in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the RadioDirectoryEntry object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the RadioDirectoryEntry id references a non-existent RA_DIRECTORY_ENTRY record. 
          */
        void modifyExistingDirectoryEntry();
        
        /**
          * addNewPredefinedMessage
          *
          * This will add a new RadioDirectoryEntry in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the RadioDirectoryEntry object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the RadioDirectoryEntry id references a non-existent RA_DIRECTORY_ENTRY record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewDirectoryEntry();

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

    private:	    

        bool            m_isNew;
        bool            m_isValidData;
        
        /**
	      * RA_DIRECTORY_ENTRY fields
          */

        unsigned long m_key;      
        unsigned long m_owner;
        unsigned long m_category;
        std::string m_name;

    private://constants used in the class
        static const std::string RA_DIRECTORYENTRY_TBL;

        // Column names in RA_DIRECTORY_ENTRY
        static const std::string PKEY_COL;
        static const std::string NAME_COL;
        static const std::string OWNER_COL;
        static const std::string CATEGORY_COL;
        static const unsigned long  INVALID_KEY;

    };
}
#endif // !defined(AFX_RADIO_DIRECTORY_ENTRY_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
