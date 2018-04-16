/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventObserver.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface for event observers
  */


#ifndef __SDS_EventObserver_H
#define __SDS_EventObserver_H

namespace TA_IRS_App 
{
    class SDSEvent;

	class SDSEventObserver
	{
		public:
            virtual ~SDSEventObserver() {}
		
			virtual void notify(SDSEvent* sdsEvt) = 0;
			virtual void setUnregistrationState() = 0;
	};
};

	
#endif //__SDS_EventObserver_H
