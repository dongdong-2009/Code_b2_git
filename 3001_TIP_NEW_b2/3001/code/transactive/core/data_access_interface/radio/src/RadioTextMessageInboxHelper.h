/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInboxHelper.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: changed the const variables difinition
  * RadioTextMessageInboxHelper is an object that is held by TextMessage and ConfigTextMessage objects.
  * It contains all data used by TextMessages, and manipulation methods for the data. 
  * It helps avoid re-writing code for both TextMessage and ConfigTextMessage.
  */

#if !defined(AFX_RADIO_TEXT_MESSAGE_INBOX_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define AFX_RADIO_TEXT_MESSAGE_INBOX_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_

#include <string>

// forward declarations
namespace TA_Base_Core
{
    class IData;
	class SQLTypeAdapter;
}


namespace TA_IRS_Core
{
    class RadioTextMessageInboxHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * RadioTextMessageInboxHelper
          *
          * The default constructor creates a new RadioTextMessageInboxHelper for configuration, 
          * initializing the key to 0 - it will be specified once this TextMessage is 
          * written to the database for the first time.
          */
        RadioTextMessageInboxHelper();

	    /**
	      * RadioTextMessageInboxHelper
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
        RadioTextMessageInboxHelper(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * RadioTextMessageInboxHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const RadioTextMessageInboxHelper& theRadioTextMessageInboxHelper
	      */
        RadioTextMessageInboxHelper(const RadioTextMessageInboxHelper& theRadioTextMessageInboxHelper);

	    /**
	      * RadioTextMessageInboxHelper
	      * 
	      * This constructor creates a new TextMessage object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        RadioTextMessageInboxHelper(const unsigned long key);
       
        RadioTextMessageInboxHelper(const time_t timestamp,const unsigned long oper,bool isread,const std::string& msgtext,const std::string& source, const std::string& recipient);

        /** 
          * ~RadioTextMessageInboxHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~RadioTextMessageInboxHelper();

        //
        // RadioTextMessageInboxHelper class methods
        //

        /**
          * writeTextMessageData
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
        void writeTextMessageData();

        /**
          * deleteTextMessage
          *
          * Remove this TextMessage from the database. Once this method has been executed, the
          * RadioTextMessageInboxHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this TextMessage was initially loaded from the database
          *      OR     - writeTextMessageData() has already been called
          */
        void deleteTextMessage();

        //
        // IRadioTextMessageInbox interface methods
        //

	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        unsigned long getKey();

	    /**
	      * getTimeStamp
	      * 
	      * @return time_t
	      */
	    virtual time_t getTimeStamp();

	    /**
	      * getOperator
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperator();

	    virtual bool isRead();

	    /**
	      * getMessageText
	      * 
	      * @return std::string
	      */
	    virtual std::string getMessageText();

	    /**
	      * getSourceAlias
	      * 
	      * @return std::string
	      */
	    virtual std::string getSourceAlias();

//TD18825
		virtual std::string getRecipientConsole();
//TD18825

        //
        // IConfigRadioTextMessageInbox interface methods
        //
        
        virtual void setOperator(unsigned long operatorId);
        virtual void setTimeStamp(time_t pTime);
        virtual void setRead(bool pRead);
        virtual void setMessageText(const std::string& messageText);
		virtual void setSourceAlias(const std::string& srcAlias);
//TD18825
		virtual void setRecipientConsole(const std::string& recipient);
//TD18825

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
	      * RadioTextMessageInboxHelper::getUpdateDateSql
	      * 
	      * convert the time_t into the string 
	      * 
	      * @return std::string 
	      * @param : time_t datetime
	      * 
	      * @exception <exceptions> Optional
	      */
		void RadioTextMessageInboxHelper::getUpdateDateSql(TA_Base_Core::SQLTypeAdapter& rSqlParam, time_t datetime, bool bIsAQ);
	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
          *
          * @return RadioTextMessageInboxHelper& 
	      * @param : const RadioTextMessageInboxHelper&
	      */
		RadioTextMessageInboxHelper& operator=(const RadioTextMessageInboxHelper&);

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
          * modifyExistingTextMessage
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
        void modifyExistingTextMessage();
        
        /**
          * addNewTextMessage
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
        void addNewTextMessage();

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
	      * RA_TEXTMESSAGE_INBOX fields
          */

        unsigned long   m_key;      
        time_t m_timestamp;
        unsigned long m_operator;
        bool m_isread;
        std::string m_messageText;
		std::string m_sourceAlias;
//TD18825
		std::string m_recipientConsole;
//TD18825
    };
}
#endif // !defined(AFX_RADIO_TEXT_MESSAGE_INBOX_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
