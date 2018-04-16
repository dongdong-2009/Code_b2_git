/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioPredefinedTextMessageHelper.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  
  * 1: changed the const variables difinition

  * RadioPredefinedTextMessageHelper is an object that is held by TextMessage and ConfigTextMessage objects.
  * It contains all data used by TextMessages, and manipulation methods for the data. 
  * It helps avoid re-writing code for both TextMessage and ConfigTextMessage.
  */

#if !defined(AFX_RADIO_PREDEFINED_TEXT_MESSAGE_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define AFX_RADIO_PREDEFINED_TEXT_MESSAGE_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_

#include <string>

// forward declarations
namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class RadioPredefinedTextMessageHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * RadioPredefinedTextMessageHelper
          *
          * The default constructor creates a new RadioPredefinedTextMessageHelper for configuration, 
          * initializing the key to 0 - it will be specified once this TextMessage is 
          * written to the database for the first time.
          */
        RadioPredefinedTextMessageHelper();

	    /**
	      * RadioPredefinedTextMessageHelper
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
        RadioPredefinedTextMessageHelper(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * RadioPredefinedTextMessageHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const RadioPredefinedTextMessageHelper& theRadioPredefinedTextMessageHelper
	      */
        RadioPredefinedTextMessageHelper(const RadioPredefinedTextMessageHelper& theRadioPredefinedTextMessageHelper);

	    /**
	      * RadioPredefinedTextMessageHelper
	      * 
	      * This constructor creates a new TextMessage object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        RadioPredefinedTextMessageHelper(const unsigned long key);
       

        /**
        * RadioPredefinedTextMessageHelper
        * 
        * this constructor creates a new TextMessage object for the specified key and operator
        * 
        * @return 
        * @param : const unsigned long operatorid
        * @param : const unsigned long key
        * 
        * @exception <exceptions> Optional
        */
        RadioPredefinedTextMessageHelper(const unsigned long operatorid,const unsigned long key);

        /**
        * RadioPredefinedTextMessageHelper
        * 
        * this constructor creates a new TextMessage object for the specified operator, key, messageName and MessageText
        * 
        * @return 
        * @param : const unsigned long operatorid
        * @param : const unsigned long messageKey
        * @param : const std::string messageName
        * @param : const std::string messageText
        * 
        * @exception <exceptions> Optional
        */
        RadioPredefinedTextMessageHelper(const unsigned long operatorid, const unsigned long messageKey, const std::string messageName, const std::string messageText);

        /** 
          * ~RadioPredefinedTextMessageHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~RadioPredefinedTextMessageHelper();

        //
        // RadioPredefinedTextMessageHelper class methods
        //

        /**
          * writePredefinedMessageData
          * 
          * Write this TextMessage to the database. If this is a new TextMessage, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the TextMessage object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writePredefinedMessageData();

        /**
          * deleteTextMessage
          *
          * Remove this TextMessage from the database. Once this method has been executed, the
          * RadioPredefinedTextMessageHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this TextMessage was initially loaded from the database
          *      OR     - writeTextMessageData() has already been called
          */
        void deletePredefinedMessage();

        //
        // IRadioPredefinedTextMessage interface methods
        //
	    /**
	      * getOperator
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperator();

	    /**
	      * getShortName
	      * 
	      * @return std::string
	      */
	    virtual std::string getShortName();

	    /**
	      * getMessageText
	      * 
	      * @return std::string
	      */
	    virtual std::string getMessageText();
        //
        // IConfigRadioPredefinedTextMessage interface methods
        //
	    virtual void setOperator(unsigned long operatorId);

        virtual void setShortName(const std::string& shortName);

	    virtual void setMessageText(const std::string& messageText);
		
        /** 
          * getType
          *
          * description
          * TD14312
          *
          * @return 
          */
		bool getType();      
		
        /** 
          * getType
          *
          * description
          * TD14312
          *
          * @return 
          */
		void setType(bool bType);  

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
          * Make the data contained by this TextMessage as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database. This includes associated profiles and regions if required.
          * It is a precondition that writeTextMessageData() has been called prior to calling
          * this method on a new TextMessage, as it does not make any sense to invalidate a
          * TextMessage that has not yet been written to the database.
          *
          * pre: Either - this TextMessage was initially loaded from the database
          *      OR     - writeTextMessageData() has already been called
          */
        void invalidate();

        //
        // IConfigItem interface methods
        //

        /**
          * isNew
          *
          * This returns whether this is a new TextMessage.  A new TextMessage is created by the default
          * constructor.  Once written to the database, it is no longer considered new.
          *
          * @return true if this is a new TextMessage
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
          * @return RadioPredefinedTextMessageHelper& 
	      * @param : const RadioPredefinedTextMessageHelper&
	      */
		RadioPredefinedTextMessageHelper& operator=(const RadioPredefinedTextMessageHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this TextMessage was initially loaded from the database
          *      OR     - writeTextMessageData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the TextMessage Id record cannot be found for the helper 
          *             constructed with an TextMessage Id.
          *             - NOT_UNIQUE if the TextMessage Id returns multiple records 
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
          * This will update an existing TextMessage in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the TextMessage object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the TextMessage id references a non-existent RA_TEXTMESSAGE_INBOX record. 
          */
        void modifyExistingPredefinedMessage();
        
        /**
          * addNewPredefinedMessage
          *
          * This will add a new TextMessage in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the TextMessage object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the TextMessage id references a non-existent RA_TEXTMESSAGE_INBOX record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewPredefinedMessage();

	    /**
	      * validateKeyExistence
	      * 
	      * This checks if the primary key is in the db.
	      * 
	      * @return bool true/false: if the primary key exist in the db, return true, otherwise false 
	      * 
	      * @exception TA_Base_Core::TransactiveException 
          *            If primary key is not in the database.
	      */
        bool validateKeyExistence();

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
		bool            m_bType; //the predefined message type
        bool            m_isNew;
        bool            m_isValidData;
        
        /**
	      * RA_TEXTMESSAGE_INBOX fields
          */

        unsigned long   m_key;      
        unsigned long m_operator;
        std::string m_shortName;
        std::string m_messageText;
    };
}
#endif // !defined(AFX_RADIO_PREDEFINED_TEXT_MESSAGE_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
