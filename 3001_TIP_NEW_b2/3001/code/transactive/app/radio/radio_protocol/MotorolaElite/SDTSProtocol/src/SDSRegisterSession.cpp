/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRegisterSession.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Builds and sends a CM_RegisterRequest
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRegisterSession.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterRequest.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

    SDSRegisterSession::SDSRegisterSession(SDTSTransactionManager& transactionManager, 
                               SDSQueueManager& queueManager, 
                               const std::string& host, const std::string& port,
                               unsigned long passNumber, unsigned long clientSSI, CMRegistrationType regType) : 
            m_transactionManager(transactionManager), m_queueManager(queueManager),
            m_host(host), m_port(port),
            m_passNumber(passNumber), m_clientSSI(clientSSI), m_regType(regType)
    {
        FUNCTION_ENTRY( "Constructor" );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SDSRegisterSession Host: %s Port: %s Pass: %d SSI: %d",
			host.c_str(), port.c_str(), passNumber, clientSSI);
        FUNCTION_EXIT;
    }


    SDSRegisterSession::~SDSRegisterSession()
    {
        FUNCTION_ENTRY( "Destructor" );

        FUNCTION_EXIT;
    }


    int SDSRegisterSession::call()
    {
        FUNCTION_ENTRY("call");

		try
		{
            //m_queueManager.connect(m_host, m_port);

            CM_RegisterRequest req(m_passNumber, m_clientSSI, m_regType);

            m_queueManager.sendMessage(req.getRawMessage());
		}
		catch(TA_Base_Core::TransactiveException e)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			return -1;
		}
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown error");
            return -1;
        }

        return 0;
    }
}
