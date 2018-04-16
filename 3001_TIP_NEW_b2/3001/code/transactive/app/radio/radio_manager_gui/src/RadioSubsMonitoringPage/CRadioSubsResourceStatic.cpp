#pragma once
#include "afxwin.h"

#include "app\radio\radio_manager_gui\src\RadioSubsMonitoringPage\CRadioSubsResourceStatic.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

CRadioSubsResourceStatic::CRadioSubsResourceStatic() 
{
}

CRadioSubsResourceStatic::~CRadioSubsResourceStatic()
{
}

BEGIN_MESSAGE_MAP(CRadioSubsResourceStatic, CStatic)
	ON_WM_RBUTTONUP()	
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_STOP_MONITOR_MENU, onStopMonitoring)
END_MESSAGE_MAP()

void CRadioSubsResourceStatic::setParentDlg(CDialog* parentDlg)
{
	m_parentDlg = parentDlg;
}

void CRadioSubsResourceStatic::setRadioResource(RadioResource& resource)
{
	FUNCTION_ENTRY("setRadioResource");
	m_resource = resource;
	FUNCTION_EXIT;
}

RadioResource CRadioSubsResourceStatic::getRadioResource()
{
	FUNCTION_ENTRY("getRadioReseource");
	FUNCTION_EXIT;
	return m_resource;
}

void CRadioSubsResourceStatic::onDropResource(RadioResource& resource) 
{
	if (RR_RADIO == resource.type) {
		setRadioResource(resource);
		SetWindowText(resource.alias.c_str());
		The_RadioOperations::instance()->startSubsMonitoring(resource);
	}
	//((CRadioSubsMonitoringPage*)m_parentDlg)->onMonitoring();
}

void CRadioSubsResourceStatic::OnRButtonUp(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnRButtonUp");

	if (m_resource.type != RR_NIL) {
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(0, ID_STOP_MONITOR_MENU, "Stop Monitoring");

		CPoint mousePoint;
		::GetCursorPos(&mousePoint);

		CRect rect;
		this->GetWindowRect(&rect);
		menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, (CWnd*)this, rect);

		menu.DestroyMenu();
	}

	FUNCTION_EXIT;
}

void CRadioSubsResourceStatic::OnSetFocus(CWnd* pOldWnd)
{
	FUNCTION_ENTRY("OnSetFocus");
	FUNCTION_EXIT;
}

void CRadioSubsResourceStatic::onStopMonitoring()
{
	FUNCTION_ENTRY("onStopMonitoring");
	
	SetWindowText("");
	The_RadioOperations::instance()->stopSubsMonitoring(m_resource);
	RadioResource r;
	setRadioResource(r);
	
	FUNCTION_EXIT;
}