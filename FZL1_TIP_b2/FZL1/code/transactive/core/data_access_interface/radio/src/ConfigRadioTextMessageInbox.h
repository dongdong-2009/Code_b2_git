/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioTextMessageInbox.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(AFX_CONFIGRADIOTEXTMESSAGEINBOX_H__438DBFDB_97D8_467D_B120_9234DCD5FBAF__INCLUDED_)
#define AFX_CONFIGRADIOTEXTMESSAGEINBOX_H__438DBFDB_97D8_467D_B120_9234DCD5FBAF__INCLUDED_


#include <string>
#include "core/data_access_interface/radio/src/IRadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxHelper.h"


namespace TA_IRS_Core
{
    class ConfigRadioTextMessageInbox : public IRadioTextMessageInbox  
    {
    public:
        ConfigRadioTextMessageInbox();
	    ConfigRadioTextMessageInbox(const unsigned long key);
        ConfigRadioTextMessageInbox(const unsigned long row, TA_Base_Core::IData& data);
        ConfigRadioTextMessageInbox(const time_t timestamp,const unsigned long oper,const char isread,const std::string& msgtext,const std::string& source, const std::string& recipient);

		virtual ~ConfigRadioTextMessageInbox();

        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey();

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

		virtual std::string getRecipientConsole();

        virtual void setOperator(unsigned long operatorId);
        virtual void setTimeStamp(time_t pTime);
        virtual void setRead(bool pRead);
        virtual void setMessageText(const std::string& messageText);
		virtual void setSourceAlias(const std::string& source);
		virtual void setRecipientConsole(const std::string& recipient);

        virtual void invalidate();
        virtual void save();

    private:
        // disabled methods
        ConfigRadioTextMessageInbox(const ConfigRadioTextMessageInbox&);
		ConfigRadioTextMessageInbox& operator=(const ConfigRadioTextMessageInbox&);

        RadioTextMessageInboxHelper m_radioHelper;
    };
}

#endif // !defined(AFX_CONFIGRADIOTEXTMESSAGEINBOX_H__438DBFDB_97D8_467D_B120_9234DCD5FBAF__INCLUDED_)
