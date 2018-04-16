/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSAPIImplDatabase.cpp $
* @author:   huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* a implementation of IMMSAPI.
*
*/

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "core/sockets/src/RawDatagramSocket.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/sockets/src/ICMPMessageCodec.h"
#include "core/data_access_interface/src/MmsRequestAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "MMSAPIImplDatabase.h"
 

// const int MmsApi::FIRST_PRIORITY = 9;
// const int MmsApi::SECOND_PRIORITY = 5;
// const int MmsApi::THIRD_PRIORITY = 1;
 
#define  MAX_PING_INTERVAL  3
 

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

	MMSAPIImplDatabase::MMSAPIImplDatabase(void): m_isControlMode(false),m_sftpSenderThread(NULL) ,m_isInitPing(false)
	{
		m_serverAddress="";
	}

	MMSAPIImplDatabase::~MMSAPIImplDatabase(void)
	{
		if (m_sftpSenderThread!=NULL)
		{
			m_sftpSenderThread->terminateAndWait();
			delete m_sftpSenderThread;
			m_sftpSenderThread=NULL;
		}
	}

	bool MMSAPIImplDatabase::submitAutoAlarmMessage(TA_Base_Core::MmsRequestAlarmData& alarm)
	{
		
		try //try and set the state of the alarm to show that a JR has been raised on it
		{
			 return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestAlarm(alarm);	
		}
		catch(...)
		{
			LOG0( SourceInfo,  DebugUtil::DebugError, "submitAutoAlarmMessage errro" );
		}
		return false;
	}

	bool MMSAPIImplDatabase::submitJobRequestMessage(const TA_Base_Bus::JobRequestDetailCorbaDef& details)
	{
		if( 0 != strcmp("NOT_FROM_ALARM", details.alarmUID.in()) )
		{
			try //try and set the state of the alarm to show that a JR has been raised on it
			{
				//translate JobRequest to database 
				 std::string alarmID= details.alarmUID.in();
				 timeb alarmDate;
				 alarmDate.time =details.alarmTime;
				 alarmDate.millitm=0;
				 unsigned long alarmSeverity=details.alarmSeverity;
				 std::string equipmentID=details.assetId.in();
				 std::string alarmFaultCode=details.alarmFaultCode.in();
				 std::string alarmDescription=details.alarmDescField.in();
				 std::string requestor=details.requestor.in();
				 std::string problemReport=details.problemReport.in();
				 timeb targetDate;			 
				 targetDate.time =details.targetDate;
				 targetDate.millitm=0;
				 bool isAutoTrigger = false;
				 TA_Base_Core::MmsRequestAlarmData* pmmsReqestData=new  TA_Base_Core::MmsRequestAlarmData
					(0,alarmID,alarmDate,alarmSeverity,equipmentID,alarmFaultCode, alarmDescription,
					requestor,problemReport,targetDate,isAutoTrigger);				 
				 std::auto_ptr<TA_Base_Core::MmsRequestAlarmData> mmsReqestData(pmmsReqestData);
				 return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestAlarm(*(mmsReqestData.get()));
			}
			catch(...)
			{
				LOG0( SourceInfo,  DebugUtil::DebugError, "submitJobRequestMessage saveMmsRequestAlarm errro" );
			}
			return false;
		}
		else //the Job Request was not from an alarm... nothing to do but log
		{
			//translate JobRequest to database 
			unsigned long entityKey=details.dataPointKey;    
			std::string equipmentID=details.assetId.in();		 
			std::string requestor=details.requestor.in();
			std::string problemReport=details.problemReport.in();
			timeb targetDate;			 
			targetDate.time =details.targetDate;
			targetDate.millitm=0; 
			TA_Base_Core::MmsRequestSchematicData*  pmmsReqestData =new TA_Base_Core::MmsRequestSchematicData(0,entityKey,      
				  equipmentID, requestor, problemReport,targetDate);
			std::auto_ptr<TA_Base_Core::MmsRequestSchematicData> mmsReqestData(pmmsReqestData);
			return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestSchematic(*(mmsReqestData.get()));
		}
	}

	bool MMSAPIImplDatabase::submitScheduledMessage(TA_Base_Bus::MMSScheduledMessage& msg)
	{
		
		unsigned long entityKey=msg.getEntityKey();    
		std::string equipmentID=msg.getAssetID();
		std::string meterCode=msg.getMeterCode();
		std::string equipmentDescription=msg.getEquipmentDescription();
		double dpValue=msg.getDataPointValue(); 
		timeb planDate;			 
		planDate=msg.getPlanDate();
		planDate.millitm=0; 
		timeb createDate;
		ftime( &createDate );
		TA_Base_Core::MmsRequestScheduledData::ScheduledType scheduledType;
		if (msg.getMessageType() ==TA_Base_Bus::MMSMessageType::COUNTER)
		{
			scheduledType=TA_Base_Core::MmsRequestScheduledData::COUNTER;
		}
		else
		{
			scheduledType=TA_Base_Core::MmsRequestScheduledData::MEASUREMENT;
		}

		TA_Base_Core::MmsRequestScheduledData*  pmmsReqestData =new TA_Base_Core::MmsRequestScheduledData( 0,
			scheduledType,
			 entityKey,         
			  equipmentID,
			  meterCode,
			 equipmentDescription,
			  dpValue,	 
			  planDate,             			
			  createDate);
		std::auto_ptr<TA_Base_Core::MmsRequestScheduledData> mmsReqestData(pmmsReqestData);
		return TA_Base_Core::MmsRequestAccessFactory::getInstance().saveMmsRequestScheduled(*(mmsReqestData.get())); 
	}

	bool MMSAPIImplDatabase::isMMSLive()
	{
		// hearbeat check
		bool r= pingServer(1);
		return r;
// 		if (r==false)
// 		{
// 			throw MmsConnectionException("cannot connect with MMS", 0);
// 		}
	}

	void  MMSAPIImplDatabase::initConfiguration(TA_Base_Core::MmsAgentEntityDataPtr pData)
	{
		if (m_sftpSenderThread==NULL)
		{
			m_sftpSenderThread=new SFTPSenderThread(pData);
			m_sftpSenderThread->start();
		}
		m_serverAddress=pData->getServerAddress().c_str();

	}

	void  MMSAPIImplDatabase::setToControlMode()
	{
		m_isControlMode=true;
		if (m_sftpSenderThread!=NULL)
		{
			m_sftpSenderThread->setToControlMode();			 
		}
	}

	void  MMSAPIImplDatabase::setToMonitorMode()
	{
		m_isControlMode=false;
		if (m_sftpSenderThread!=NULL)
		{
			m_sftpSenderThread->setToMonitorMode();			 
		}
	}

	void MMSAPIImplDatabase::initPing()		
	{
		TA_Base_Core::ThreadGuard guard( m_lockInitPing );
		if (m_isInitPing==true) return;

#ifdef WIN32
		m_pid = _getpid();
#else
		m_pid = getpid();
#endif
		//Mintao++
		//in order to use the ping method here under windows, must do the following
		//Init for socket (pinging) stuff:
#ifdef WIN32
		try
		{
			WSAData winsockData;
			WSAStartup( WINSOCK_VERSION, &winsockData);
			m_isInitPing=true;
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "WSAStartup threw");
			m_isInitPing=false;
		}

#endif	
	}



	bool MMSAPIImplDatabase::pingServer(double maxpingsecs)
	{
		initPing();
		
		bool toReturn = false;

		bool canRead = false;
		bool canWrite = false;
		//The socket that we use to send pings on
		TA_Base_Core::RawDatagramSocket m_socket("icmp", true);

		if(!m_socket.open())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"ping socket can not open");
			return false;
		}

		//TD15841 Mintao++
		if (maxpingsecs <= 0)
		{
			maxpingsecs = MAX_PING_INTERVAL;
		}

		double current = 0;
		double sentTime = 0;
		int count = 0;
		//Mintao++
		//TD14344
		//get the start ping time
		time_t timeSent = 0;
		time(&timeSent);
		sentTime = timeSent;
		//Mintao++
		//TD14344

		do {
			count ++;

			//API REF:
			// wait for io on only one socket.  One of read or write must be
			// true otherwise function will return immediately with false.
			// static bool waitForOneIO(SocketType *socket, bool read, bool write,
			//					 bool &canRead, bool &canWrite,
			//					 unsigned int timeMillis);
			//
			// wait in 200ms blocks so we can bail out on
			// m_tellThreadToStop in a reasonably responsive manner.
			//this call sets the canRead and canWrite
			if (TA_Base_Core::SocketSet<TA_Base_Core::IDatagramSocket>::waitForOneIO(&m_socket,
				true, true, canRead, canWrite, 200))
			{
				if (canWrite)
				{
					//note that the address obj is modified by the send method
					TA_Base_Core::InetAddress ina(m_serverAddress); 
					//construct a datagram
					TA_Base_Core::Datagram datagram;

					//fill out packet structure so that it is ECHO, i.e. a "ping"
					TA_Base_Core::ICMPMessageCodec::getInstance()->constructECHO(datagram, m_pid, count);

					//if the sending of this datagram fails...
					//note that the address obj is modified by the send method
					if (!m_socket.send(ina, datagram))
					{

						const std::string& name = ina.getHostName();
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "failed to send to ping for %s", name.c_str());

					}

				} //end if (canWrite)

				//if a ping responce has arrived...
				if (canRead)
				{
					//These variables are populated by the receive method
					TA_Base_Core::InetAddress outa;
					TA_Base_Core::Datagram datagram;

					//if we can read the responce from the socket...
					if (m_socket.receive(outa, datagram))
					{
						unsigned short id = 0, index = 0;

						//if we can get the data out of the package...
						if (TA_Base_Core::ICMPMessageCodec::getInstance()->decodeECHO(datagram, id, index))
						{

							//if this is a datagram we sent...
							if (id == m_pid && index < count)
							{							        
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"Successfully obtained the ping reply from server");
								//get out of the while loop
								toReturn = true;
								break;                           
							}
							else
							{
								// an old or someone else's ping
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"decodeECHO() got bad id=%u or bad seq=%u", id, index);
							}
						}
						else
						{
							// this isn't necessarily bad. It could be someone else's packet.
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "response failed checksum.");
						}
					}
					else
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive() failed");
					}
				} //end : if(canRead)
			} //end: if socket is ready to read or write

			time_t now;
			time(&now);
			current = now;

		}while (current - sentTime <= maxpingsecs);

		return toReturn;
	}

	 
}
