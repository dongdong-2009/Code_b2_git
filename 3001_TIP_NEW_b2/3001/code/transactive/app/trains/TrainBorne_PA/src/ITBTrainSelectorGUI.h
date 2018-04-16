#ifndef _I_TB_TRAIN_SELECOTR_GUI_H_
#define _I_TB_TRAIN_SELECOTR_GUI_H_

#include <afxwin.h> 
#include <string>
#include <list>

#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

namespace TA_IRS_App
{
	class ITBTrainSelectorGUI : public TA_IRS_Bus::ITrainSelectorGUI
{
public:
	ITBTrainSelectorGUI():m_parentGUI(NULL){};

	virtual void updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) =0;
		
	virtual void updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) =0;
		
	virtual void updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) {};
		
	virtual void updateFallbackStatus(bool inFallback) {};
		
	virtual bool hasRadioGroupSupport() {return false;};
		
	virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames) {};
		
	virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const 	
	{ 
		if(m_parentGUI!=NULL) 
		return m_parentGUI->getSelectedTrainData(); 
	};
		
	virtual void setParentGUI(TA_IRS_Bus::ITrainSelectorGUI* gui)
	{
		m_parentGUI = gui;
	}

	virtual TA_IRS_Bus::ITrainSelectorGUI* getParentGUI()
	{
		return m_parentGUI;
	}
private:
	TA_IRS_Bus::ITrainSelectorGUI* m_parentGUI;
};
}
#endif
