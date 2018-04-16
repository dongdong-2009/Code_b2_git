// PDSDatagramServer.cpp: implementation of the CPDSDatagramServer class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSDatagramServer.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{

	CPDSDatagramServerException::CPDSDatagramServerException(const std::string &errorText) throw ()
	: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY( "CPDSDatagramServerException" );
		FUNCTION_EXIT;
	}

	CPDSDatagramServerException::~CPDSDatagramServerException() throw()
	{
		FUNCTION_ENTRY( "~CPDSDatagramServerException" );
		FUNCTION_EXIT;
	}
		

	CPDSDatagramServer::CPDSDatagramServer(const std::string & strHost, const std::string strService,IRadioPDSReceiver * radioPDSReciever)
	:	m_radioPDSReciever (radioPDSReciever)
	{
		
		FUNCTION_ENTRY ("CPDSDatagramServer");
		if (strHost.empty() || strService.empty())
		{
			TA_THROW (CPDSDatagramServerException("Empty host or port number"));
		}
		
		if (!m_radioPDSReciever)
		{
			TA_THROW (CPDSDatagramServerException("Invalid pointer for reciever"));
		}

		m_udpManager = new CUDPSocketManager(strHost,strService,this);
		start ();
		FUNCTION_EXIT;

	}

	CPDSDatagramServer::~CPDSDatagramServer()
	{
		FUNCTION_ENTRY ("~CPDSDatagramServer");
		// terminating QueProcessor Thread that start during construction
		terminateAndWait();	
		if (m_udpManager)
		{
			delete m_udpManager;
			m_udpManager = NULL;	// Fix for : 56:  New value to pointer should be assigned  MR-2-1-19-3
		}
		FUNCTION_EXIT;
	}


	void CPDSDatagramServer::connectionEstablish(const int & socketID, ::sockaddr* clientAddress, std::vector <unsigned char> socketData)
	{
		
		{
			FUNCTION_ENTRY ("connectionEstablish");
			TA_THREADGUARD (m_clientRecLock)
			
			std::string strClientName = getIPAddress (clientAddress);
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Map Size : %d",m_connectedClients.size());
			
			std::multimap <long,SClientEventInfo>::iterator iter = m_mltiMapEventTrainRecord.begin();

			for (iter; iter != m_mltiMapEventTrainRecord.end(); iter++)
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"TrainId : %d Event Type : %d",iter->first,iter->second.lEventType);
			}

			TA_IRS_App::RadioPDSEvent * radioPDSEvent = NULL;
			try
			{
				radioPDSEvent = new TA_IRS_App::RadioPDSEvent(strClientName,socketData);
			
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Event Received : %s", radioPDSEvent->getBufferDump().c_str() );

				long lTrainID = radioPDSEvent->getTrainId(); 
				long lEventType = radioPDSEvent->getEventType();
			
				// check if call is existing or not
				// if no call for this train reject
				if (false == isCallForTrainIDExist(lTrainID))
				{
					LOG_GENERIC (SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Begin\\Resend Event with TrainID %d, has no call yet, rejecting event",
						lTrainID);
				
					delete radioPDSEvent;
					radioPDSEvent = NULL;	// Fix for : 100:  New value to pointer should be assigned  MR-2-1-19-3
					return;

				}			
			
				// check if the train id and event is already existing
				// if yes, then reject.
				if (true == isEventForTrainExist(lTrainID,lEventType))	
				{
					LOG_GENERIC (SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Begin/Resend Event with TrainID %d with EventType : %d, is already on process, rejecting event",
						lTrainID,lEventType);
					delete radioPDSEvent;
					radioPDSEvent = NULL;	// Fix for : 113:  New value to pointer should be assigned  MR-2-1-19-3
					return;
				}
			

				// the event is cleared, need to record as valid event
				SClientEventInfo cliEventInfo;
			
				CPDSDatagramClient * pdsClient = new CPDSDatagramClient (clientAddress,m_udpManager);
				m_connectedClients.insert(std::pair <std::string,CPDSDatagramClient *>(strClientName,pdsClient));
	
				cliEventInfo.lEventType = lEventType;
				cliEventInfo.strClientName = strClientName;

				m_mltiMapEventTrainRecord.insert(std::pair <long,SClientEventInfo>(lTrainID,cliEventInfo));
			
				m_radioPDSReciever->processEvent(radioPDSEvent);
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Checksum Invalid");
				//make sure to remove radioPDSEvent object
				delete radioPDSEvent;
				radioPDSEvent = NULL;	// fixed for : 136:  New value to pointer should be assigned  MR-2-1-19-3
			}

			FUNCTION_EXIT;
		}

	}


	void CPDSDatagramServer::Send(IPDSMethod * pdsMethod)
	{
		
		{
			FUNCTION_ENTRY ("Send");
			TA_THREADGUARD (m_clientRecLock);
			
			SClientData clientData; 
			
			clientData.strClientName = pdsMethod->getClientName();
			clientData.vctrData = pdsMethod->getFrameMessage();
			
			int nSize = clientData.vctrData.size();

			boost::shared_ptr <SClientData> clientPtr(new SClientData (clientData));
			
			if (m_connectedClients.end() == m_connectedClients.find (clientData.strClientName))
			{
				return;
			}

			insert (clientPtr);
			FUNCTION_EXIT;
		}

	}

	CPDSDatagramClient * CPDSDatagramServer::getClientObjFromClientName (const std::string & clientName)
	{
		FUNCTION_ENTRY ("getClientObjFromClientName");
		FUNCTION_EXIT;
		return m_connectedClients[clientName];
		
	}

	std::string CPDSDatagramServer::getIPAddress(::sockaddr *clientInfo)
	{
		FUNCTION_ENTRY ("getIPAddress");

		struct sockaddr_in * clientInAddr = (sockaddr_in *)clientInfo;

		long lPort = clientInAddr->sin_port;
		
		char * pszData = new char [1024];
		memset(pszData, 0, 1024);
		std::string ip_address = inet_ntoa(clientInAddr->sin_addr);
		sprintf (pszData,":%d",lPort);

		ip_address = ip_address + pszData;
		
		delete [] pszData;
		pszData = NULL;		// fixed for violation : 195:  New value to pointer should be assigned  MR-2-1-19-3

		FUNCTION_EXIT;

		return ip_address;
	}


	void CPDSDatagramServer::processEvent( boost::shared_ptr<SClientData> pdsMethod )
	{
		
		{
			
			FUNCTION_ENTRY ("processEvent");
			TA_THREADGUARD (m_sendLock);
			
			std::string strClientName = pdsMethod->strClientName;
			
			LOG_GENERIC (SourceInfo,
						 TA_Base_Core::DebugUtil::DebugInfo,
						 "Client Name - %s - Sending",
						 strClientName.c_str());

			try
			{
				CPDSDatagramClient * client = getClientObjFromClientName (strClientName);
				
				std::string strData = getClientObjFromClientName (strClientName)->getClientName ().c_str();
				client->sendData(pdsMethod->vctrData);
			}
			catch (...)
			{
				LOG_GENERIC (SourceInfo,
							TA_Base_Core::DebugUtil::DebugInfo,
							"Client Name Not Exist - %s",
							strClientName.c_str()
							);
			}
			FUNCTION_EXIT;
		}
	}
	
	void CPDSDatagramServer::setCallingTrain(long lTrainID)
	{
		
		FUNCTION_ENTRY ("setCallingTrain");
		std::set <long>::iterator trainIter;
		
		trainIter = m_setCallRecord.find(lTrainID);
		
		if (m_setCallRecord.end() == trainIter)
		{
			m_setCallRecord.insert(lTrainID);
		}
		FUNCTION_EXIT
		
	}

	void CPDSDatagramServer::removeAllRecordForTrain (long lTrainID)
	{
		
		FUNCTION_ENTRY ("connectionTerminated");
		
		LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Deleting Train # %d, Record, Current Record Holder size : %d",
					lTrainID,
					m_mltiMapEventTrainRecord.size());

		std::multimap<long,SClientEventInfo>::iterator clientEventIter;
		std::map <std::string,CPDSDatagramClient *>::iterator connctdCliIter;
		std::string strClientName;

		// check if trainID has call
		if (isCallForTrainIDExist(lTrainID))
		{
			m_setCallRecord.erase (lTrainID);
		}
		
		// check if the trainID has record on event collection
		// if have delete all record with the same key
		clientEventIter = m_mltiMapEventTrainRecord.find(lTrainID);
		
		while (clientEventIter != m_mltiMapEventTrainRecord.end())
		{
			strClientName = clientEventIter->second.strClientName;
			m_mltiMapEventTrainRecord.erase(clientEventIter);
			clientEventIter = m_mltiMapEventTrainRecord.find(lTrainID);
		}
		
		// check if there is a record on connection client map
		// if yes the erase
		
		connctdCliIter = m_connectedClients.find(strClientName);
		if (connctdCliIter != m_connectedClients.end())
		{
			CPDSDatagramClient * pdsClient = connctdCliIter->second;
			
			delete pdsClient;
			pdsClient = NULL;

			m_connectedClients.erase(strClientName);
		}
		
		
		LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Deleted Train # %d, Record, Current Record Holder size : %d",
					lTrainID,
				    m_mltiMapEventTrainRecord.size());

		FUNCTION_EXIT;
	}

	
	bool CPDSDatagramServer::isCallForTrainIDExist(const long &lTrainID)
	{
		FUNCTION_ENTRY ("isCallForTrainIDExist");
		
		bool bExist = false;

		std::set <long>::iterator trainIter;
		trainIter = m_setCallRecord.find(lTrainID);

		if (m_setCallRecord.end() != trainIter)
		{
			bExist = true;
		}
		
		FUNCTION_EXIT;
		
		return bExist;
	}

	bool CPDSDatagramServer::isEventForTrainExist(const long &lTrainID, const long &lEventType)
	{
		
		FUNCTION_ENTRY ("isEventForTrainExist");
		
		bool bExist = false;
		
		std::pair <std::multimap <long,SClientEventInfo>::iterator, std::multimap <long,SClientEventInfo>::iterator> equalRangeIter;

		equalRangeIter = m_mltiMapEventTrainRecord.equal_range(lTrainID);
		
		// there are record on the map using the trainid
		if (equalRangeIter.first == equalRangeIter.second)
		{
			FUNCTION_EXIT;
			return bExist;
		}	
		
		std::multimap <long,SClientEventInfo>::iterator cliIter = equalRangeIter.first;
		
		// check if the event is already existing
		for (cliIter; cliIter != equalRangeIter.second; cliIter++)
		{
			if (lEventType == cliIter->second.lEventType)
			{
				bExist = true;
				break;
			}
		}


		FUNCTION_EXIT;
		return bExist;
	}

	void CPDSDatagramServer::removeResendRecordForTrain(const long & lTrainID)
	{
		
		FUNCTION_ENTRY ("removeResendRecordForTrain()");

		std::pair <std::multimap <long,SClientEventInfo>::iterator, std::multimap <long,SClientEventInfo>::iterator> equalRangeIter;
		
		equalRangeIter = m_mltiMapEventTrainRecord.equal_range(lTrainID);
		
		// there are record on the map using the trainid
		if (equalRangeIter.first == equalRangeIter.second)
		{
			return ;
		}	
		
		std::multimap <long,SClientEventInfo>::iterator cliIter = equalRangeIter.first;
		
		// check if the event is already existing
		for (cliIter; cliIter != equalRangeIter.second; cliIter++)
		{
			if (TA_IRS_App::TtdFrame::FrameRepition == cliIter->second.lEventType)
			{
				m_mltiMapEventTrainRecord.erase(cliIter);
				break;
			}
		}
		
		FUNCTION_EXIT;
		
	}
}



