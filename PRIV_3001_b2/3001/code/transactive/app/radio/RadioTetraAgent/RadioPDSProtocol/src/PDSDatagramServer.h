// PDSDatagramServer.h: interface for the CPDSDatagramServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PDSDATAGRAMSERVER_H__2631B8B4_1D91_46E0_8FA3_0B555103BD64__INCLUDED_)
#define AFX_PDSDATAGRAMSERVER_H__2631B8B4_1D91_46E0_8FA3_0B555103BD64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSClientConnectionObserver.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSDatagramClient.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/UDPSocketManager.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSDatagramServer.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSMethod.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSReceiver.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IEventRecordRemover.h"

#include <set>
#include <vector>

#include "core/threads/src/QueueProcessor.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

	struct SClientData
	{
		std::string strClientName;
		std::vector <unsigned char> vctrData;
	};
	
	struct SClientEventInfo
	{
		std::string strClientName;
		long		lEventType;
	};

	class CPDSDatagramServerException : public TA_Base_Core::TransactiveException
	{
		public:
			CPDSDatagramServerException( const std::string& errorText ) throw();
			virtual ~CPDSDatagramServerException() throw();
	};


	class CPDSDatagramServer  : public IPDSClientConnectionObserver,
								public TA_Base_Core::QueueProcessor<SClientData>,
								public IPDSDatagramServer,
								public IEventRecordRemover

	{

		public:
			void setCallingTrain (long lTrainID);
			CPDSDatagramServer(const std::string & strHost, const std::string strService, IRadioPDSReceiver * radioPDSReciever);
			virtual ~CPDSDatagramServer();
			virtual void Send (IPDSMethod * pdsMethod);
			virtual void removeResendRecordForTrain(const long & lTrainID);
		protected:

			
			virtual void connectionEstablish(const int & socketID,::sockaddr* clientAddress,std::vector <unsigned char> socketData);
			virtual void removeAllRecordForTrain (long lTrainID);
			virtual void processEvent( boost::shared_ptr<SClientData> pdsMethod );
			void setClientInfo (struct sockaddr * test);
		private:
			bool isEventForTrainExist (const long & lTrainID, const long & lEventType);
			bool isCallForTrainIDExist (const long & lTrainID);
			std::string getIPAddress (::sockaddr * clientInfo);
			CUDPSocketManager * m_udpManager;
			std::map <std::string, CPDSDatagramClient *> m_connectedClients;
			
			CPDSDatagramClient * getClientObjFromClientName (const std::string & clientName);
			
			TA_Base_Core::ReEntrantThreadLockable   m_clientRecLock;
			TA_Base_Core::ReEntrantThreadLockable   m_sendLock;
			TA_IRS_App::IRadioPDSReceiver		  * m_radioPDSReciever;

			std::set <long>							m_setCallRecord;
			std::multimap <long,SClientEventInfo>		m_mltiMapEventTrainRecord;
	};
}

#endif // !defined(AFX_PDSDATAGRAMSERVER_H__2631B8B4_1D91_46E0_8FA3_0B555103BD64__INCLUDED_)
