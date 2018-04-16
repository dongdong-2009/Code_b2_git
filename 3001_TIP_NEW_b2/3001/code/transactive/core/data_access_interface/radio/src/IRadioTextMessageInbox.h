/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioTextMessageInbox.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * 
  */

#if !defined(IRadioTextMessageInbox_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioTextMessageInbox_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include <time.h>
#include <string>


namespace TA_IRS_Core
{
    class IRadioTextMessageInbox
    {
    public:

	    /**
	      * ~IRadioTextMessageInbox
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioTextMessageInbox(){};

        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey() =0;

	    /**
	      * getTimeStamp
	      * 
	      * @return time_t
	      */
	    virtual time_t getTimeStamp() =0;

	    /**
	      * getOperator
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperator() =0;

	    virtual bool isRead() =0;

	    /**
	      * getMessageText
	      * 
	      * @return std::string
	      */
	    virtual std::string getMessageText() =0;

		virtual std::string getSourceAlias() =0;

		virtual std::string getRecipientConsole() =0;
		
    };
}
#endif // !defined(IRadioTextMessageInbox_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
