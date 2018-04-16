/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSUnregisterSession.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef __SDS_UnregisterSession_H
#define __SDS_UnregisterSession_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h"	

namespace TA_IRS_App 
{
    class SDTSTransactionManager;
    class SDSQueueManager;

	class SDSUnregisterSession : public SDSFunction
	{
		public:

			SDSUnregisterSession(SDTSTransactionManager& transactionManager, 
                                 SDSQueueManager& queueManager);

			virtual ~SDSUnregisterSession();
			
			virtual int call();
            
        private:
            // disabled method
            SDSUnregisterSession(const SDSUnregisterSession&);
            void operator=(const SDSUnregisterSession&);

            SDTSTransactionManager& m_transactionManager;
            SDSQueueManager& m_queueManager;
	};

};
	

#endif //__SDS_UnregisterSession_H
