#include "StdAfx.h"
#include "Controller.h"

namespace TA_IRS_App
{
	Controller::Controller(IModel* model, IView* view)
		:m_model(model)
		,m_view(view)
	{
		m_view->addListener(this);
		m_model->attach(m_view);
	}

	Controller::~Controller(void)
	{
		
	}

	void Controller::onBtnClick(const std::string& caption, unsigned int ID)
	{m_model->setCaption(caption,ID);
	}

	void Controller::logViewID(ResourceInfo infoResource)
	{
		m_model->logViewID(infoResource);
	}

	void Controller::pecEvent(const std::string& pecMessage)
	{
		m_model->pecEvent(pecMessage);
	}

	HeartBeatInfo Controller::getTrainHeartBeatInfo()
	{
		return m_model->getTrainHeartBeatInfo();
	}

	void Controller::addTrainHeartBeat(int trainID, int delay)
	{
		//call the model to add the train
		m_model->addTrainHeartBeat(trainID,delay);
	}

	TA_Base_Core::Thread::ThreadState_t Controller::getCurrentState()
	{
		return m_model->getCurrentState();
	}

	int Controller::getCommsObserverStatus()
	{
		return m_model->getCommsObserverStatus();
	}

	void Controller::sendStatusTrainHeartBeat(TrainStatus stat, int trainID)
	{
		m_model->sendStatusTrainHeartBeat(stat,trainID);
	}

	std::vector<TA_IRS_App::HeartBeatCommandProcessor*> Controller::getHeartBeatProcessor()
	{
		return m_model->getHeartBeatProcessor();
	}

	void Controller::addTrainResponseStrategy(const TrainStrategyInfo& info)
	{
		m_model->addTrainResponseStrategy(info);
	}

	std::map<int, TrainStrategyInfo> Controller::getTrainStrategyMap()
	{
		return m_model->getTrainStrategyMap();
	}

	void Controller::removeTrainResponseStrategy(int trainID)
	{
		m_model->removeTrainResponseStrategy(trainID);
	}

	bool Controller::sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains, bool isUsePrimary)
	{
		return m_model->sendTIMSCommand(cmdType,status,trains,isUsePrimary);
	}
	
	// for auto sending configuration procedure
	void Controller::onAutoSendConfPECClick (AutoSendingPecConfInfo autoSendInfo)
	{
		
		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(EPecType);
		
		PecAutoSendInfo * pecAutoSendInfo = (PecAutoSendInfo *)sharedObj.get();

		pecAutoSendInfo->setAutoSendPecAnswer(autoSendInfo.bAutoSendPecAnswer);
		pecAutoSendInfo->setAutoSendPecHold(autoSendInfo.bAutoSendPecHold);
		pecAutoSendInfo->setAutoSendPecReset(autoSendInfo.bAutoSendPecReset);

		m_model->updateEctConfiguration(sharedObj);
		
	}

	void Controller::onAutoSendConfPaClick (AutoSendingPAConfInfo autoSendInfo)
	{
		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(EPAType);

		PAAutoSendInfo * paAutoSendInfo = (PAAutoSendInfo *)sharedObj.get();

		paAutoSendInfo->setAutoSendPALive(autoSendInfo.bAutoSendLive);
		paAutoSendInfo->setAutoSendPAPreRecorded(autoSendInfo.bAutoSendPreRecorded);
		paAutoSendInfo->setAutoSendPALiveReset(autoSendInfo.bAutoSendLiveReset);
		paAutoSendInfo->setAutoSendPAPreRecordedReset(autoSendInfo.bAutoSendPreRecordedReset);
		paAutoSendInfo->setAutoSendPAContinue(autoSendInfo.bContinue);
		paAutoSendInfo->setAutoSendPAPreRecCompl(autoSendInfo.bAutoSendPreRecCompl);
		paAutoSendInfo->setAutoSendPAPreRecEmergency(autoSendInfo.bPreRecEmergency);

		m_model->updateEctConfiguration(sharedObj);
	}
	
	IConfigDataItemPtrType Controller::getUpdateConfForAutoChanging (EAutoSendType autoSendType)
	{
		return m_model->getUpdateConfForAutoChanging(autoSendType);
	}

	void Controller::onSendState (bool bRelease)
	{
		m_model->sendStateUpdate(bRelease);
	}

	void Controller::onAutoSendConfTTISClick (AutoSendingTTISConfInfo autoSendInfo)
	{

		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(ETTISType);

		TTISAutoSendInfo * ttisAutoSendInfo = (TTISAutoSendInfo *)sharedObj.get();

		ttisAutoSendInfo->setAutoSendTTISLib(autoSendInfo.bAutoSendTTISLib);
		ttisAutoSendInfo->setSendTtisCmdRecvFailed(autoSendInfo.bSendTtisCmdRecvFailed);

		m_model->updateEctConfiguration(sharedObj);
	}

	void Controller::onChangeOverClick (ChangeOverConInfo autoSendInfo)
	{

		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(EChangeOver);

		ChangeOverConfig * changeOver = (ChangeOverConfig *)sharedObj.get();

		changeOver->setStatusFailure(autoSendInfo.bStatus);
		changeOver->setTtisStatusFailure(autoSendInfo.bTtisStatus);
		changeOver->setPECAnsStatusFailure(autoSendInfo.bPecAnsStatus);
		changeOver->setPALiveStatusFailure(autoSendInfo.bPaLiveStatus);
		changeOver->setPAPreRecStatusFailure(autoSendInfo.bPaPreRecStatus);
		changeOver->setUpgradeTtisStatusFailure(autoSendInfo.bUpdateTtisStatus);

		m_model->updateEctConfiguration(sharedObj);
	}

	void Controller::onTrainCountCmboSelChange (TrainCountInfo autoSendInfo)
	{
		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(ETrainCount);

		TrainCountForAutoSend * trainCount = (TrainCountForAutoSend *)sharedObj.get();

		trainCount->setTrainCount(autoSendInfo.strTrainCount);

		m_model->updateEctConfiguration(sharedObj);
	}
	
	void Controller::onRadioIdComboSelChange (RadioIdSetupInfo radioIdSetupInfo)
	{
		IConfigDataItemPtrType sharedObj = m_autoSendDataFactory.createAutoSendDataInfo(ERadioIdSetup);

		RadioIdSetupConfig * radioSetup = (RadioIdSetupConfig *)sharedObj.get();

		radioSetup->setRadioConfigValue(radioIdSetupInfo.bIsUsePrimary);

		m_model->updateEctConfiguration(sharedObj);
	}
}	

