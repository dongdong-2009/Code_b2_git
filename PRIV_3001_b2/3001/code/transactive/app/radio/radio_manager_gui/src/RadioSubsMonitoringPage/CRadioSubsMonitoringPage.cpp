// CRadioSubsMonitoringPage.cpp : implementation file
//

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "app\radio\radio_manager_gui\src\RadioSubsMonitoringPage\CRadioSubsMonitoringPage.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpMethodTimeoutException.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



// CRadioSubsMonitoringPage dialog

IMPLEMENT_DYNAMIC(CRadioSubsMonitoringPage, CPropertyPage)

const std::string CRadioSubsMonitoringPage::DEFAULT_BUTTON_TEXT = "Portable 1";


CRadioSubsMonitoringPage::CRadioSubsMonitoringPage(RECT* parentRect, CRadioStatusPage* statusPage)
	: CPropertyPage(CRadioSubsMonitoringPage::IDD)
{
	FUNCTION_ENTRY("CRadioSubsMonitoringPage");

    if (parentRect != NULL)
    {
        m_parentRect = *parentRect;
    }

	if (statusPage != NULL)
	{
		radioStatusPage = statusPage;
	}

	FUNCTION_EXIT;

}

CRadioSubsMonitoringPage::~CRadioSubsMonitoringPage()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_START_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_STOP_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_DELETE);
}

void CRadioSubsMonitoringPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_MONITORING_BTN, monitorBtn);
	DDX_Control(pDX, IDC_MONITORING_TEXT, monitorStatic);
	DDX_Control(pDX, IDC_AFFILIATED_EDIT, stationText);
	DDX_Control(pDX, IDC_SEL_TG_EDIT, talkgroupText);
	//DDX_Control(pDX, IDC_AFFILIATED_STATIC, staticStation);
	//DDX_Control(pDX, IDC_SEL_TG_STATIC, staticTalkgroup);
	//DDX_Control(pDX, IDC_RESULT_STATIC, staticSubscriber);
}


BEGIN_MESSAGE_MAP(CRadioSubsMonitoringPage, CPropertyPage)	
	ON_WM_CTLCOLOR()
	//ON_BN_CLICKED(IDC_MONITORING_BTN, onMonitoring)
	ON_MESSAGE(WM_START_SUBS_MONITORING, onMonitoring)
	ON_MESSAGE(WM_STOP_SUBS_MONITORING, onStopMonitoring)
	ON_MESSAGE(WM_DB_UPDATE, onNotifiedDbUpdate)
	ON_MESSAGE(WM_DB_DELETE, onNotifiedDbDelete)
	//ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CRadioSubsMonitoringPage::OnInitDialog()
{
	FUNCTION_ENTRY("CRadioSubsMonitoringPage");
	CPropertyPage::OnInitDialog();

	whiteColor = ::GetSysColor(COLOR_3DFACE);//RGB(255,255,255);
	
	whiteBrush.CreateSolidBrush(whiteColor);
	
	monitorStatic.setParentDlg(this);

	stationText.SetReadOnly(TRUE);
	talkgroupText.SetReadOnly(TRUE);

	//int interval = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::FLASH_BUTTON_INTERVAL);

	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_START_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_STOP_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_DELETE);

	//if (interval == 0)
	//{
	//	interval = 500; // ms
	//}

	//SetTimer(1,interval,NULL); 

	FUNCTION_EXIT;
	return TRUE;
}

HBRUSH CRadioSubsMonitoringPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	FUNCTION_ENTRY("OnCtlColor");
	HBRUSH hbr; 

	switch (pWnd->GetDlgCtrlID())
	{     			
	case IDC_SEL_TG_EDIT:   
	case IDC_AFFILIATED_EDIT:         
		pDC->SetBkColor(whiteColor);  
		//pDC->SetTextColor(textcolor);  // change the text color
		hbr = (HBRUSH) whiteBrush;    
		break;
	case IDC_MONITORING_TEXT:
		pDC->SetBkColor(RGB(255,255,255));
		hbr = (HBRUSH)RGB(255,255,255);
		break;
	default:
		hbr=CPropertyPage::OnCtlColor(pDC,pWnd,nCtlColor);
		break;
	}  
	FUNCTION_EXIT;
	return hbr; // return brush
	
}


// CRadioSubsMonitoringPage message handlers

LRESULT CRadioSubsMonitoringPage::onMonitoring(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onMonitoring");

	try {
		MonitorStatus mStatus;
		if (monitorStatic.getRadioResource().id != 0) {
			mStatus = The_RadioSystemStatus::instance()->requestRadioCheck(monitorStatic.getRadioResource());			

			stationText.SetWindowText(mStatus.first.c_str());
			talkgroupText.SetWindowText(mStatus.second.c_str());
		}
	}
	catch(TA_IRS_App::TcpException& e)
	{
		monitorStatic.SetWindowText("");
		stationText.SetWindowText("");
		talkgroupText.SetWindowText("");
		AddErrorMessage("Unable to establish Connection with the Radio System");
        LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		monitorStatic.SetWindowText("");
		stationText.SetWindowText("");
		talkgroupText.SetWindowText("");
		AddErrorMessage("Agent cannot be contacted");
        LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
	}
	catch(TA_Base_Core::TransactiveException& e)
    {
		monitorStatic.SetWindowText("");
		stationText.SetWindowText("");
		talkgroupText.SetWindowText("");
		AddErrorMessage("Unable to perform requested operation");
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }

	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioSubsMonitoringPage::onStopMonitoring(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onStopMonitoring");

	stationText.SetWindowTextA("");
	talkgroupText.SetWindowTextA("");

	FUNCTION_EXIT;
	return 0;
}

//void CRadioSubsMonitoringPage::OnTimer(UINT nIDEvent) 
//{
//	try {
//		if (inMonitoring) {
//			
//		}
//	}
//	
//	CPropertyPage::OnTimer(nIDEvent);
//}

void CRadioSubsMonitoringPage::addTimerObserver(CWnd* observer)
{

	//std::set<CWnd*>::iterator iter;
	//
	//// check if observer already exists
	//iter = m_timerSubscribedButtonCollection.find(observer);

	//if ( iter == m_timerSubscribedButtonCollection.end() )
	//{
	//	m_timerSubscribedButtonCollection.insert(observer);	
	//}

	return;
}


void CRadioSubsMonitoringPage::removeTimerObserver(CWnd* observer)
{
	FUNCTION_ENTRY("removeTimverObserver");
	
	//std::set<CWnd*>::iterator iter;
	//
	//iter =m_timerSubscribedButtonCollection.find(observer);

	//if ( iter != m_timerSubscribedButtonCollection.end() )
	//{
	//	m_timerSubscribedButtonCollection.erase(iter);
	//}
	

	FUNCTION_EXIT;

}
//
//
//bool CRadioSubsMonitoringPage::isMonitored(int nProfile,const RadioResource& resource)
//{
//	CResourceButton *temp = NULL;
//	RadioResource tempResource = monitorBtn.getRadioResource();
//
//	if (nProfile == 0)//only user-defined button
//	{
//		if (RR_NIL == tempResource.type) return false;
//
//		if (resource.id == tempResource.id && 
//			resource.ssi == tempResource.ssi )
//		{			
//			if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
//			{
//				return true;
//			}
//		}
//	}
//	
//    return false;
//}

LRESULT CRadioSubsMonitoringPage::onNotifiedDbUpdate(WPARAM pOldResource, LPARAM pNewResource)
{
	FUNCTION_ENTRY("onNotifiedDbUpdate");
	RadioResource* oldRes = (RadioResource*) pOldResource;
	RadioResource* newRes = (RadioResource*) pNewResource;
	if (RR_RADIO == oldRes->type && oldRes->ssi == monitorStatic.getRadioResource().ssi) {
		monitorStatic.setRadioResource(*newRes);
		monitorStatic.SetWindowText(newRes->alias.c_str());
	}
	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioSubsMonitoringPage::onNotifiedDbDelete(WPARAM pOldResource, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbDelete");	
	RadioResource* res = (RadioResource*) pOldResource;
	if (RR_RADIO == res->type && res->ssi == monitorStatic.getRadioResource().ssi) {
		RadioResource r;
		r.initialise();
		monitorStatic.setRadioResource(r);
		monitorStatic.SetWindowText("");
		stationText.SetWindowTextA("");
		talkgroupText.SetWindowTextA("");
	}
	FUNCTION_EXIT;
	return 0;
}

void CRadioSubsMonitoringPage::OnPaint() 
{
  	CPaintDC dc(this);	
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = NULL;
	pOldBrush = dc.SelectObject(&b);
	CRect r(0,0,0,0);
	GetClientRect(&r);
	dc.FillRect(&r, &b);

	CPaintDC dcR(GetDlgItem(IDC_RESULT_STATIC));
	CBrush bR;
	bR.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rR(0,0,0,0);
	GetDlgItem(IDC_RESULT_STATIC)->GetClientRect(&rR);
	dcR.SetBkMode(TRANSPARENT);
	dcR.FillRect(&rR,&bR);
	dcR.DrawText("Monitored Subscriber", &rR, DT_BOTTOM);

	CPaintDC dcS(GetDlgItem(IDC_AFFILIATED_STATIC));
	CBrush bS;
	bS.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rS(0,0,0,0);
	GetDlgItem(IDC_AFFILIATED_STATIC)->GetClientRect(&rS);
	dcS.SetBkMode(TRANSPARENT);
	dcS.FillRect(&rS,&bS);
	dcS.DrawText("Affiliated Station", &rS, DT_BOTTOM);

	CPaintDC dcTg(GetDlgItem(IDC_SEL_TG_STATIC));
	CBrush bTg;
	bTg.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rTg(0,0,0,0);
	GetDlgItem(IDC_SEL_TG_STATIC)->GetClientRect(&rTg);
	dcTg.SetBkMode(TRANSPARENT);
	dcTg.FillRect(&rTg,&bTg);
	dcTg.DrawText("Selected Talkgroup", &rTg, DT_BOTTOM);
}