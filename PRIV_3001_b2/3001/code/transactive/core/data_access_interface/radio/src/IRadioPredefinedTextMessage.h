/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Used to persist operator defined message templates
  */

#if !defined(IRadioPredefinedTextMessage_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioPredefinedTextMessage_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include <string>

namespace TA_IRS_Core
{
    class IRadioPredefinedTextMessage 
    {
    public:

	    /**
	      * ~IRadioPredefinedTextMessage
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioPredefinedTextMessage(){};

        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey() =0;

	    /**
	      * getOperator
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperator() =0;

	    /**
	      * getShortName
	      * 
	      * @return std::string
	      */
	    virtual std::string getShortName() =0;

	    /**
	      * getMessageText
	      * 
	      * @return std::string
	      */
	    virtual std::string getMessageText() =0;
		
        /** 
          * getType
          *
          * description, get the type of the predefined message.
          * TD14312
          * 
          * @return 
          */
		virtual bool getType() =0;
    };
}
#endif // !defined(IRadioPredefinedTextMessage_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
