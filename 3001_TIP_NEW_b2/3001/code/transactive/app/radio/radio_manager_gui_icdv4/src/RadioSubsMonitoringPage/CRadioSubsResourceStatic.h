
#pragma once
#include "afxwin.h"
#include "bus\radio\radio_bus\src\radiotypes.h"
#include "app\radio\radio_manager_gui_icdv4\src\resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
//#include "app\radio\radio_manager_gui\src\RadioSubsMonitoringPage\CRadioSubsMonitoringPage.h"


class CRadioSubsResourceStatic : public CStatic
{
public:
CRadioSubsResourceStatic();
~CRadioSubsResourceStatic();

void onDropResource(RadioResource& resource);

void setParentDlg(CDialog* parentDlg);

void setRadioResource(RadioResource& resource);
RadioResource getRadioResource();

	
afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	
afx_msg void OnSetFocus(CWnd* pOldWnd);

void onStopMonitoring();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CDialog* m_parentDlg;
	RadioResource m_resource;
};