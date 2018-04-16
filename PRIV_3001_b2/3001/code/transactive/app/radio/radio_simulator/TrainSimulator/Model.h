#pragma once
#include "IModel.h"
#include "IView.h"
#include <vector>
#include "GlobalDataTypes.h"
#include <string>
#include <map>
#include "app/radio/radio_simulator/TrainSimulator/IModelLogger.h"
#include "stdafx.h"

#include "app/radio/radio_simulator/TrainSimulator/RadioSimulator.h"
#include "UDPSocketManager.h"
#include "IPDSClientConnectionObserver.h"

#include "CommsMessageSenderImpl.h"

namespace TA_IRS_App
{
	class Model :
		public IModel,
		public IModelLogger,
		public IPDSClientConnectionObserver
	{
	public:
		Model(void);
		~Model(void);

		void setCaption(const std::string& caption, unsigned int ID);
		
		void attach(IView* view);
		void dettach(IView* view);
		void logViewID(ResourceInfo infoResource);
		void pecEvent(const std::string& pecMsge);

		std::vector<TA_IRS_App::HeartBeatCommandProcessor*> getHeartBeatProcessor();

		bool sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains,bool bIsPrimary);


		virtual void connectionEstablish (::sockaddr* clientAddress, std::vector<unsigned char> socketData);
	
		enum eBtnCaption
		{
			START_SERVER = 0,
			STOP_SERVER
		};

		enum eTitle
		{
			START = 0,
			STOP
		};

		typedef std::map< eTitle,std::string> MessageMap;

		TA_IRS_App::CUDPSocketManager* getUDPSocketManager();
		ResourceInfo getLogViewId();

		void beginDownload(std::vector<unsigned char>& data);
		
		HeartBeatInfo getTrainHeartBeatInfo();
		void addTrainHeartBeat(int trainID, int delay);
		TA_Base_Core::Thread::ThreadState_t getCurrentState();

		int getCommsObserverStatus();
		void sendStatusTrainHeartBeat(TrainStatus stat, int trainID);

		void addTrainResponseStrategy(const TrainStrategyInfo& info);

		std::map<int, TrainStrategyInfo> getTrainStrategyMap();

		void removeTrainResponseStrategy(int trainID);

		virtual void updateEctConfiguration (IConfigDataItemPtrType autoSendingConfData);
		IConfigDataItemPtrType getUpdateConfForAutoChanging (EAutoSendType autoSendType);

	private:
		std::vector<IView*> m_Observer;
		eBtnCaption m_eBtnCaption;
		eTitle m_eTitle;

		FILE * m_fileHandler;
		CString m_strFileName;
		
		void notifyChange(MessageInfo msgInfo);
		std::string getBtnCaption() const;
		std::string getListTitleInfo() const;
		std::string getListMessageInfo();
		void initialize();
		void serverStart();
		void serverStop();
		std::string getTimeLog();
		void setLogView(const std::string& logs);

		void setHeartBeatView(const HeartBeatInfo& info);
		//void setHeartBeatView(const TrainHeartBeatProcessorInfo& info);

		TA_IRS_App::CUDPSocketManager * m_socketManager;
		::sockaddr_in 		m_sockClientInfo;
		unsigned char _trainIDLibraryDownload;


		//std::string m_listMessageInfo;
		MessageMap m_messageMap;

		RadioSimulator* m_radioSimulator;
		unsigned int m_logViewID;
		ResourceInfo m_infoResource;
	
		public:
			virtual void sendStateUpdate (bool bRelease = false);

		private:
			CommsMessageSenderImpl m_messageSender;
	
	};
}

