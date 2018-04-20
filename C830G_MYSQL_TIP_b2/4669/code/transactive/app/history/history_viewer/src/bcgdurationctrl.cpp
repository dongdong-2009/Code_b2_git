/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/bcgdurationctrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "bcgdurationctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGDurationCtrl

IMPLEMENT_DYNCREATE(CBCGDurationCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CBCGDurationCtrl properties

BOOL CBCGDurationCtrl::GetSpinButton()
{
	BOOL result;
	GetProperty(0x1, VT_BOOL, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetSpinButton(BOOL propVal)
{
	SetProperty(0x1, VT_BOOL, propVal);
}

CString CBCGDurationCtrl::GetDaysLabel()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetDaysLabel(LPCTSTR propVal)
{
	SetProperty(0x2, VT_BSTR, propVal);
}

CString CBCGDurationCtrl::GetHoursLabel()
{
	CString result;
	GetProperty(0x3, VT_BSTR, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetHoursLabel(LPCTSTR propVal)
{
	SetProperty(0x3, VT_BSTR, propVal);
}

CString CBCGDurationCtrl::GetMinutesLabel()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetMinutesLabel(LPCTSTR propVal)
{
	SetProperty(0x4, VT_BSTR, propVal);
}

BOOL CBCGDurationCtrl::GetShowDays()
{
	BOOL result;
	GetProperty(0x5, VT_BOOL, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetShowDays(BOOL propVal)
{
	SetProperty(0x5, VT_BOOL, propVal);
}

BOOL CBCGDurationCtrl::GetShowHoursMinutes()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetShowHoursMinutes(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}

long CBCGDurationCtrl::GetTotalSeconds()
{
	long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetTotalSeconds(long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

long CBCGDurationCtrl::Get_TotalSeconds()
{
	long result;
	GetProperty(0x0, VT_I4, (void*)&result);
	return result;
}

void CBCGDurationCtrl::Set_TotalSeconds(long propVal)
{
	SetProperty(0x0, VT_I4, propVal);
}

BOOL CBCGDurationCtrl::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void CBCGDurationCtrl::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CBCGDurationCtrl operations

void CBCGDurationCtrl::SizeToContent()
{
	//InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CBCGDurationCtrl::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL CBCGDurationCtrl::Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext)
{ 
    return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
}





