/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckRecvText.h $
  * @author:  Alexis Laredo      
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef __SDS_AckRecvText_H
#define __SDS_AckRecvText_H

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h"	
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"	

namespace TA_IRS_App {
    class SDTSTransactionManager;
    class SDSQueueManager;

	class SDSAckRecvText : public SDSFunction
	{
		public:
			SDSAckRecvText(SDTSTransactionManager& transactionManager, 
                           SDSQueueManager& queueManager,
                           unsigned int srcSsi, unsigned int destSsi, CMProtocolIdentifier pi, unsigned char msgRef);

			virtual ~SDSAckRecvText();
			
			virtual int call();

            void setDeliveryStatus(TLDeliveryStatus status);
			void setCMMsgRef(unsigned char ref);
		private:
            // disabled methods
            SDSAckRecvText();
            SDSAckRecvText(const SDSAckRecvText&);
            void operator=(const SDSAckRecvText&);

        private:
            unsigned int m_srcSsi;
            unsigned int m_destSsi;
            CMProtocolIdentifier m_pi;
            TLDeliveryStatus m_status;
			unsigned char m_cmMsgRef;

            SDTSTransactionManager& m_transactionManager;
            SDSQueueManager& m_queueManager;

	};
};


#endif //__SDS_AckRecvText_H
