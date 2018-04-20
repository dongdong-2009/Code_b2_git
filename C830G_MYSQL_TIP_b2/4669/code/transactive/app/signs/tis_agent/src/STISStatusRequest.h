#if !defined(STISSTATUSREQUEST_H)
#define STISSTATUSREQUEST_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISStatusRequest.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
    };

};

#endif // STATUSREQUEST_H

