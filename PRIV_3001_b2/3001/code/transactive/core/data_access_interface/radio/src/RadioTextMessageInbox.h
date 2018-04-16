/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInbox.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Class RadioTextMessageInbox is an implementation of the interface IRadioTextMessageInbox.
  * It holds the data specific to a RadioTextMessageInbox entry in the database, 
  * and allows read-only access to that data.
  */

#if !defined(AFX_RADIO_TEXT_MESSAGE_INBOX_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define AFX_RADIO_TEXT_MESSAGE_INBOX_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxHelper.h"


namespace TA_IRS_Core
{
    class RadioTextMessageInbox : public IRadioTextMessageInbox  
    {
    public:

	    /**
	      * RadioTextMessageInbox
	      * 
	      * Constructs a train object based on the primary key
	      * 
	      * @param : const unsigned long key
	      */
	    RadioTextMessageInbox(const unsigned long key);
        
	    /**
	      * RadioTextMessageInbox
	      * 
	      * Constructs a new RadioTextMessageInbox object using the information provided in IData interface
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      * 
	      * @see RadioTextMessageInboxHelper::RadioTextMessageInboxHelper(const unsigned long, TA_Base_Core::IData&);
	      */
        RadioTextMessageInbox(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * ~RadioTextMessageInbox
	      * 
	      * Destructor 
	      */
	    virtual ~RadioTextMessageInbox();

        //
        // IRadioTextMessageInbox methods
        //
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

	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        virtual unsigned long getKey();

		virtual std::string getRecipientConsole();
	    /**
	      * invalidate
	      * 
	      * Make the data contained by this item as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
	      * 
	      * @return void 
	      */
        virtual void invalidate();

    private:

	    /**
	      * RadioTextMessageInbox
	      * 
	      * Copy constructor (disabled).
          * If a copy constructor is required care should be taken. The copy constructor to 
          * the RadioTextMessageInboxHelper has only been written for ConfigRadioTextMessageInbox objects and will not copy 
          * the RadioTextMessageInbox helper for a read-only object (ie this one).
	      * 
	      * @param : const RadioTextMessageInbox&
	      */
        RadioTextMessageInbox(const RadioTextMessageInbox&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return RadioTextMessageInbox& 
	      * @param : const RadioTextMessageInbox &
	      */
		RadioTextMessageInbox& operator=(const RadioTextMessageInbox&);

    private:

        RadioTextMessageInboxHelper m_radioTextHelper;
    };
}
#endif // !defined(AFX_RADIO_TEXT_MESSAGE_INBOX_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
