#include "DVTrainSelectorGUI.h"
#include "VideoDisplayManagerDlg.h"

namespace TA_IRS_App
{
	
	void DVTrainSelectorGUI::updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
	{
		CString str;
		str.Format("DV updateOutboundList. action:%d,trainID:%d,current location:%ld,track:%ld",
		action,trainDetails.trainId,trainDetails.currentLocation,trainDetails.track);
	//	AfxMessageBox(str);
	//	m_dlg->SetDlgItemTextA(IDC_PA_ED_INFO,str);
	}
		
	void DVTrainSelectorGUI::updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) 
	{
		CString str;
		str.Format("DV updateOutboundList. action:%d,trainID:%d,current location:%ld,track:%ld",
		action,trainDetails.trainId,trainDetails.currentLocation,trainDetails.track);
	//	AfxMessageBox(str);
	//	m_dlg->SetDlgItemTextA(IDC_PA_ED_INFO,str);
	}
		
	void DVTrainSelectorGUI::updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) {};
		
	void DVTrainSelectorGUI::updateFallbackStatus(bool inFallback) {};
		
	bool DVTrainSelectorGUI::hasRadioGroupSupport() {return false;};
		
	void DVTrainSelectorGUI::updateRadioGroupCombo(const std::vector<std::string>& trainListNames) {};
		
	TA_IRS_Bus::TrainInformationTypes::TrainDetailsList DVTrainSelectorGUI::getSelectedTrainData() const 	
	{ 
		if(m_parentGUI!=NULL) 
		return m_parentGUI->getSelectedTrainData(); 
	};
}
