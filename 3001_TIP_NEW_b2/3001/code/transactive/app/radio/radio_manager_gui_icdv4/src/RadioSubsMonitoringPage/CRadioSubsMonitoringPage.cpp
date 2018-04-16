// CRadioSubsMonitoringPage.cpp : implementation file
//

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app\radio\radio_manager_gui_icdv4\src\RadioSubsMonitoringPage\CRadioSubsMonitoringPage.h"

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

	inMonitoring = false;

	FUNCTION_EXIT;

}

CRadioSubsMonitoringPage::~CRadioSubsMonitoringPage()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_START_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_STOP_SUBS_MONITORING);
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
	ON_WM_TIMER()
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

	//monitorBtn.setButtonReference(CResourceButton::RADIO_BUTTON_REFERENCE);
	//IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
	////RadioMonitorButtonStrategy* pTempResourceButtonStrategy = new RadioMonitorButtonStrategy();
	//monitorBtn.setResourceButtonStrategy(pTempResourceButtonStrategy);
	//pTempResourceButtonStrategy->initialise(&monitorBtn);
	//
	////monitorBtn.SetWindowText(DEFAULT_BUTTON_TEXT.c_str());
	//monitorBtn.setResourceButtonContainer(radioStatusPage);
	//monitorBtn.setProfileButton(0);
	//RadioResource* resource = new RadioResource();
	//monitorBtn.setRadioResource(*resource);
	//radioStatusPage->setRadioMonitorBtn(&monitorBtn);

	//staticSubscriber.SetWindowTextA("Monitored Subscriber");

	int interval = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::FLASH_BUTTON_INTERVAL);

	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_START_SUBS_MONITORING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_STOP_SUBS_MONITORING);

	if (interval == 0)
	{
		interval = 500; // ms
	}

	SetTimer(1,interval,NULL); 

	FUNCTION_EXIT;
	return TRUE;
}

HBRUSH CRadioSubsMonitoringPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	FUNCTION_ENTRY("OnCtlColor");
	HBRUSH hbr; 

	//switch (nCtlColor) 
	//{ 
	//	// process my edit controls by ID.
	//case CTLCOLOR_EDIT:
	//case CTLCOLOR_MSGBOX:
	switch (pWnd->GetDlgCtrlID())
	{     			
	case IDC_SEL_TG_EDIT:   
	case IDC_AFFILIATED_EDIT:         
		pDC->SetBkColor(whiteColor);  
		//pDC->SetTextColor(textcolor);  // change the text color
		hbr = (HBRUSH) whiteBrush;    
		break;    
		//pDC->SetBkMode(TRANSPARENT);  
		//pDC->SetTextColor(textcolor);  
		//hbr = (HBRUSH) m_redbrush;    
	case IDC_MONITORING_TEXT:
		pDC->SetBkColor(RGB(255,255,255));
		hbr = (HBRUSH)RGB(255,255,255);
		break;
	default:
		hbr=CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
	}  
		//break;
	//case CTLCOLOR_STATIC:
	//default:
	//	hbr=CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
	//}

	return hbr; // return brush
	FUNCTION_EXIT;
}


// CRadioSubsMonitoringPage message handlers

LRESULT CRadioSubsMonitoringPage::onMonitoring(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onMonitoring");

	inMonitoring = true;

	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioSubsMonitoringPage::onStopMonitoring(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onStopMonitoring");

	inMonitoring = false;
	stationText.SetWindowTextA("");
	talkgroupText.SetWindowTextA("");

	FUNCTION_EXIT;
	return 0;
}

void CRadioSubsMonitoringPage::OnTimer(UINT nIDEvent) 
{
	try {
		if (inMonitoring) {
			MonitorStatus mStatus;
			if (monitorStatic.getRadioResource().id != 0) {
				mStatus = The_RadioSystemStatus::instance()->requestRadioCheck(monitorStatic.getRadioResource());			

				stationText.SetWindowText(mStatus.first.c_str());
				talkgroupText.SetWindowText(mStatus.second.c_str());
			}
		}
	}
	catch(TA_Base_Core::TransactiveException& e)
    {
		inMonitoring = false;
		monitorStatic.SetWindowText("");
		stationText.SetWindowText("");
		talkgroupText.SetWindowText("");
		::MessageBox(NULL, "Agent cannot be contacted", "Error connecting", MB_OK|MB_ICONERROR);
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
	
	CPropertyPage::OnTimer(nIDEvent);
}

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