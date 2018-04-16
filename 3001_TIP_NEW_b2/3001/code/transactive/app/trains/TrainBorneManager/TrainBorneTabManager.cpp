#include "stdafx.h"
#include "TrainBorneManager/TrainBorneTabManager.h"

/* include ITrainBorne object header files here */



#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core\utilities\src\runparams.h"
#include "core/data_access_interface/entity_access/src/AlarmRaiserEntityData.h"

#include "ITrainBorne.h"
 /*Modify this macro to actual ITrainBorne inherited object number*/
#define MANAGER_COUNT 4
/**
#include the ITrainBorne inherited class'h header files here
 */
#include "Resource.h"
//#include "TrainBorne_PA/PAManagerDlg.h"
#include "app/trains/TrainBorne_PA/src/TrainBroadcastPage.h"
//#include "app/signs/TrainBorne_PIDS/src/PIDSManagerDlg.h"
#include "app/trains/TrainBorne_PIDS/src/PIDSManagerDlg.h"
//#include "Test2/Test2Dlg.h"
#include "TrainBorne_DisplayManager/VideoDisplayManagerDlg.h"

#include "app/trains/TrainControlManager/src/TrainControlDialog.h"
namespace TA_IRS_App
{
	TrainBorneTabManager::TrainBorneTabManager()
		:m_curSel(-1),m_lastSel(-1),m_curTab(NULL)
	{
		m_trainBorneList.clear();
	}

	TrainBorneTabManager::~TrainBorneTabManager()
	{
		
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin(); 
		for(;it!=m_trainBorneList.end();it++)
		{
			ITrainBorne* tab = dynamic_cast<ITrainBorne*>(*it);
	
			delete tab;
			tab = NULL;
		}
		m_trainBorneList.clear();
	}

	void TrainBorneTabManager::PushTabItems()
	{

		ITrainBorne* item[MANAGER_COUNT];
		/**
		 construct ITrainBorne objects here and set their names
		 */
		
		//item[0]  = new PAManagerDlg(m_wndParent);
		item[0]  = new PIDSManagerDlg(m_wndParent);
		item[0]->TB_SetAppName(PIDS_MANAGER);
		item[1]  = new TrainBroadcastPage(m_wndParent);
		item[1]->TB_SetAppName(PA_MANAGER);
		item[2]= new VideoDisplayManagerDlg(m_wndParent);
		item[2]->TB_SetAppName(DISPLAY_MANAGER);
		item[3] = new TrainControlDialog(m_wndParent);
		item[3]->TB_SetAppName(TRAIN_CONTROLLER);
		for(int i=0;i<MANAGER_COUNT;i++)
		{

			item[i]->TB_SetParentGUI(m_tbTrainSelectorGUI);
			item[i]->TB_SetParentSelector(m_tbTrainSelector);
			item[i]->Create(item[i]->TB_GetTemplateID(),m_wndParent);
			item[i]->SetWindowPos(NULL,m_DisplayRect.left,m_DisplayRect.top+7,m_DisplayRect.right - m_DisplayRect.left,m_DisplayRect.bottom-m_DisplayRect.top,SWP_HIDEWINDOW);
			m_trainBorneList.push_back( item[i]);
			

		}
		TabSeleted(0);
	}

	void TrainBorneTabManager::TabSeleted(int curTab)
	{
		if(curTab<0 || curTab>=MANAGER_COUNT )
			return;
		if(curTab==m_curSel)
			return;
		m_lastSel=m_curSel;
		m_curSel = curTab;
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(int i=0;it!=m_trainBorneList.end();it++,i++)
		//std::list<TBTab>::iterator it =m_tabList.begin();
		//for(int i=0;it!=m_tabList.end();it++,i++)
		{
			if(i==m_curSel)
			{
				m_curTab = (*it);
				
				(*it)->ShowWindow(SW_SHOW);
				(*it)->TB_PageActived(true);
			}
			if(i==m_lastSel)
			{
				
				(*it)->ShowWindow(SW_HIDE);
				(*it)->TB_PageActived(false);
			}
		}
	}

	void TrainBorneTabManager::MouseAction(UINT action,UINT nFlags,CPoint point)
	{
		if(NULL!=m_curTab)
			m_curTab->TB_MouseAction(action,nFlags,point) ;
	}

	void TrainBorneTabManager::checkEntity(TA_Base_Core::IEntityData* guiEntity) 
	{
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
			(*it)->TB_CheckEntity(guiEntity);
			std::string str=guiEntity->getDescription();
			std::string str1=guiEntity->getName();
		}
	}

	std::string TrainBorneTabManager::getCurAppName()
	{
		if(NULL!=m_curTab)
			return m_curTab->TB_GetAppName() ;
		return "";
	}

	ITrainBorne* TrainBorneTabManager::getCurTrainBorne()
	{
		return m_curTab;
	}

	ITrainBorne* TrainBorneTabManager::getTrainBorneByName(std::string appName)
	{
		if(appName.empty())
			return NULL;
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
			if(appName==(*it)->TB_GetAppName())
				return (*it);
		}
		return NULL;
	}

	void TrainBorneTabManager::checkCommandLine() 
	{;}

	void TrainBorneTabManager::entityChanged(const std::vector<std::string>& changes){;}
	
	void TrainBorneTabManager::serverIsDown(){;}
	
	void TrainBorneTabManager::serverIsUp(){;}

	void TrainBorneTabManager::loadAllUserSettings(){;}

	void TrainBorneTabManager::saveAllUserSettings(){;}

	void TrainBorneTabManager::prepareForClose() {;}
	
	unsigned long TrainBorneTabManager::getApplicationType(){ return 0l;}

	void TrainBorneTabManager::createApplicationObject() 
	{

	}

	void TrainBorneTabManager::TrainSelected(TA_IRS_Bus::CommonTypes::TrainIdList selList)
	{
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		{
			ITrainBorne* tab = (*it);
			tab->TB_TrainSelected(selList);
		}
	}

	void TrainBorneTabManager::DoApply()
	{
		if(m_curTab!=NULL)
			m_curTab->TB_DoApply();
	}

	void TrainBorneTabManager::onInitGenericGUICompleted(){;}

	void TrainBorneTabManager::updateInboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
	{
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
	//		ITBTrainSelectorGUI* gui = (*it).second;
	//		gui->updateInboundList(action,trainDetails);
			if(NULL!=(*it)->TB_GetObserver())
			{
				(*it)->TB_GetObserver()->updateInboundList(action,trainDetails);
				SendMessage(m_wndParent->m_hWnd,WM_TB_TRAIN_CTRL_UPDATE,MNG_INBORNE_UPDATE,0l );
			}
		}
	}

	void TrainBorneTabManager::updateOutboundList(TA_IRS_Bus::ITrainSelectorGUI::EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails) 
	{
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
	//		ITBTrainSelectorGUI* gui = (*it).second;
	//		gui->updateOutboundList(action,trainDetails);
			if(NULL!=(*it)->TB_GetObserver())
			{
				(*it)->TB_GetObserver()->updateOutboundList(action,trainDetails);
				SendMessage(m_wndParent->m_hWnd,WM_TB_TRAIN_CTRL_UPDATE,MNG_INBORNE_UPDATE,0l );
			}
		}
	}

	void TrainBorneTabManager::updateSelectionCombo(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames) 
	{
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
		//	ITBTrainSelectorGUI* gui = (*it).second;
		//	gui->updateSelectionCombo(trainListNames);
			if(NULL!=(*it)->TB_GetObserver())
				(*it)->TB_GetObserver()->updateSelectionCombo(trainListNames);
		}
	}

	void TrainBorneTabManager::updateFallbackStatus(bool inFallback) 
	{
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
	//		ITBTrainSelectorGUI* gui = (*it).second;
	//		gui->updateFallbackStatus(inFallback);
			if(NULL!=(*it)->TB_GetObserver())
				(*it)->TB_GetObserver()->updateFallbackStatus(inFallback);
		}
	}

	void TrainBorneTabManager::updateRadioGroupCombo(const std::vector<std::string>& trainListNames) 
	{
		FUNCTION_ENTRY( "manager tab updateRadioGroupCombo" );
	//	list<TBTab>::iterator it = m_tabList.begin();
	//	for(;it!=m_tabList.end();it++)
		std::list<ITrainBorne*>::iterator it = m_trainBorneList.begin();
		for(;it!=m_trainBorneList.end();it++)
		{
	//		ITBTrainSelectorGUI* gui = (*it).second;
	//		gui->updateRadioGroupCombo(trainListNames);
			if(NULL!=(*it)->TB_GetObserver())
				(*it)->TB_GetObserver()->updateRadioGroupCombo(trainListNames);
		}
	}
}
