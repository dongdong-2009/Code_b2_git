/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventSubject.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface of the event subject
  */


#ifndef _SDS_EventSubject_H
#define _SDS_EventSubject_H


namespace TA_IRS_App 
{
    class SDSEventObserver;
    class SDSEvent;

	class SDSEventSubject
	{
		public:
            virtual ~SDSEventSubject() {};
		
		
			virtual void registerObserver( SDSEventObserver* evtOb) = 0;
			virtual void deregisterObserver( SDSEventObserver* evtOb) = 0;
	};
};

	
#endif //_SDS_EventSubject_H
