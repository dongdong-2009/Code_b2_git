
#include "stdafx.h"
#include "resource.h"
#include "TBTrainSelectorGUI.h"
#include "TrainBorneTabManager.h"
#include "TrainDvaVersionsPage.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
    TBTrainSelectorGUI::TBTrainSelectorGUI(TrainBorneTabManager* mg,
                         TrainDvaVersionsPage* pTrainDvaVersionPage,
						 CWnd* callbackWindow,
						 int updateInboundMessage,
						 int updateOutboundMessage,
						 TA_IRS_Bus::TrainListCtrl& inboundList,
                         TA_IRS_Bus::TrainListCtrl* outboundList,
                         CButton* allInbound,
                         CButton* allOutbound,
                         CButton* allTrains,
                         CButton* clearAllTrains,
						 CStatic* inboundLabel,
                         CStatic* outboundLabel,
                         TA_IRS_Bus::TrainGroupComboBox* trainGroupCombo,
                         CButton* saveGroup,
                         CButton* deleteGroup,
						 TA_IRS_Bus::TrainStaticGroupComboBox* trainStaticGroupCombo,
                         TA_IRS_Bus::ITrainSelectionListener* selectionListener ) : 
                         TA_IRS_Bus::TrainSelectorGUI(
						 inboundList,
                         outboundList,
                         allInbound,
                         allOutbound,
                         allTrains,
                         clearAllTrains,
						 inboundLabel,
                         outboundLabel,
                         trainGroupCombo,
                         saveGroup,
                         deleteGroup,
						 trainStaticGroupCombo,
                         selectionListener),
                         m_trainBorneTabManager(mg),
                         m_pTrainDvaVersionPage(pTrainDvaVersionPage),
						 m_callbackWindow(callbackWindow),
						 m_updateInboundMessage(updateInboundMessage),
						 m_updateOutboundMessage(updateOutboundMessage)
	{
	//	inboundList.setEventListener(NULL);
	//	outboundList->setEventListener(NULL);

	}

	void TBTrainSelectorGUI::updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) 
	{
		FUNCTION_ENTRY("updateInboundList");

		postUpdates(action,trainDetails,m_updateInboundMessage);

		FUNCTION_EXIT;
	}

	void TBTrainSelectorGUI::updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) 
	{
		FUNCTION_ENTRY("updateOutboundList");

		postUpdates(action,trainDetails,m_updateOutboundMessage);

		FUNCTION_EXIT;
	}

	void TBTrainSelectorGUI::updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) 
	{
		//(dynamic_cast<TA_IRS_Bus::TrainSelectorGUI*>(this))->updateSelectionCombo(trainListNames) ;
		TA_IRS_Bus::TrainSelectorGUI::updateSelectionCombo(trainListNames) ;
		m_trainBorneTabManager->updateSelectionCombo(trainListNames) ;
	}

	void TBTrainSelectorGUI::updateFallbackStatus(bool inFallback) 
	{
		//(dynamic_cast<TA_IRS_Bus::TrainSelectorGUI*>(this))->updateFallbackStatus(inFallback) ;
		TA_IRS_Bus::TrainSelectorGUI::updateFallbackStatus(inFallback) ;
		m_trainBorneTabManager->updateFallbackStatus(inFallback) ;
	}

	void TBTrainSelectorGUI::updateRadioGroupCombo(const std::vector<std::string>& trainListNames) 
	{
	//	(dynamic_cast<TA_IRS_Bus::TrainSelectorGUI*>(this))->updateRadioGroupCombo(trainListNames) ;
		TA_IRS_Bus::TrainSelectorGUI::updateRadioGroupCombo(trainListNames) ;
		m_trainBorneTabManager->updateRadioGroupCombo(trainListNames) ;
	}

	bool TBTrainSelectorGUI::hasRadioGroupSupport()
	{
		return true;
	}

	void TBTrainSelectorGUI::postUpdates(EAction action,const TrainInformationTypes::TrainDetails& trainDetails,int messageNumber)
	{
		FUNCTION_ENTRY("postUpdates");

		TrainListUpdate* update = new TrainListUpdate();
		update->action = action;
		update->trainDetails = trainDetails;

		int ret = m_callbackWindow->PostMessage(messageNumber,
												reinterpret_cast<unsigned int>(update));
		if ( ret == 0 )
		{
			LOG_GENERIC(
				SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error posting train update message. %d", messageNumber );

			// clean up - it wasnt posted so never will be deleted
			delete update;
			update = NULL;
		}

		FUNCTION_EXIT;
	}

	void TBTrainSelectorGUI::inboundListPostUpdate(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
	{
		FUNCTION_ENTRY("inboundListPostUpdate");

		TA_IRS_Bus::TrainSelectorGUI::updateInboundList(action,trainDetails) ;
		m_trainBorneTabManager->updateInboundList(action,trainDetails) ;
		if ( NULL != m_pTrainDvaVersionPage )
		{
			m_pTrainDvaVersionPage->updateTrainList( action, trainDetails );
		}

		FUNCTION_EXIT;
	}

	void TBTrainSelectorGUI::outboundListPostUpdate(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
	{
		FUNCTION_ENTRY("outboundListPostUpdate");

		TA_IRS_Bus::TrainSelectorGUI::updateOutboundList(action,trainDetails) ;
		m_trainBorneTabManager->updateOutboundList(action,trainDetails) ;
		if ( NULL != m_pTrainDvaVersionPage )
		{
			m_pTrainDvaVersionPage->updateTrainList( action, trainDetails );
		}

		FUNCTION_EXIT;
	}
}
