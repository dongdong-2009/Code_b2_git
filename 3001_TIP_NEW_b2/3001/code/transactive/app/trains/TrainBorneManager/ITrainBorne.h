#ifndef _I_TRAIN_BORN_H_
#define _I_TRAIN_BORN_H_

#include <afxwin.h> 
#include "core/utilities/src/RunParams.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include <string>
#include <list>

#define WM_TB_PREPARED				WM_USER+1000
#define WM_TB_WILLCLOSE				WM_USER+1001
#define WM_TB_PROCESSSSTATUSCHANGED WM_USER+1002
#define WM_TB_CANAPPLY  		    WM_USER+1003
//using namespace std;

namespace TA_Base_Core
{
class IEntityData;
}
namespace TA_IRS_Bus
{
class ITrainSelectorGUI;
class TrainSelectorGUI;
class TrainSelector;
}
namespace TA_IRS_App
{

class ITrainBorne : public CDialog
{
public:
	enum procStatus
	{
		idle,
		processing,
		error
	};
	ITrainBorne(UINT templateID,CWnd* pParent)
		:CDialog(templateID,pParent),
	m_TemplateID( templateID),
	m_parent(pParent),m_parentGUI(NULL),m_parentSelector(NULL),m_observer(NULL)
	{
	};

	ITrainBorne(UINT templateID,CWnd* pParent,std::string sessionID)
		:CDialog(templateID,pParent),
	m_TemplateID( templateID),
	m_parent(pParent),
	m_sessionID(sessionID),m_parentGUI(NULL),m_parentSelector(NULL),m_observer(NULL)
	{
	};
//	virtual void TB_TrainSelected(unsigned long trainID)=0;
	virtual bool TB_DoApply()=0;
	
	virtual void TB_TrainSelected( TA_IRS_Bus::CommonTypes::TrainIdList selList) {  }
	virtual void TB_Prepared() 
	{
		::SendMessage(m_parent->m_hWnd,WM_TB_PREPARED,(WPARAM)(m_AppName.c_str()),0l);
	} 
	virtual void TB_WillClose() 
	{
		::SendMessage(m_parent->m_hWnd,WM_TB_WILLCLOSE,(WPARAM)(m_AppName.c_str()),0l);
	}

	virtual void TB_ProcessStatusChanged(procStatus status) 
	{
		::SendMessage(m_parent->m_hWnd,WM_TB_PROCESSSSTATUSCHANGED,(WPARAM)(m_AppName.c_str()),(LPARAM)status);
	}
	virtual void TB_PageActived(bool bActived) {};
	virtual inline void TB_SetAppName(const std::string& name) {m_AppName = name;};
	virtual inline std::string TB_GetAppName() {return m_AppName;};
	virtual inline UINT TB_GetTemplateID() {return m_TemplateID;};
	virtual inline void TB_SetTemplateID(UINT templateID) {m_TemplateID=templateID;};
	virtual inline void TB_SetParentWnd(const CWnd* wnd) {m_parent = const_cast<CWnd*>(wnd);};
	virtual inline void TB_SetSessionID(std::string sessionID) {m_sessionID = sessionID;};
	virtual void TB_SetParam(const char* name, const char* value) {TA_Base_Core::RunParams::getInstance().set( name,value );}
	
	/**
	 TB_SetParentGUI - set the ITrainSelectorGUI object of parent GUI,which can be used to visit parent window's ITrainSelectorGUI object 
	 @param selector - input TA_IRS_Bus::TrainSelectorGUI pointer to be stored
	 */
	virtual inline void TB_SetParentGUI(TA_IRS_Bus::TrainSelectorGUI* gui) {m_parentGUI = gui;};
	
	/**
	 TB_GetParentGUI - get the TrainSelectorGUI object of parent GUI,which can be used to visit parent window's ITrainSelectorGUI object 
	 
	 */
	virtual inline TA_IRS_Bus::TrainSelectorGUI* TB_GetParentGUI(){ return m_parentGUI;};
	/**
	 TB_SetParentSelector - set the TrainSelector object of parent GUI,which can be used to visit parent window's ITrainSelector object 
	 @param selector - input TA_IRS_Bus::ITrainSelector pointer to be stored
	 */
	virtual inline void TB_SetParentSelector(TA_IRS_Bus::TrainSelector* selector) {m_parentSelector = selector;};

	/**
	 TB_GetParentSelector - get the TrainSelector object of parent GUI,which can be used to visit parent window's ITrainSelector object 
	 
	 */
	virtual inline TA_IRS_Bus::TrainSelector* TB_GetParentSelector(){ return m_parentSelector;};

	/**
	 TB_SetObserver - set the TrainSelectorGUI object included in the this ITrainBorne object
	 @param gui - input TA_IRS_Bus::ITrainSelectorGUI pointer to be stored
	 */
	virtual inline void TB_SetObserver(TA_IRS_Bus::ITrainSelectorGUI* gui=NULL) {m_observer = gui;};
	
	/**
	 TB_GetObserver - return the TrainSelectorGUI object included in the this ITrainBorne object
	 */
	virtual inline TA_IRS_Bus::ITrainSelectorGUI* TB_GetObserver(){ return m_observer;};
	
	/**
	 TB_CheckEntity - this function is called once ITrainBorne object is created,in order to supply a chance for ITrainBorne object to check if this application GUI entity is allowed
	 @param guiEntity - input application GUI entity to be checked
	 */
	virtual bool TB_CheckEntity(TA_Base_Core::IEntityData* guiEntity){return true;};
	
	/**
	 TB_CanApply - info the parent UI to enable of disable the Apply button 
	 @param bCanApplyNow - input 
	 */
	virtual void TB_CanApply(bool bCanApplyNow)
	{
		::SendMessage(m_parent->m_hWnd,WM_TB_CANAPPLY,(WPARAM)(m_AppName.c_str()),(LPARAM)bCanApplyNow);
	};
	virtual void TB_MouseAction(UINT action,UINT flag,CPoint pt) {};
public:
	UINT m_TemplateID;
protected:
	std::string m_AppName;
//	CWnd* m_wnd;
	CWnd* m_parent;
	
	std::string m_sessionID;
	TA_IRS_Bus::TrainSelectorGUI* m_parentGUI;
	TA_IRS_Bus::TrainSelector* m_parentSelector;
	TA_IRS_Bus::ITrainSelectorGUI* m_observer;
};

}

#endif
