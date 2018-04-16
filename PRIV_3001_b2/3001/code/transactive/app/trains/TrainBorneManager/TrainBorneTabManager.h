#ifndef _TRAIN_BORN_TAB_MANAGER_H_
#define _TRAIN_BORN_TAB_MANAGER_H_

#include "stdafx.h"

#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"

#include "ITrainBorne.h"

//#include "ITBTrainSelectorGUI.h"
#include <string>
#include <vector>
#include <list>

#define PA_MANAGER				"PaManager"
#define PIDS_MANAGER			"PIDSManager"
#define DISPLAY_MANAGER			"DisplayManager"
#define TRAIN_CONTROLLER		"TrainController"
#define DISPLAY_TAB_ORDER		2

#define WM_TB_TRAIN_CTRL_UPDATE    WM_USER+2000
#define MNG_INBORNE_UPDATE			10
#define MNG_OUTBORNE_UPDATE			11


namespace TA_IRS_Bus
{
	class ITrainSelector;
}
namespace TA_IRS_App
{
	class TrainBorneTabManager
	{
	public:
		//typedef std::pair<ITrainBorne*,ITBTrainSelectorGUI*> TBTab;
		TrainBorneTabManager();
		~TrainBorneTabManager();
		//this class special functions
		void PushTabItems();
		void setSelectorGUI(TA_IRS_Bus::TrainSelectorGUI* gui) {m_tbTrainSelectorGUI = gui;};
		void setSelector(TA_IRS_Bus::TrainSelector* selector) {m_tbTrainSelector = selector;};

		void setWndParent(CWnd* wnd) { m_wndParent = wnd;};
		void setDisplayRect(RECT rect) {m_DisplayRect = rect;};
		void TrainSelected(TA_IRS_Bus::CommonTypes::TrainIdList selList);
		std::string getCurAppName();
		ITrainBorne* getCurTrainBorne();
		ITrainBorne* getTrainBorneByName(std::string appName);
		void MouseAction(UINT action,UINT nFlags,CPoint point);
		//genereic GUI interface
		void checkEntity(TA_Base_Core::IEntityData* guiEntity) ;

		void checkCommandLine() ;

		void entityChanged(const std::vector<std::string>& changes);
		
		void serverIsDown();
		
		void serverIsUp();

		void loadAllUserSettings();

		void saveAllUserSettings();

		void prepareForClose() ;
		
		unsigned long getApplicationType();

		virtual void createApplicationObject() ;
        virtual void onInitGenericGUICompleted();

		//ITrainBorne iterface
		void TabSeleted(int curTab);
		void DoApply();

		//ITrainSelectorGUI interface
		void updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		void updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) ;
		void updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) ;
		void updateFallbackStatus(bool inFallback) ;
		void updateRadioGroupCombo(const std::vector<std::string>& trainListNames) ;
	private:
		//std::list<TrainBorneTabGUI*> m_trainBorneTabList;
		std::list<ITrainBorne*> m_trainBorneList;
		//std::list<ITBTrainSelectorGUI*> m_trainSelectorGUIList;
		//std::list<TBTab> m_tabList;
		ITrainBorne* m_curTab;
		CWnd* m_wndParent;
		RECT m_DisplayRect;
		int m_curSel,m_lastSel;
		TA_IRS_Bus::TrainSelectorGUI* m_tbTrainSelectorGUI;
		TA_IRS_Bus::TrainSelector* m_tbTrainSelector;
	};

}

#endif

