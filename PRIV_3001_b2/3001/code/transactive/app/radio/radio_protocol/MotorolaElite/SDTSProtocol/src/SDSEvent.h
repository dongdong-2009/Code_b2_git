/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef __SDS_Event_H
#define __SDS_Event_H

#include "ace/Method_Request.h"

namespace TA_IRS_App 
{
    class SDTSProtocolEvent;

	class SDSEvent : public ACE_Method_Request
	{
		public:
			SDSEvent();

			virtual ~SDSEvent();
			
			virtual int call() = 0;

            SDTSProtocolEvent* getProtocolEvent();

            void setProtocolEvent(SDTSProtocolEvent* event);

        private:
            // disabled methods
            SDSEvent(const SDSEvent&);
            void operator=(const SDSEvent&);
            
		private:

	        /**
	          * the protocol event to be dispatched
	          */
			SDTSProtocolEvent* m_event;
	};

};


#endif // __SDS_Event_H
