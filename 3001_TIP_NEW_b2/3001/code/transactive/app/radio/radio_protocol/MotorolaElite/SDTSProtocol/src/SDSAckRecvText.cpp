/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckRecvText.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckRecvText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"

namespace TA_IRS_App {

	SDSAckRecvText::SDSAckRecvText(SDTSTransactionManager& transactionManager, 
                                   SDSQueueManager& queueManager,
                                   unsigned int srcSsi, unsigned int destSsi, CMProtocolIdentifier pi, unsigned char msgRef) :
                    m_transactionManager(transactionManager), m_queueManager(queueManager),
                    m_srcSsi(srcSsi), m_destSsi(destSsi), m_pi(pi), m_status(TLDELIVERYSTATUS_RECEIVED),m_cmMsgRef(msgRef)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDSAckRecvText::~SDSAckRecvText()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}

	int	SDSAckRecvText::call()
	{
		FUNCTION_ENTRY( "call" );

		try
		{
			if(m_cmMsgRef == 0)
			{
				// begin transaction
				m_cmMsgRef = m_transactionManager.beginTransaction(m_srcSsi, m_destSsi, m_pi);
			}

            TL_ReportEvent msg;
            CM_Data* cmData = new CM_Data();

            TA_ASSERT(cmData != NULL, "Cannot allocate memory");

            cmData->setSourceSSI(m_srcSsi);
            cmData->setDestSSI(m_destSsi);
            cmData->setMessageRef(m_cmMsgRef);
            cmData->setProtocolId(m_pi);
            msg.setCMHeader(cmData);
            msg.setDeliveryStatus(m_status);
			
			// send message 
			m_queueManager.sendMessage(msg.getRawMessage());
		}
		catch(TA_Base_Core::TransactiveException e)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			return -1;
		}

		FUNCTION_EXIT;

		return 1;
	}

    void SDSAckRecvText::setDeliveryStatus(TLDeliveryStatus status)
    {
		FUNCTION_ENTRY( "setDeliveryStatus" );
        m_status = status;
		FUNCTION_EXIT;
    }

	void SDSAckRecvText::setCMMsgRef(unsigned char ref)
    {
		FUNCTION_ENTRY( "setCMMsgRef" );
        m_cmMsgRef = ref;
		FUNCTION_EXIT;
    }	
}
