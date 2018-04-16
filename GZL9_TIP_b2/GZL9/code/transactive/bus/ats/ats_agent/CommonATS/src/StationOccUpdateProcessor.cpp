/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\StationOccUpdateProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is a thread object responsible for calling the station SIG Agent for PA data synchronization
  */

#include "bus/ats/ats_agent/CommonATS/src/StationOccUpdateProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/CorbaException.h"

namespace TA_IRS_Bus
{
	StationOccUpdateProcessor::StationOccUpdateProcessor(TA_Base_Core::CorbaName& corbaName, unsigned long stationId)
		: m_corbaName(corbaName),
		  m_AtsAgent(NULL),
		  m_keepRunning(true),
		  m_semaphore(0),
		  m_stationId(stationId)
	{
		FUNCTION_ENTRY("StationOccUpdateProcessor");

		initialize();
		start();

		FUNCTION_EXIT;
	}

	StationOccUpdateProcessor::~StationOccUpdateProcessor()
	{
		FUNCTION_ENTRY("~StationOccUpdateProcessor");

		deInitialize();

		FUNCTION_EXIT;
	}

	void StationOccUpdateProcessor::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		terminateAndWait();
		if (NULL != m_AtsAgent)
		{
			delete m_AtsAgent;
			m_AtsAgent = NULL;
		}

		FUNCTION_EXIT;
	}

	void StationOccUpdateProcessor::initialize()
	{
		FUNCTION_ENTRY("initialize");

		m_AtsAgent = new OccUpdateProcessor::AtsAgentNamedObject;
		m_AtsAgent->setCorbaName(m_corbaName);
		// Set CORBA Timeout to 2sec. 
		m_AtsAgent->setObjectTimeout(2); 

		FUNCTION_EXIT;
	}

	unsigned long StationOccUpdateProcessor::getStationId()
	{
		FUNCTION_ENTRY("getStationId");
		FUNCTION_EXIT;
		return m_stationId;
	}

	void StationOccUpdateProcessor::addPasUpdate(const OccUpdateProcessor::PasMessage& pasMessage)
	{
		FUNCTION_ENTRY("addPasUpdate");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Add Pas Update Message for Station Id %d",m_stationId);	

		TA_Base_Core::ThreadGuard guard(m_pasUpdates.lock);

		m_pasUpdates.messages.push_back(pasMessage);

		FUNCTION_EXIT;
	}

	void StationOccUpdateProcessor::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_keepRunning = false;
		m_semaphore.post();

		FUNCTION_EXIT;
	}

	void StationOccUpdateProcessor::processUpdates()
	{
		m_semaphore.post();
	}

	void StationOccUpdateProcessor::run()
	{
		while(m_keepRunning)
		{
			m_semaphore.wait();

			{
				TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* pasList = new TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList;

				{
					TA_Base_Core::ThreadGuard guard(m_pasUpdates.lock);

					pasList->length(m_pasUpdates.messages.size());

					OccUpdateProcessor::PasMessageVector::iterator evIt;
					unsigned int i = 0;

					for (evIt = m_pasUpdates.messages.begin(), i = 0; evIt != m_pasUpdates.messages.end(); evIt++, i++)
					{
						(*pasList)[i] = *evIt;
					}

					m_pasUpdates.messages.clear();
				}

				try
				{
					if ( pasList->length() != 0)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"sendOccUpdates() - sending OccPasUpdate to %s, %d records", 
							m_corbaName.getObjectName().c_str(), pasList->length());
						CORBA_CALL( (*m_AtsAgent),
							processOccPasUpdate,
							( *pasList ) );
					}
				}
				catch( const TA_Base_Core::ObjectResolutionException& e )
				{
					std::string text = "sendOccUpdates() [PAS update] - " 
						+ std::string(e.what());
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
				}
				catch( const TA_Base_Core::OperationModeException& e )
				{
					std::string text = "sendOccUpdates() [PAS update] - " 
						+ std::string(e.reason);
					LOG_EXCEPTION_CATCH( SourceInfo, "OperationModeException", text );
				}
				catch( const CORBA::Exception& e )
				{
					std::string text = "sendOccUpdates() [PAS update] - " 
						+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
					LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
				}
				catch ( ...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
						"sendOccUpdates() [PAS update]");
				}

				delete pasList;
				pasList = NULL;
			}


		}
	}
}