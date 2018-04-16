/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckSendText.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {


    SDSAckSendText::SDSAckSendText(SDTSTransactionManager& transactionManager) :
                    m_transactionManager(transactionManager)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDSAckSendText::~SDSAckSendText()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}

	int	SDSAckSendText::call()
	{
		FUNCTION_ENTRY( "call" );

        SDTSProtocolEvent* event = getProtocolEvent();

        if (event != NULL)
        {
            CMCommon2* common2 = dynamic_cast<CMCommon2*>(event);
            if (common2 == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid acknowledge send text event");
                return -1;
            }

		    unsigned long src = common2->getSourceSSI();
		    unsigned long dst = common2->getDestSSI();
		    unsigned char msgRef = common2->getMessageRef();
            CMProtocolIdentifier pi = common2->getProtocolId();

		    m_transactionManager.endTransaction(src, dst, pi, msgRef);
        }

		FUNCTION_EXIT;

		return 0;
	}
}
