/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/StationOccUpdateProcessor.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2009/01/02 14:52:15 $
  * Last mofified by:  $Author: builder $
  *
  * This implements data relay for the OCC Agent
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "app/ats/AtsAgent/src/StationOccUpdateProcessor.h"

namespace TA_IRS_App
{
	StationOccUpdateProcessor::StationOccUpdateProcessor(TA_Base_Core::CorbaName& corbaName, WatchDogData& watchDogData) :
	m_corbaName (corbaName), m_atsAgent (NULL), m_keepRunning(true), m_semaphore(0), m_watchDogData (watchDogData)
	{
		m_atsAgent = new OccUpdateProcessor::AtsAgentNamedObject;

		if (m_atsAgent != NULL)
		{
			m_atsAgent->setCorbaName(corbaName);
			m_atsAgent->setObjectTimeout(5);
		}

		start();
	}


	StationOccUpdateProcessor::~StationOccUpdateProcessor()
	{
		terminateAndWait();

		delete m_atsAgent;
		m_atsAgent = NULL;
	}

   	//
	// ECS
	//

	void StationOccUpdateProcessor::addEcsUpdate(const OccUpdateProcessor::EcsMessage& ecsMessage)
	{
		TA_Base_Core::ThreadGuard guard(m_ecsUpdates.lock);

		m_ecsUpdates.messages.push_back(ecsMessage);
	}

	//
	// Train
	//

	void StationOccUpdateProcessor::addTrainUpdate(const OccUpdateProcessor::TrainMessage& trainMessage)
	{
		TA_Base_Core::ThreadGuard guard(m_trainUpdates.lock);

		m_trainUpdates.messages.push_back(trainMessage);
	}

	//
	//	CCTV
	//

	void StationOccUpdateProcessor::addCctvUpdate(const OccUpdateProcessor::CctvMessage& cctvMessage)
	{
		TA_Base_Core::ThreadGuard guard(m_cctvUpdates.lock);

		m_cctvUpdates.messages.push_back(cctvMessage);
	}

	//
	// TIS
	//

	void StationOccUpdateProcessor::addTisUpdate(const OccUpdateProcessor::TisMessage& tisMessage)
	{
		TA_Base_Core::ThreadGuard guard(m_tisUpdates.lock);

		m_tisUpdates.messages.push_back(tisMessage);
	}
	
	//
	// PAS
	//

	void StationOccUpdateProcessor::addPasUpdate(const OccUpdateProcessor::PasMessage& pasMessage)
	{
		TA_Base_Core::ThreadGuard guard(m_pasUpdates.lock);

		m_pasUpdates.messages.push_back(pasMessage);
	}

	void StationOccUpdateProcessor::processUpdates()
	{
		m_semaphore.post();
	}

	void StationOccUpdateProcessor::run()
	{
		while (m_keepRunning)
        {
			m_semaphore.wait();

			if (m_keepRunning)
			{			
				
				//
				// ECS
				//
				{
					TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList* ecsList = new TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList;

					{
						TA_Base_Core::ThreadGuard guard(m_ecsUpdates.lock);

						ecsList->length(m_ecsUpdates.messages.size());

						EcsMessageVector::iterator evIt;
						unsigned int i = 0;

						for (evIt = m_ecsUpdates.messages.begin(), i = 0; evIt != m_ecsUpdates.messages.end(); evIt++, i++)
						{
							(*ecsList)[i] = *evIt;
						}
						
						m_ecsUpdates.messages.clear();
					}

					try
					{
						if (m_watchDogData.isAtsOk() || ecsList->length() != 0)
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - sending OccEcsUpdate to %s, %d records", 
								m_corbaName.getObjectName().c_str(), ecsList->length());
							CORBA_CALL( (*m_atsAgent),
							            processOccEcsUpdate,
							            ( *ecsList ) );
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - OccEcsUpdate not sent to %s, OCC ATS down", 
								m_corbaName.getObjectName().c_str());
						}
					}
					catch( const TA_Base_Core::ObjectResolutionException& e )
					{
						std::string text = "sendOccUpdates() [ECS update] - " 
											+ std::string(e.what());
						//LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						//commented about line to avoid large amount of logs
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
					}
					catch( const CORBA::Exception& e )
					{
						std::string text = "sendOccUpdates() [ECS update] - " 
											+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
						LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
					}
					catch ( ...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
											"sendOccUpdates() [ECS update]");
					}

					delete ecsList;
					ecsList = NULL;
				}

				//
				// Train
				//
				{
					TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* trainList = new TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList;

					{
						TA_Base_Core::ThreadGuard guard(m_trainUpdates.lock);

						trainList->length(m_trainUpdates.messages.size());

						TrainMessageVector::iterator evIt;
						unsigned int i = 0;

						for (evIt = m_trainUpdates.messages.begin(), i = 0; evIt != m_trainUpdates.messages.end(); evIt++, i++)
						{
							(*trainList)[i] = *evIt;
						}

						m_trainUpdates.messages.clear();
					}

					try
					{
						if (m_watchDogData.isAtsOk() || trainList->length() != 0)
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - sending OccTrainUpdate to %s, %d records", 
								m_corbaName.getObjectName().c_str(), trainList->length());
							CORBA_CALL( (*m_atsAgent),
							            processOccTrainUpdate,
							            ( *trainList ) );
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - OccTrainUpdate not sent to %s, OCC ATS down", 
								m_corbaName.getObjectName().c_str());
						}
					}
					catch( const TA_Base_Core::ObjectResolutionException& e )
					{
						std::string text = "sendOccUpdates() [Train update] - " 
											+ std::string(e.what());
						//LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						//commented about line to avoid large amount of logs
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
					}
					catch( const CORBA::Exception& e )
					{
						std::string text = "sendOccUpdates() [Train update] - " 
											+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
						LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
					}
					catch ( ...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
											"sendOccUpdates() [Train update]");
					}

					delete trainList;
					trainList = NULL;
				}

				//
				//	CCTV
				//
				{
					TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList* cctvList = new TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList;
						
					{
						TA_Base_Core::ThreadGuard guard(m_cctvUpdates.lock);
						
						cctvList->length(m_cctvUpdates.messages.size());

						CctvMessageVector::iterator evIt;
						unsigned int i = 0;

						for (evIt = m_cctvUpdates.messages.begin(), i = 0; evIt != m_cctvUpdates.messages.end(); evIt++, i++)
						{
							(*cctvList)[i] = *evIt;
						}

						m_cctvUpdates.messages.clear();
					}

					try
					{
						if (m_watchDogData.isAtsOk() || cctvList->length() != 0)
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - sending OccCctvUpdate to %s, %d records", 
								m_corbaName.getObjectName().c_str(), cctvList->length());
							CORBA_CALL( (*m_atsAgent),
							            processOccCctvUpdate,
							            ( *cctvList ) );
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - OccCctvUpdate not sent to %s, OCC ATS down", 
								m_corbaName.getObjectName().c_str());
						}
					}
					catch( const TA_Base_Core::ObjectResolutionException& e )
					{
						std::string text = "sendOccUpdates() [CCTV update] - " 
											+ std::string(e.what());
						//LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						//commented about line to avoid large amount of logs
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
					}
					catch( const TA_Base_Core::OperationModeException& e )
					{
						std::string text = "sendOccUpdates() [CCTV update] - " 
											+ std::string(e.reason);
						LOG_EXCEPTION_CATCH( SourceInfo, "OperationModeException", text );
					}
					catch( const CORBA::Exception& e )
					{
						std::string text = "sendOccUpdates() [CCTV update] - " 
											+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
						LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
					}
					catch ( ...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
											"sendOccUpdates() [CCTV update]");
					}

					delete cctvList;
					cctvList = NULL;
				}

				//
				// TIS
				//
				{
					TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList* tisList = new TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList;
						
					{
						TA_Base_Core::ThreadGuard guard(m_tisUpdates.lock);

						tisList->length(m_tisUpdates.messages.size());

						TisMessageVector::iterator evIt;
						unsigned int i = 0;

						for (evIt = m_tisUpdates.messages.begin(), i = 0; evIt != m_tisUpdates.messages.end(); evIt++, i++)
						{
							(*tisList)[i] = *evIt;
						}

						m_tisUpdates.messages.clear();
					}

					try
					{
						if (m_watchDogData.isAtsOk() || tisList->length() != 0)
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - sending OccTisUpdate to %s, %d records", 
								m_corbaName.getObjectName().c_str(), tisList->length());

							CORBA_CALL( (*m_atsAgent),
							            processOccTisUpdate,
							            ( *tisList ) );
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - OccTisUpdate not sent to %s, OCC ATS down", 
								m_corbaName.getObjectName().c_str());
						}
					}
					catch( const TA_Base_Core::ObjectResolutionException& e )
					{
						std::string text = "sendOccUpdates() [TIS update] - " 
											+ std::string(e.what());
						//LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						//commented about line to avoid large amount of logs
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
					}
					catch( const TA_Base_Core::OperationModeException& e )
					{
						std::string text = "sendOccUpdates() [TIS update] - " 
											+ std::string(e.reason);
						LOG_EXCEPTION_CATCH( SourceInfo, "OperationModeException", text );
					}
					catch( const CORBA::Exception& e )
					{
						std::string text = "sendOccUpdates() [TIS update] - " 
											+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
						LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
					}
					catch ( ...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
											"sendOccUpdates() [TIS update]");
					}

					delete tisList;
					tisList = NULL;
				}
	
				//
				// PAS
				//
				{
					TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* pasList = new TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList;
						
					{
						TA_Base_Core::ThreadGuard guard(m_pasUpdates.lock);

						pasList->length(m_pasUpdates.messages.size());

						PasMessageVector::iterator evIt;
						unsigned int i = 0;

						for (evIt = m_pasUpdates.messages.begin(), i = 0; evIt != m_pasUpdates.messages.end(); evIt++, i++)
						{
							(*pasList)[i] = *evIt;
						}

						m_pasUpdates.messages.clear();
					}

					try
					{
						if (m_watchDogData.isAtsOk() || pasList->length() != 0)
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - sending OccPasUpdate to %s, %d records", 
								m_corbaName.getObjectName().c_str(), pasList->length());
							CORBA_CALL( (*m_atsAgent),
							            processOccPasUpdate,
							            ( *pasList ) );
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"sendOccUpdates() - OccPasUpdate not sent to %s, OCC ATS down", 
								m_corbaName.getObjectName().c_str());
						}
					}
					catch( const TA_Base_Core::ObjectResolutionException& e )
					{
						std::string text = "sendOccUpdates() [PAS update] - " 
											+ std::string(e.what());
						//LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						//commented about line to avoid large amount of logs
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

	void StationOccUpdateProcessor::terminate()
    {
        m_keepRunning = false;
		m_semaphore.post();
    }
}
