
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSManager.h"
#include "core/utilities/src/DebugUtil.h"
//#define STATUS_FAILED   1
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSMessageCreator.h"

namespace TA_IRS_App
{
	
	const unsigned long RadioPDSManager::STATUS_FAILED = 1;

	RadioPDSManagerException::RadioPDSManagerException(const std::string &errorText) throw ()
		: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY( "RadioPDSManagerException" );
		FUNCTION_EXIT;
	}

	RadioPDSManagerException::~RadioPDSManagerException() throw()
	{
		FUNCTION_ENTRY( "~RadioPDSManagerException" );
		FUNCTION_EXIT;
	}
	
	RadioPDSManager::RadioPDSManager(std::string strHost, std::string strPort, const TA_IRS_App::CommonTypes::SRadioPDSTimer& radioPDSTimer,IRadioPDSStatusCallback * radioPDSResCallback )
		: m_running( false ),
		 m_radioPDSTimer(radioPDSTimer),
		 m_radioResultCallback (radioPDSResCallback)
	{
		FUNCTION_ENTRY( "RadioPDSManager" );
		
		if (strHost.empty() || strPort.empty())
		{
			TA_THROW (CPDSDatagramServerException("Empty host or port number"));
		}
		else
		{
			// server here
			m_pdsDatagramServer = new CPDSDatagramServer (strHost,strPort,this);
		}

		FUNCTION_EXIT;
	}

	RadioPDSManager::~RadioPDSManager()
	{
		FUNCTION_ENTRY("~RadioPDSManager()");

		// delete everything
		// everything should be stopped
		TA_ASSERT(m_running == false, "Cannot stop this without shutting it down");

		// there should be no need to delete anything
		// it should be all cleaned up
		TA_ASSERT(m_mapTranferRef.size() == 0, "The thread has been shut down but transfers remain");
		TA_ASSERT(m_removeList.size() == 0, "The thread has been shut down but transfers remain to be removed");
	
		sleep(500);
		terminateAndWait();
		if (m_pdsDatagramServer)
		{
			delete m_pdsDatagramServer;
			m_pdsDatagramServer = NULL;
		}


		FUNCTION_EXIT;
	}

	void RadioPDSManager::makeDataCall( const long & lTrainID, const TA_IRS_App::CommonTypes::RadioPDSMessage& data )
	{
		FUNCTION_ENTRY( "makeDataCall" );
		
		if ( false == m_running )
		{
			// no more can be started
			LOG_GENERIC( SourceInfo,
				TA_Base_Core::DebugUtil::DebugError,
				"New PDS Message transfer for destination: %d rejected, agent starting up or shutting down",
				lTrainID );
			TA_THROW( RadioPDSManagerException("Data transfer can not be started. Agent in startup or shutdown."));
		}

		// aquire lock
		TA_THREADGUARD(m_transferListLock);
			
		// check for existing transfers
		std::map< long, STransferProc* >::iterator findIter = m_mapTranferRef.find( lTrainID );

		if ( findIter == m_mapTranferRef.end() )
		{
			// Log Incoming call for feature reference
			LOG_GENERIC ( SourceInfo,
						  TA_Base_Core::DebugUtil::DebugInfo,
						  "Request transfer (call), from TrainID : %d",
						  lTrainID
						);
			
			//add PDS Transfer
			try
			{
				CRadioPDSSender * pdsSender = new CRadioPDSSender (m_pdsDatagramServer);			
				CRadioPDSHandler* objRadioHandler  = new CRadioPDSHandler(m_pdsDatagramServer,(IRadioPDSSender *)pdsSender, this, data, lTrainID, m_radioPDSTimer );
				
				pdsSender->attach(objRadioHandler);
				m_mapTranferRef[ lTrainID ] = new STransferProc( (IRadioPDSSender *)pdsSender, objRadioHandler ); 
				m_pdsDatagramServer->setCallingTrain (lTrainID);
			}
			catch (...)
			{
				TA_THROW(RadioPDSManagerException("Invalid pointer is pass as one of the arguments"));
			}	
		}
		else
		{
			// log an error and throw exception
			LOG_GENERIC( SourceInfo,
				TA_Base_Core::DebugUtil::DebugError,
				"New PDS Message transfer for trainID: %d rejected, existing transfer already in progress",
				lTrainID );

			 TA_THROW(RadioPDSManagerException("Existing data transfer already in progress"));
		}

		FUNCTION_EXIT;
	}

	void RadioPDSManager::cancelDataCall( const long & lTrainID )
	{
		FUNCTION_ENTRY( "cancelDataCall" );

		// aquire lock
		TA_THREADGUARD(m_transferListLock);

		//Before cancelling make sure that the trainITsi is exist in the map else throw an exception
		std::map< long , STransferProc*>::iterator findIter = m_mapTranferRef.find( lTrainID );

		if ( findIter != m_mapTranferRef.end() )
		{
			// Log call cancellation for feature reference
			LOG_GENERIC (	SourceInfo,
							TA_Base_Core::DebugUtil::DebugInfo,
							"Cancelling transfer (call), from TrainID : %d",
							lTrainID
						);
			findIter->second->radioPDSHandler->cancelPDS();
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Failed to cancelDataCall trainID : %d which is not exist in the map", lTrainID );

			TA_THROW( RadioPDSManagerException( "CancelDataCall is not permitted since the specified trainID is not exist in the map" ) );
		}

		FUNCTION_EXIT;
	}

	void RadioPDSManager::endDataCall( const long & lTrainID )
	{
		FUNCTION_ENTRY( "endDataCall" );

		//Terminate the DataCall.
		removePDSHandlerObject( lTrainID );

		FUNCTION_EXIT;
	}

	void RadioPDSManager::processEvent( TA_IRS_App::IPDSEvent * event )
	{
		FUNCTION_ENTRY( "processEvent" );
		
		// TA_IRS_App::RadioPDSEvent& radioPdsEvent = dynamic_cast<TA_IRS_App::RadioPDSEvent&>(*event);
		TA_IRS_App::RadioPDSEvent * radioPdsEvent = (TA_IRS_App::RadioPDSEvent *)(event);

		std::map< long, STransferProc* >::iterator findIter = m_mapTranferRef.find( radioPdsEvent->getTrainId());
		
		
		if ( findIter != m_mapTranferRef.end() )
		{
			// Log call disconnection for feature reference
			LOG_GENERIC (	SourceInfo,
							TA_Base_Core::DebugUtil::DebugInfo,
							"Received event trainID: %d", radioPdsEvent->getTrainId()
						);
			
			findIter->second->radioPDSHandler->SendPDS(event);
		}

		FUNCTION_EXIT;
	}

	void RadioPDSManager::NotifyTTSStatus( const long & lTrainID, bool success, const std::string& failReason ) // send
	{
		FUNCTION_ENTRY( "NotifyTTSStatus" );

		//TODO: Notify if there is a cancel or failed then delete the object.
		LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"\t\t\n== NotifyTTSStatus =="
			"\t\t\nTrain ID   : %d"
			"\t\t\nStatus      : %d"
			"\t\t\nFail Reason : %s", lTrainID, success, failReason.c_str() );

		endDataCall(lTrainID);
		
		// callback to radioServerAgentServant

		TA_IRS_App::TtisTypes::SPDSDownloadDataResults sDownloadResult;
		
		sDownloadResult.bStatus = success;
		sDownloadResult.lTrainID = lTrainID;
		sDownloadResult.strReason = failReason;
		
		m_radioResultCallback->notifyPDSDownloadStatus(sDownloadResult);
		
		FUNCTION_EXIT;
	}

	void RadioPDSManager::removePDSHandlerObject( const long & lTrainID )
	{
		FUNCTION_ENTRY( "removePDSHandlerObject" );

		//TODO: Notify if there is a cancel or failed then delete the object.
		{
			TA_THREADGUARD( m_removeListLock );
			
			// queue the transfer for deleting
			m_removeList.push(lTrainID);
		}

		m_semaphore.post();
		FUNCTION_EXIT;

	}

	void RadioPDSManager::run()
	{

		FUNCTION_ENTRY( "run()" );
		
		{
			TA_THREADGUARD( m_removeListLock );
			m_running = true;
		}
		

		// while not shutting down
		// also keep running while any transfers are closing down
		while ( m_running || m_mapTranferRef.size() > 0 )
		{
			// process those to be deleted
			{
				TA_THREADGUARD( m_removeListLock );

				while ( false == m_removeList.empty() )
				{
					//get the next one to remove
					long lTrainID = m_removeList.front();
					m_removeList.pop();


					// find the transfer in the maps and remove it
					{
						TA_THREADGUARD( m_transferListLock )

						// delete from the TSI map (the master one)
						std::map< long, STransferProc* >::iterator findIter = m_mapTranferRef.find( lTrainID );
						if ( findIter != m_mapTranferRef.end() )
						{
							
							LOG_GENERIC (SourceInfo,
										TA_Base_Core::DebugUtil::DebugInfo,
										"Removing %d trainID record - Size : %d",
										 lTrainID,
										 m_mapTranferRef.size ());

							delete (*findIter).second;
							findIter->second = NULL;
							m_mapTranferRef.erase( findIter );

							LOG_GENERIC (SourceInfo,
								TA_Base_Core::DebugUtil::DebugInfo,
								"Removed %d trainID record - Size : %d",
								lTrainID,
								m_mapTranferRef.size ());
						}

					}

				}

			}
			// wait on semaphore again
			m_semaphore.wait();
		}
	
		FUNCTION_EXIT;
	}

	void RadioPDSManager::terminate()
	{
		FUNCTION_ENTRY( "terminate()" );
		
		// If terminate is called then
		// cancel all transfers
		// let them shut down cleanly
		{
			TA_THREADGUARD( m_transferListLock );
			for ( std::map< long, STransferProc* >::iterator iter = m_mapTranferRef.begin();
				iter != m_mapTranferRef.end(); iter++ )
			{
				iter->second->radioPDSHandler->cancelPDS();
			}
		}

		m_running = false;
		m_semaphore.post();

		FUNCTION_EXIT;
	}

	bool RadioPDSManager::IsRunning()
	{
		TA_THREADGUARD( m_removeListLock );
		return m_running;	
	}

	void RadioPDSManager::NotifyEndOfTtisDownloadStatus( const unsigned long& lTrainID, unsigned short& sStatus )
	{
		FUNCTION_ENTRY( "NotifyEndOfTtisDownloadStatus" );
		
		std::map< long, STransferProc* >::iterator findIter;
		
		{
			LOG_GENERIC (	SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"Thread Guard Resource"
				);

			TA_THREADGUARD (m_transferListLock);
			findIter = m_mapTranferRef.find( lTrainID );
		}
		
		if ( findIter != m_mapTranferRef.end() )
		{
			// Log call disconnection for feature reference
			LOG_GENERIC (	SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"EndofTtisDownload TrainID : %d, Status %d", lTrainID, sStatus
				);

			//Check the status if its equal to STATUS_FAILED
			if ( sStatus == STATUS_FAILED)
			{
				if (findIter->second)
				{
					findIter->second->radioPDSHandler->callDisconnection();
				}
				
			}
			
		}

		FUNCTION_EXIT;
	}

}



