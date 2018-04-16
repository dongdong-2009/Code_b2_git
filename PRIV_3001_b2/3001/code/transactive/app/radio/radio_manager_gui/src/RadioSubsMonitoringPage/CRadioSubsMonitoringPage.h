#pragma once
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app\radio\radio_manager_gui\src\RadioSubsMonitoringPage\CRadioSubsResourceStatic.h"
#include "app\radio\radio_manager_gui\src\RadioSubsMonitoringPage\CRadioSubsResourceText.h"
#include "app\radio\radio_manager_gui\src\RadioStatusPage\ResourceButton\CResourceButton.h"
//#include "app\radio\radio_manager_gui\src\RadioStatusPage\ResourceButton\RadioRadioResourceButtonStrategy.h"
#include "app\radio\radio_manager_gui\src\RadioStatusPage\ResourceButton\IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/IResourceButtonContainer.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "app\radio\radio_manager_gui\src\RadioStatusPage\CRadioStatusPage.h"
#include "bus\radio\radio_bus\src\radiotypes.h"
#include "afxwin.h"


// CRadioSubsMonitoringPage dialog

class CRadioSubsMonitoringPage : public CPropertyPage, public IResourceButtonContainer
{
	DECLARE_DYNAMIC(CRadioSubsMonitoringPage)

public:
	CRadioSubsMonitoringPage(RECT* parentRect = NULL, CRadioStatusPage* statusPage = NULL);
	virtual ~CRadioSubsMonitoringPage();

// Dialog Data
	enum { IDD = IDD_SUBS_MONITORING_PAGE };

	/**
	* Callback for WM_TIMER message. This will be used to toggle the "flashing" of
	* the buttons.
	*/
	void addTimerObserver(CWnd* observer);
	void removeTimerObserver(CWnd* observer);


	static const std::string DEFAULT_BUTTON_TEXT;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT onMonitoring(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onStopMonitoring(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg LRESULT onNotifiedDbUpdate(WPARAM pOldResource = 0, LPARAM pNewResource = 0);
	afx_msg LRESULT onNotifiedDbDelete(WPARAM pOldResource = 0, LPARAM lParam = 0);

	RECT m_parentRect;

	DECLARE_MESSAGE_MAP()
	//CResourceButton monitorBtn;
	CRadioSubsResourceStatic monitorStatic;
	CRadioSubsResourceText stationText;
	CRadioSubsResourceText talkgroupText;
	COLORREF whiteColor;
	CBrush whiteBrush;
	CRadioStatusPage* radioStatusPage;

	//CStatic staticSubscriber;
	//CStatic staticStation;
	//CStatic staticTalkgroup;

	//bool isPressed;
	//bool inMonitoring;
};
