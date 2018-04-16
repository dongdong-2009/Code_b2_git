#include "PATrainSelectorGUI.h"

namespace TA_IRS_App
{
	
	void PATrainSelectorGUI::updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
	{
		CString str;
		str.Format("PA updateOutboundList. action:%d,trainID:%d,current location:%ld,track:%ld",
		action,trainDetails.trainId,trainDetails.currentLocation,trainDetails.track);
		AfxMessageBox(str);
	}
		
	void PATrainSelectorGUI::updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) 
	{
		CString str;
		str.Format("PA updateOutboundList. action:%d,trainID:%d,current location:%ld,track:%ld",
		action,trainDetails.trainId,trainDetails.currentLocation,trainDetails.track);
		AfxMessageBox(str);
	}
		
	void PATrainSelectorGUI::updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) {};
		
	void PATrainSelectorGUI::updateFallbackStatus(bool inFallback) {};
		
	bool PATrainSelectorGUI::hasRadioGroupSupport() {return false;};
		
	void PATrainSelectorGUI::updateRadioGroupCombo(const std::vector<std::string>& trainListNames) {};
		
	TA_IRS_Bus::TrainInformationTypes::TrainDetailsList PATrainSelectorGUI::getSelectedTrainData() const 	
	{ 
		if(m_parentGUI!=NULL) 
		return m_parentGUI->getSelectedTrainData(); 
	};
}
