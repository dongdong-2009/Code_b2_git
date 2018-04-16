/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRegisterSession.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Builds and sends a CM_RegisterRequest
  */




#ifndef __SDS_RegisterSession_H
#define __SDS_RegisterSession_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"


namespace TA_IRS_App 
{
    class SDTSTransactionManager;
    class SDSQueueManager;

	class SDSRegisterSession : public virtual SDSFunction
	{
		public:

            SDSRegisterSession(SDTSTransactionManager& transactionManager, 
                               SDSQueueManager& queueManager,
                               const std::string& host, const std::string& port,
                               unsigned long passNumber, unsigned long clientSSI, CMRegistrationType regType);

			virtual ~SDSRegisterSession();

			virtual int call();

	
		private:
            std::string m_host;
            std::string m_port;
            
            unsigned long m_passNumber;
            unsigned long m_clientSSI;
            CMRegistrationType m_regType;

            SDTSTransactionManager& m_transactionManager;
            SDSQueueManager& m_queueManager;
    };

};

	
#endif //__SDS_RegisterSession_H
