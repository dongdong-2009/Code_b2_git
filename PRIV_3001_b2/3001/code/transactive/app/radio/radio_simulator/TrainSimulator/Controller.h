#pragma once
#include "stdafx.h"
#include "IController.h"

#include "IModel.h"
#include "IView.h"

#include "AutoSendDataFactory.h"

namespace TA_IRS_App
{
	class Controller :public IController
	{
		public:
			Controller(IModel* model, IView* view);
			~Controller(void);

			void onBtnClick(const std::string& caption, unsigned int ID);
			void logViewID(ResourceInfo infoResource);
			void pecEvent( const std::string& pecMessage);
		
			bool sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains, bool isUsePrimary);

			HeartBeatInfo getTrainHeartBeatInfo();
			void addTrainHeartBeat(int trainID, int delay);
			TA_Base_Core::Thread::ThreadState_t getCurrentState();
			int getCommsObserverStatus();
			void sendStatusTrainHeartBeat(TrainStatus stat, int trainID);
			std::vector<TA_IRS_App::HeartBeatCommandProcessor*> getHeartBeatProcessor();

			void addTrainResponseStrategy(const TrainStrategyInfo& info);
			std::map<int, TrainStrategyInfo> getTrainStrategyMap();

			void removeTrainResponseStrategy(int trainID);
			
			// for auto sending configuration procedure
			
			// function evaluates the value for all PEC checkbox
			// this function needs to know the ID of all chkc box control
			// to evaluate
			virtual void onAutoSendConfPECClick (AutoSendingPecConfInfo autoSendInfo);
			virtual void onAutoSendConfPaClick (AutoSendingPAConfInfo autoSendInfo);
			virtual IConfigDataItemPtrType getUpdateConfForAutoChanging (EAutoSendType autoSendType);
			virtual void onAutoSendConfTTISClick (AutoSendingTTISConfInfo autoSendInfo);
			virtual void onChangeOverClick (ChangeOverConInfo autoSendInfo);
			virtual void onTrainCountCmboSelChange (TrainCountInfo autoSendInfo);

			virtual void onRadioIdComboSelChange (RadioIdSetupInfo radioIdSetupInfo);

		private:
			IModel* m_model;
			IView* m_view;

			AutoSendDataFactory m_autoSendDataFactory;

		public:
			virtual void onSendState (bool bRelease =  false);

	};
}

