#include "StdAfx.h"
#include "Model.h"
#include <algorithm>
#include <iostream>
#include "app/radio/radio_simulator/TrainSimulator/SimulatorConfig.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/MotorolaSDSProtocolHelper.h"

#include <vector>

#include "EtcTrainSimConfigHandler.h"

namespace TA_IRS_App
{
	
	Model::Model(void)
		:m_eBtnCaption(STOP_SERVER)
		 ,m_eTitle(STOP)
		 ,m_radioSimulator(NULL)
		 ,m_socketManager(NULL)
		 ,_trainIDLibraryDownload(0)
	{
		FUNCTION_ENTRY("Model");

		initialize();

		FUNCTION_EXIT;
	}

	Model::~Model(void)
	{
		FUNCTION_ENTRY("~Mode");

		if ( NULL != m_radioSimulator )
		{
			delete m_radioSimulator;
			m_radioSimulator = NULL;
		}

		FUNCTION_EXIT;
	}

	void Model::attach(IView* view)
	{
		FUNCTION_ENTRY("attach");

		//Check if there is an existing view, else add the new view in cache
		std::vector<IView*>::iterator iter = std::find(m_Observer.begin(),m_Observer.end(),view);
		if ( iter == m_Observer.end() )
		{
			m_Observer.push_back(view);
		}

		FUNCTION_EXIT;
		
	}

	void Model::dettach(IView* view)
	{
		FUNCTION_ENTRY("dettach");

		std::vector<IView*>::iterator iter = std::find(m_Observer.begin(),m_Observer.end(),view);
		if ( iter != m_Observer.end())
		{
			if ( NULL != *iter )
			{
				delete *iter;
				*iter = NULL;
			}
			
		}

		FUNCTION_EXIT;
	}

	void Model::notifyChange(MessageInfo msgInfo)
	{
		FUNCTION_ENTRY("notifyChange");

		std::vector<IView*>::iterator iter = m_Observer.begin();
		for ( iter; iter != m_Observer.end(); iter++ )
		{
			(*iter)->updateUI(msgInfo);
		}

		FUNCTION_EXIT;
	}

	void Model::setCaption(const std::string& caption, unsigned int ID)
	{
		FUNCTION_ENTRY("setCaption");

		if ( caption.compare("Start Server") == 0)
		{
			if ( SimulatorConfig::getInstance()->checkRadioIDConfig())
			{
				serverStart();
				m_eTitle = START;
				m_messageMap.insert(std::pair<eTitle,std::string>(m_eTitle,"Server Started"));
				m_eBtnCaption = STOP_SERVER;
			}
			else
			{
				::MessageBox(NULL,L"Server cannot be started, \nbecause RadioIDConfig.cfg is missing",L"Error Message",MB_ICONERROR | MB_OK);
				return;
			}
			
		}
		else
		{
			serverStop();
			m_eTitle = STOP;
			m_messageMap.insert(std::pair<eTitle,std::string>(m_eTitle,"Server Stopped"));
			m_eBtnCaption = START_SERVER;
		}

		MessageInfo msgInfo;
		msgInfo.ID = ID;
		msgInfo.BtnCaption = getBtnCaption();
		msgInfo.ListInfoMessage.Title = getListTitleInfo();
		msgInfo.ListInfoMessage.Message = m_messageMap[m_eTitle];
		notifyChange(msgInfo);

		FUNCTION_EXIT;
	}

	std::string Model::getBtnCaption() const
	{
		std::string ret = "";
		switch(m_eBtnCaption)
		{
		case START_SERVER:
			ret = "Start Server";
			break;

		case STOP_SERVER:
			ret = "Stop Server";
			break;
		}

		return ret;
	}

	std::string Model::getListTitleInfo() const
	{
		std::string _ret = "";
		switch(m_eTitle)
		{
		case START :
			_ret = "Start";
			break;

		case STOP :
			_ret = "Stop";
			break;
		}
		return _ret;
	}

	std::string Model::getListMessageInfo() 
	{
		//return m_listMessageInfo;
		return "";
	}

	void Model::serverStart()
	{
		m_radioSimulator->startSim();

		//Start UDP Socket
// 		m_socketManager = new TA_IRS_App::CUDPSocketManager(SimulatorConfig::getInstance()->m_PDSServerIP,
// 			SimulatorConfig::getInstance()->m_PDSServerPort,
// 			this,&m_sockClientInfo);
	}

	void Model::serverStop()
	{
		m_radioSimulator->stopSim();

// 		if ( m_socketManager )
// 		{
// 			m_socketManager->terminateAndWait();
// 			delete m_socketManager;
// 			m_socketManager = NULL;
// 		}
	}

	void Model::initialize()
	{
		SimulatorConfig* config = SimulatorConfig::getInstance();
		std::string name = "TrainSimulator";
		char    port[100];
		itoa(config->m_port,port,10);
		m_radioSimulator = new RadioSimulator(name,
			config->m_ip.c_str(),
			port,this);
		m_radioSimulator->getInstanceMotorolaHelper()->setLoggerView(this);
		m_radioSimulator->getTTISCommandProcessor()->attachSocketManager(this);
	}

	void Model::logViewID(ResourceInfo infoResource)
	{
		m_infoResource = infoResource;
	}

	ResourceInfo Model::getLogViewId()
	{
		return m_infoResource;
	}

	std::string Model::getTimeLog()
	{
		//Get Time
		SYSTEMTIME now;
		GetLocalTime(&now);
		CString strTime;

		strTime.Format(L"%d-%d-%d %d:%d:%d.%d",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,now.wMilliseconds );
	
		CT2A _ret(strTime);

		return _ret;
	}

	void Model::setLogView(const std::string& logs)
	{
		
		char _timeTemp[1024] = {0};
		std::sprintf(_timeTemp,"%s : %d : ",__FILE__, __LINE__ );
		
		std::string _logMessage = _timeTemp + logs;
// 		CString*  timeLog = new CString(getTimeLog().c_str());
// 
// 		CString* pstring = new CString(_logMessage.c_str());

		LogInfo* _logInfo = new LogInfo;

		_logInfo->LogMessage = logs; //_logMessage;
		_logInfo->Time = getTimeLog();
		
		::PostMessage(m_infoResource.HanderWND,WM_UPDATE_LOGGER,reinterpret_cast<WPARAM>(_logInfo),NULL);
	}

	void Model::setHeartBeatView(const HeartBeatInfo& info)
	{
		HeartBeatInfo* _heartBeatInfo = new HeartBeatInfo;
		_heartBeatInfo->TrainID = info.TrainID;
		_heartBeatInfo->TimeDelay = info.TimeDelay;
		_heartBeatInfo->TrainStatus = info.TrainStatus;

		_heartBeatInfo->CommsStatus = m_radioSimulator->getCommsObserverStatus();

 		::PostMessage(m_infoResource.HanderWND,WM_HEARTBEAT_INFO,reinterpret_cast<WPARAM>(_heartBeatInfo),NULL);
	
	}

	void Model::pecEvent(const std::string& pecMsge)
	{
		if ( m_radioSimulator->getNumberOfConnection() == 0 )
		{
			::MessageBox(NULL,L"No RadioAgent has been connected. \nThe message is ignored.",L"Message",MB_ICONINFORMATION);
			return;
		}
		setLogView(pecMsge);
		m_radioSimulator->getInstanceMotorolaHelper()->buildMotorolaHeaders(pecMsge);
		
	}

	void Model::connectionEstablish(sockaddr *clientAddress, std::vector<unsigned char> socketData)
	{
		// recieved data
		char * pszData = new char [1023];
		memset(pszData, 0, 1023);
		int nSize = socketData.size();

		for (int nCounter = 0; nCounter < nSize; nCounter++)
		{	
			if (nCounter != 0)
				sprintf (pszData,"%s %d",pszData,socketData.at (nCounter));
			else
				sprintf (pszData,"%d",socketData.at (nCounter));
		}

		std::string strData = pszData;

		delete [] pszData;

	
		::PostMessage (m_infoResource.HanderWND,WM_UIUPDATE,(WPARAM)new CString (strData.c_str()),(LPARAM)FRAME_RECV);

	}

	TA_IRS_App::CUDPSocketManager* Model::getUDPSocketManager()
	{
		return m_socketManager;
	}

	void Model::beginDownload(std::vector<unsigned char> &data)
	{
		
		_trainIDLibraryDownload = data[2];

		char * pszData = new char [1023];
		
		for ( int nCounter = 0; nCounter < data.size(); nCounter++ )
		{
			if (nCounter != 0)
				sprintf (pszData,"%s %d",pszData,data.at (nCounter));
			else
				sprintf (pszData,"%d",data.at (nCounter));
			
		}
					 
		std::string strData = pszData;
		delete[] pszData;
		pszData = NULL;

		BeginDownloadData* _beginDownload = new BeginDownloadData;

		_beginDownload->trainID = _trainIDLibraryDownload;
		_beginDownload->Data = data;
		_beginDownload->StringData = strData;
					 
		//::PostMessage(m_infoResource.HanderWND,WM_UIUPDATE,(WPARAM)new CString (strData.c_str()),(LPARAM)FRAME_SEND);

		::PostMessage(m_infoResource.HanderWND,WM_UIUPDATE, reinterpret_cast<WPARAM> (_beginDownload), (LPARAM)FRAME_SEND );
	
	}

	HeartBeatInfo Model::getTrainHeartBeatInfo()
	{
		return m_radioSimulator->getTrainHeartBeatInfo();
	}

	void Model::addTrainHeartBeat(int trainID, int delay)
	{
		m_radioSimulator->addTrainHeartBeat(trainID,delay);
	}

	TA_Base_Core::Thread::ThreadState_t Model::getCurrentState()
	{
		return m_radioSimulator->getCurrentState();
	}

	int Model::getCommsObserverStatus()
	{
		return m_radioSimulator->getCommsObserverStatus();
	}

	void Model::sendStatusTrainHeartBeat(TrainStatus stat, int trainID)
	{
		m_radioSimulator->sendStatusTrainHeartBeat(stat,trainID);
	}

	std::vector<TA_IRS_App::HeartBeatCommandProcessor*> Model::getHeartBeatProcessor()
	{
		return m_radioSimulator->getHeartBeatProcessor();
	}

	void Model::addTrainResponseStrategy(const TrainStrategyInfo& info)
	{
		m_radioSimulator->addTrainResponseStrategy(info);
	}

	std::map<int, TrainStrategyInfo> Model::getTrainStrategyMap()
	{
		return m_radioSimulator->getTrainStrategyMap();
	}

	void Model::removeTrainResponseStrategy(int trainID)
	{
		m_radioSimulator->removeTrainResponseStrategy(trainID);
	}

	bool Model::sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains, bool bIsPrimary)
	{
		return m_radioSimulator->sendTIMSCommand(cmdType,status,trains,bIsPrimary);
	}

	void Model::updateEctConfiguration (IConfigDataItemPtrType autoSendingConfData)
	{
		EtcTrainSimConfigHandler::getInstance()->updateEtcCConfiguration(autoSendingConfData);
	}

	IConfigDataItemPtrType Model::getUpdateConfForAutoChanging (EAutoSendType autoSendType)
	{
		
		IConfigDataItemPtrType ptrType = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(autoSendType);
		return ptrType;
	}

	void Model::sendStateUpdate (bool bRelease)
	{
		m_messageSender.sendStateUpdate(bRelease);
	}
}
