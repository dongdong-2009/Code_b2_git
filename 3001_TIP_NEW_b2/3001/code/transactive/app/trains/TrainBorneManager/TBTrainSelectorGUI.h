#ifndef _TB_TRAIN_SELECOTR_GUI_H_
#define _TB_TRAIN_SELECOTR_GUI_H_

#include <afxwin.h> 
#include <string>
#include <list>

#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

using namespace TA_IRS_Bus;

namespace TA_IRS_App
{
class TrainBorneTabManager;
class TrainDvaVersionsPage;
class TBTrainSelectorGUI : public TA_IRS_Bus::TrainSelectorGUI
{
public:

	typedef struct
	{
		ITrainSelectorGUI::EAction action;
		TrainInformationTypes::TrainDetails trainDetails;
	} TrainListUpdate;

	TBTrainSelectorGUI(  TrainBorneTabManager* mg,
                         TrainDvaVersionsPage* pTrainDvaVersionPage,
						 CWnd* callback,
                    	 int updateInboundMessage,
                         int updateOutboundMessage,
                         //     int updateSelectionMessage
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
						 TA_IRS_Bus::TrainStaticGroupComboBox* trainStaticGroupCombo = NULL,
                         TA_IRS_Bus::ITrainSelectionListener* selectionListener = NULL
						 );

	
	virtual void updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		
	virtual void updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		
	virtual void updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) ;
		
	virtual void updateFallbackStatus(bool inFallback) ;
			
	virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames) ;

	virtual bool hasRadioGroupSupport();

	/** 
      * postUpdates
      *
      * Post the inbound/outbound list update to all callback windows
      *
      * @param action the action (add/delete etc)
      * @param trainDetails the train details structure
      * @param messageNumber the message number to post,
      *        m_updateInboundMessage or m_updateOutboundMessage
      *
      */
        void postUpdates( EAction action,
						  const TrainInformationTypes::TrainDetails& trainDetails,
                          int messageNumber );


	void inboundListPostUpdate(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
	void outboundListPostUpdate(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
	void selectionComboPostUpdate(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) ;
	void FallbackStatusPostUpdate(bool inFallback) ;
	void radioGroupComboPostUpdate(const std::vector<std::string>& trainListNames);
	bool hasRadioGroupSupportPostUpdate();

private:
	TrainBorneTabManager* m_trainBorneTabManager;
    TrainDvaVersionsPage* m_pTrainDvaVersionPage;

	//Messages to send
	int m_updateInboundMessage;
	int m_updateOutboundMessage;
	CWnd* m_callbackWindow;


};
}
#endif
