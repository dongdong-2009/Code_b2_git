#if !defined(STISSTATUSREQUEST_H)
#define STISSTATUSREQUEST_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISStatusRequest.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

