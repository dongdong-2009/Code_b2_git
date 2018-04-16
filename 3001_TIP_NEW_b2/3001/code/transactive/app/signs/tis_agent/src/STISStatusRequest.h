#if !defined(STISSTATUSREQUEST_H)
#define STISSTATUSREQUEST_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISStatusRequest.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This abstract class represents a status request message.
  * Classes derived from this class can process the response
  * for the request in a way specific to their messages.
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"


namespace TA_IRS_App
{

    class STISStatusRequest : public STISMessage
    {

    public:

        /**
          * Constructor
          */
		STISStatusRequest()
        {
        }


        /**
          * Destructor
          */
        ~STISStatusRequest()
        {
        }


        /** 
          * processResponse
          *
          * Processes the response from the status request.
          *
          */
        virtual void processResponse() = 0;
	private:
    };

};

#endif // STATUSREQUEST_H

