#pragma once
#include "afxwin.h"

#include "app\radio\radio_manager_gui_icdv4\src\RadioSubsMonitoringPage\CRadioSubsResourceText.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

CRadioSubsResourceText::CRadioSubsResourceText() 
{
}

CRadioSubsResourceText::~CRadioSubsResourceText()
{
}

BEGIN_MESSAGE_MAP(CRadioSubsResourceText, CEdit)
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


void CRadioSubsResourceText::OnRButtonUp(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnRButtonUp");
	FUNCTION_EXIT;
}

void CRadioSubsResourceText::OnSetFocus(CWnd* pOldWnd)
{
	FUNCTION_ENTRY("OnSetFocus");
	FUNCTION_EXIT;
}