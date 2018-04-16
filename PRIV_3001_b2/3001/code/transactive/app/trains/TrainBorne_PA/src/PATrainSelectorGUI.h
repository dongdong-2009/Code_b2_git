#ifndef _PA_TRAIN_SELECOTR_GUI_H_
#define _PA_TRAIN_SELECOTR_GUI_H_

#include <afxwin.h> 
#include <string>
#include <list>

#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"

namespace TA_IRS_App
{
class PATrainSelectorGUI : public TA_IRS_Bus::ITrainSelectorGUI
{
public:
	PATrainSelectorGUI(ITrainSelectorGUI* gui):m_parentGUI(gui){};

	virtual void updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		
	virtual void updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		
	virtual void updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) ;
		
	virtual void updateFallbackStatus(bool inFallback) ;
		
	virtual bool hasRadioGroupSupport() ;
		
	virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames) ;
		
	virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const ;
		
private:
	TA_IRS_Bus::ITrainSelectorGUI* m_parentGUI;
};
}
#endif
