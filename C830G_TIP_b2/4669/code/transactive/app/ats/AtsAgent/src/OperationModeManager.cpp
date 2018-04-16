/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Darren Sampson
  * @version:  $Revision$
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime$
  * Last mofified by:  $Author$
  *
  * This implements data relay for the OCC Agent
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/ats/AtsAgent/src/OperationModeManager.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"
#include "app/ats/AtsAgent/src/DoubleAtsFailureWorkItem.h"


namespace TA_IRS_App
{

        OperationModeManager::OperationModeManager(TA_Base_Core::AtsAgentEntityDataPtr entityData) :
			m_controlMode (OperationModeManager::UnknownMode),
			DEFAULT_CONTROL_MODE (OperationModeManager::MonitorMode),
			m_iscsDegraded (OperationModeManager::UnknownDegraded),
			DEFAULT_ISCS_DEGRADED (OperationModeManager::IscsNormal),
			m_radioFallback (OperationModeManager::UnknownFallback),
			DEFAULT_RADIO_FALLBACK (OperationModeManager::RadioNormal),
			m_occAtsState (OperationModeManager::Startup),
			m_locationKey (entityData->getLocation()),
			m_locationType (entityData->getAgentAtsLocationType()),
			m_keepRunning (true),
			m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance())
		{
            FUNCTION_ENTRY( "OperationModeManager" );

			TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
					  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot ||
					  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
					  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
					  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
					  "Not a known location");

			// Set degraded mode Run Param
			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
				m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				// Needs some sort of exception handling
				initialiseTrainAgentNamedObject();

				// Get the offline group
				std::string offlineGroups = TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);

				// If group is not online
				if( (!offlineGroups.empty()) &&
					(offlineGroups.compare(TA_Base_Bus::GenericAgent::GROUP_ONLINE) != 0) )
				{
					inIscsDegraded(true);
				}
				else
				{
					inIscsDegraded(false);
				}

				setOccAtsState(OperationModeManager::Startup);

				start();
			}
			else
			{
				TA_THREADGUARD(m_lock);

				m_iscsDegraded = IscsNormal;
				m_occAtsState = Normal;
				m_radioFallback = RadioNormal;
			}

            FUNCTION_EXIT;
		}


        OperationModeManager::~OperationModeManager()
        {
            FUNCTION_ENTRY( "~OperationModeManager" );

			// clean up the thread pool
			TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
			m_threadPool = NULL;

            if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ &&
                m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
            {
                terminateAndWait();
            }

            FUNCTION_EXIT;
        }


		void OperationModeManager::inControlMode(bool controlMode)
		{
            FUNCTION_ENTRY( "inControlMode" );

			TA_THREADGUARD(m_lock);

			if (m_controlMode != (controlMode?ControlMode:MonitorMode))
			{
				m_controlMode = (controlMode?ControlMode:MonitorMode);

				if (controlMode == true)
				{
					setOccAtsState(Startup);

					m_radioFallback = UnknownFallback;
				}
			}		
            FUNCTION_EXIT;
		}

		bool OperationModeManager::inControlMode()
		{
			TA_THREADGUARD(m_lock);

			return ((m_controlMode == ControlMode) || 
					(m_controlMode == UnknownMode && DEFAULT_CONTROL_MODE == ControlMode));
		}

        void OperationModeManager::inIscsDegraded(bool iscsDegraded)
        {
            FUNCTION_ENTRY( "inIscsDegraded" );

			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				TA_THREADGUARD(m_lock);

				if (m_iscsDegraded == UnknownDegraded)
				{
					if (iscsDegraded == true)
					{
						m_iscsDegraded = IscsDegraded;

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ATS Agent initialising ISCS Degraded Mode - degraded");
					}
					else
					{
						m_iscsDegraded = IscsNormal;

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ATS Agent initialising ISCS Degraded Mode - not degraded");
					}
				}
				else
				{
					if (iscsDegraded == true)
					{
						if (m_iscsDegraded != IscsDegraded)
						{
							m_iscsDegraded = IscsDegraded;

							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ATS Agent transitioning to ISCS Degraded Mode");
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ATS Agent is already in ISCS Degraded Mode");
						}
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ISCS Degraded Mode ended - Can not exit until Duty changes");
					}
				}
            }

            FUNCTION_EXIT;
        }


        bool OperationModeManager::inIscsDegraded()
        {
            FUNCTION_ENTRY( "inIscsDegraded" );

            TA_THREADGUARD(m_lock);

            FUNCTION_EXIT;
            return ((m_iscsDegraded == IscsDegraded) ||
                    (m_iscsDegraded == UnknownDegraded && DEFAULT_ISCS_DEGRADED == IscsDegraded));
        }


        void OperationModeManager::inRadioFallback(bool radioFallback)
        {
            FUNCTION_ENTRY( "inRadioFallback" );

            if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ &&
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				TA_THREADGUARD(m_lock);

				if (m_radioFallback == UnknownFallback)
				{
					if (radioFallback == true)
					{
						m_radioFallback = RadioFallback;

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ATS Agent initialising Radio Fallback Mode - in fallback");
					}
					else
					{
						m_radioFallback = RadioNormal;
						setOccAtsState(Startup);

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ATS Agent initialising Radio Fallback Mode - not in fallback");
					}
				}
				else
				{
					if (radioFallback == true)
					{
						if (m_radioFallback != RadioFallback)
						{
							m_radioFallback = RadioFallback;

							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ATS Agent transitioning to Radio Fallback Mode");
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ATS Agent is already in Radio Fallback Mode");
						}
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"Radio Fallback Mode ended - Can not exit until Duty changes");
					}
				}
			}

            FUNCTION_EXIT;
        }


        bool OperationModeManager::inRadioFallback()
        {
            FUNCTION_ENTRY( "inRadioFallback" );

            TA_THREADGUARD(m_lock);

            FUNCTION_EXIT;
            return ((m_radioFallback == RadioFallback) ||
                    (m_radioFallback == UnknownFallback && DEFAULT_RADIO_FALLBACK == RadioFallback));
        }


		void OperationModeManager::setOccAtsState(OperationModeManager::EOccAtsState occAtsState)
        {
            FUNCTION_ENTRY( "setOccAtsState" );

			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				switch (m_occAtsState)
				{
				case Startup:
				case Normal:
					switch (occAtsState)
					{
					case Startup:
					case Normal:
						m_occAtsState = occAtsState;
						break;

					case DoubleFailure:
						if (inControlMode())
						{
							m_occAtsState = DoubleFailure;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"ATS Agent transitioning to OCC Double Ats Failure Mode");

							{
								//bool setOccDoubleFailureState = false;

								//while (setOccDoubleFailureState == false)
								//{
								//	try
								//	{
								TA_Base_Core::IWorkItemPtr newWorkItem( new DoubleAtsFailureWorkItem(m_trainAgent, true));

								m_threadPool->queueWorkItem( newWorkItem );
										

										//setOccDoubleFailureState = true;
                                   /* }
                                    catch (const TA_IRS_Bus::AgentCommunicationException& e)
                                    {
                                        std::string text = "OperationModeManager::run() - "
                                            + std::string(e.what());
                                        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
                                    }
									catch (...)
									{
									}

									if (setOccDoubleFailureState == false)
									{
										TA_Base_Core::Thread::sleep(1000);
									}
								}*/
							}
						}
						break;
					}
					break;

				case DoubleFailure:
					switch (occAtsState)
					{
					case Startup:
	                    m_occAtsState = occAtsState;
	                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"OCC Double Ats Failure Mode ended");
	                    break;
					case Normal:
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"OCC Double Ats Failure Mode ended - Can not exit until Duty changes");
						break;

					case DoubleFailure:
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ATS Agent is already in OCC Double Ats Failure Mode");
						break;
					}
					break;
                }
            }

            FUNCTION_EXIT;
        }


        bool OperationModeManager::inDoubleAtsFailure()
        {
            FUNCTION_ENTRY( "inDoubleAtsFailure" );
            FUNCTION_EXIT;
            return m_occAtsState == DoubleFailure;
        }


        bool OperationModeManager::inOccAtsStartup()
        {
            FUNCTION_ENTRY( "inOccAtsStartup" );
            FUNCTION_EXIT;
            return m_occAtsState == Startup;
        }


        void OperationModeManager::resetAts()
        {
            FUNCTION_ENTRY( "resetAts" );

			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				TA_THREADGUARD(m_lock);

				if (inDoubleAtsFailure())
				{
					setOccAtsState(Startup);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"OCC Double Ats Failure Mode ended - reset");
				}

				if (inRadioFallback())
				{
					m_radioFallback = RadioNormal;
					setOccAtsState(Startup);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"Radio Fallback Mode ended - reset");
				}

				if (inIscsDegraded())
				{
					m_iscsDegraded = IscsNormal;
					setOccAtsState(Startup);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"ISCS Degraded Mode ended - reset");
				}
			}

            FUNCTION_EXIT;
        }


		void OperationModeManager::initialiseTrainAgentNamedObject()
        {
            FUNCTION_ENTRY( "initialiseTrainAgentNamedObject" );

            m_trainAgent = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainOperationModeCorbaProxy();

            FUNCTION_EXIT;
        }


		OperationModeManager::EAgentState OperationModeManager::getAgentState()
        {
            FUNCTION_ENTRY( "getAgentState" );

			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				if (inIscsDegraded())
				{
                    FUNCTION_EXIT;
					return AgentIscsDegraded;
				}
				else if (inDoubleAtsFailure())
				{
                    FUNCTION_EXIT;
					return AgentOccFailure;
				}
				else if (inRadioFallback())
				{
                    FUNCTION_EXIT;
					return AgentRadioFallback;
				}
            }

            FUNCTION_EXIT;
            return AgentNormal;
		}


		void OperationModeManager::run()
        {
            FUNCTION_ENTRY( "run" );

			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			    m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				while (m_keepRunning)
				{
					bool radioFallbackUnknown = false;

					{
						TA_THREADGUARD(m_lock);

						radioFallbackUnknown = (m_radioFallback == UnknownFallback);
					}
						
					if (radioFallbackUnknown)
					{
						try
						{
							bool radioFallback = m_trainAgent->radioInFallback();
							
							{
								TA_THREADGUARD(m_lock);
							
								if (m_radioFallback == UnknownFallback)
								{
									inRadioFallback(radioFallback);
								}
							}
						}
	                    catch( const TA_Base_Core::TransactiveException& e )
						{
							std::string text = "OperationModeManager::run() - " 
												+ std::string(e.what());
							LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", text );
						}
						catch( const TA_Base_Core::OperationModeException& e )
						{
							std::string text = "OperationModeManager::run() - " 
												+ std::string(e.reason);
							LOG_EXCEPTION_CATCH( SourceInfo, "OperationModeException", text );
						}
						catch( const CORBA::Exception& e )
						{
							std::string text = "OperationModeManager::run() - " 
												+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
							LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
						}
						catch ( ...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"OperationModeManager::run()");
						}
					}

					if (m_keepRunning)
					{
						TA_Base_Core::Thread::sleep(1000);
					}
				}
			}

            FUNCTION_EXIT;
        }


		void OperationModeManager::terminate()
        {
            FUNCTION_ENTRY( "terminate" );

            m_keepRunning = false;

            FUNCTION_EXIT;
        }


}
