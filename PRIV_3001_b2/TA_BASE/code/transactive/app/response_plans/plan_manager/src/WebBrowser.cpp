/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/WebBrowser.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "webbrowser.h"

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser properties

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser operations

void CWebBrowser::GoBack()
{
    FUNCTION_ENTRY( "GoBack" );

    InvokeHelper(0x64, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::GoForward()
{
    FUNCTION_ENTRY( "GoForward" );

    InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::GoHome()
{
    FUNCTION_ENTRY( "GoHome" );

    InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::GoSearch()
{
    FUNCTION_ENTRY( "GoSearch" );

    InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::Navigate(LPCTSTR URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers)
{
    FUNCTION_ENTRY( "Navigate" );

    static BYTE parms[] =
        VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
    InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         URL, Flags, TargetFrameName, PostData, Headers);

    FUNCTION_EXIT;
}


void CWebBrowser::Refresh()
{
    FUNCTION_ENTRY( "Refresh" );

    InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::Refresh2(VARIANT* Level)
{
    FUNCTION_ENTRY( "Refresh2" );

    static BYTE parms[] =
        VTS_PVARIANT;
    InvokeHelper(0x69, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         Level);

    FUNCTION_EXIT;
}


void CWebBrowser::Stop()
{
    FUNCTION_ENTRY( "Stop" );

    InvokeHelper(0x6a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


LPDISPATCH CWebBrowser::GetApplication()
{
    FUNCTION_ENTRY( "GetApplication" );

    LPDISPATCH result;
    InvokeHelper(0xc8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


LPDISPATCH CWebBrowser::GetParent()
{
    FUNCTION_ENTRY( "GetParent" );

    LPDISPATCH result;
    InvokeHelper(0xc9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


LPDISPATCH CWebBrowser::GetContainer()
{
    FUNCTION_ENTRY( "GetContainer" );

    LPDISPATCH result;
    InvokeHelper(0xca, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


LPDISPATCH CWebBrowser::GetDocument()
{
    FUNCTION_ENTRY( "GetDocument" );

    LPDISPATCH result;
    InvokeHelper(0xcb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


BOOL CWebBrowser::GetTopLevelContainer()
{
    FUNCTION_ENTRY( "GetTopLevelContainer" );

    BOOL result;
    InvokeHelper(0xcc, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


CString CWebBrowser::GetType()
{
    FUNCTION_ENTRY( "GetType" );

    CString result;
    InvokeHelper(0xcd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


long CWebBrowser::GetLeft()
{
    FUNCTION_ENTRY( "GetLeft" );

    long result;
    InvokeHelper(0xce, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetLeft(long nNewValue)
{
    FUNCTION_ENTRY( "SetLeft" );

    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0xce, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         nNewValue);

    FUNCTION_EXIT;
}


long CWebBrowser::GetTop()
{
    FUNCTION_ENTRY( "GetTop" );

    long result;
    InvokeHelper(0xcf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetTop(long nNewValue)
{
    FUNCTION_ENTRY( "SetTop" );

    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0xcf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         nNewValue);

    FUNCTION_EXIT;
}


long CWebBrowser::GetWidth()
{
    FUNCTION_ENTRY( "GetWidth" );

    long result;
    InvokeHelper(0xd0, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetWidth(long nNewValue)
{
    FUNCTION_ENTRY( "SetWidth" );

    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0xd0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         nNewValue);

    FUNCTION_EXIT;
}


long CWebBrowser::GetHeight()
{
    FUNCTION_ENTRY( "GetHeight" );

    long result;
    InvokeHelper(0xd1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetHeight(long nNewValue)
{
    FUNCTION_ENTRY( "SetHeight" );

    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0xd1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         nNewValue);

    FUNCTION_EXIT;
}


CString CWebBrowser::GetLocationName()
{
    FUNCTION_ENTRY( "GetLocationName" );

    CString result;
    InvokeHelper(0xd2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


CString CWebBrowser::GetLocationURL()
{
    FUNCTION_ENTRY( "GetLocationURL" );

    CString result;
    InvokeHelper(0xd3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


BOOL CWebBrowser::GetBusy()
{
    FUNCTION_ENTRY( "GetBusy" );

    BOOL result;
    InvokeHelper(0xd4, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::Quit()
{
    FUNCTION_ENTRY( "Quit" );

    InvokeHelper(0x12c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

    FUNCTION_EXIT;
}


void CWebBrowser::ClientToWindow(long* pcx, long* pcy)
{
    FUNCTION_ENTRY( "ClientToWindow" );

    static BYTE parms[] =
        VTS_PI4 VTS_PI4;
    InvokeHelper(0x12d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         pcx, pcy);

    FUNCTION_EXIT;
}


void CWebBrowser::PutProperty(LPCTSTR Property_, const VARIANT& vtValue)
{
    FUNCTION_ENTRY( "PutProperty" );

    static BYTE parms[] =
        VTS_BSTR VTS_VARIANT;
    InvokeHelper(0x12e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         Property_, &vtValue);

    FUNCTION_EXIT;
}


VARIANT CWebBrowser::GetProperty_(LPCTSTR Property_)
{
    FUNCTION_ENTRY( "GetProperty_" );

    VARIANT result;
    static BYTE parms[] =
        VTS_BSTR;
    InvokeHelper(0x12f, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
        Property_);

    FUNCTION_EXIT;
    return result;
}


CString CWebBrowser::GetName()
{
    FUNCTION_ENTRY( "GetName" );

    CString result;
    InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


long CWebBrowser::GetHwnd()
{
    FUNCTION_ENTRY( "GetHwnd" );

    long result;
    InvokeHelper(DISPID_HWND, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


CString CWebBrowser::GetFullName()
{
    FUNCTION_ENTRY( "GetFullName" );

    CString result;
    InvokeHelper(0x190, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


CString CWebBrowser::GetPath()
{
    FUNCTION_ENTRY( "GetPath" );

    CString result;
    InvokeHelper(0x191, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


BOOL CWebBrowser::GetVisible()
{
    FUNCTION_ENTRY( "GetVisible" );

    BOOL result;
    InvokeHelper(0x192, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetVisible(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetVisible" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x192, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetStatusBar()
{
    FUNCTION_ENTRY( "GetStatusBar" );

    BOOL result;
    InvokeHelper(0x193, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetStatusBar(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetStatusBar" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x193, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


CString CWebBrowser::GetStatusText()
{
    FUNCTION_ENTRY( "GetStatusText" );

    CString result;
    InvokeHelper(0x194, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetStatusText(LPCTSTR lpszNewValue)
{
    FUNCTION_ENTRY( "SetStatusText" );

    static BYTE parms[] =
        VTS_BSTR;
    InvokeHelper(0x194, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         lpszNewValue);

    FUNCTION_EXIT;
}


long CWebBrowser::GetToolBar()
{
    FUNCTION_ENTRY( "GetToolBar" );

    long result;
    InvokeHelper(0x195, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetToolBar(long nNewValue)
{
    FUNCTION_ENTRY( "SetToolBar" );

    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0x195, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         nNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetMenuBar()
{
    FUNCTION_ENTRY( "GetMenuBar" );

    BOOL result;
    InvokeHelper(0x196, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetMenuBar(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetMenuBar" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x196, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetFullScreen()
{
    FUNCTION_ENTRY( "GetFullScreen" );

    BOOL result;
    InvokeHelper(0x197, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetFullScreen(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetFullScreen" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x197, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


void CWebBrowser::Navigate2(VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers)
{
    FUNCTION_ENTRY( "Navigate2" );

    static BYTE parms[] =
        VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
    InvokeHelper(0x1f4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         URL, Flags, TargetFrameName, PostData, Headers);

    FUNCTION_EXIT;
}


long CWebBrowser::QueryStatusWB(long cmdID)
{
    FUNCTION_ENTRY( "QueryStatusWB" );

    long result;
    static BYTE parms[] =
        VTS_I4;
    InvokeHelper(0x1f5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
        cmdID);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::ExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut)
{
    FUNCTION_ENTRY( "ExecWB" );

    static BYTE parms[] =
        VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT;
    InvokeHelper(0x1f6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         cmdID, cmdexecopt, pvaIn, pvaOut);

    FUNCTION_EXIT;
}


void CWebBrowser::ShowBrowserBar(VARIANT* pvaClsid, VARIANT* pvarShow, VARIANT* pvarSize)
{
    FUNCTION_ENTRY( "ShowBrowserBar" );

    static BYTE parms[] =
        VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
    InvokeHelper(0x1f7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
         pvaClsid, pvarShow, pvarSize);

    FUNCTION_EXIT;
}


long CWebBrowser::GetReadyState()
{
    FUNCTION_ENTRY( "GetReadyState" );

    long result;
    InvokeHelper(DISPID_READYSTATE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


BOOL CWebBrowser::GetOffline()
{
    FUNCTION_ENTRY( "GetOffline" );

    BOOL result;
    InvokeHelper(0x226, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetOffline(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetOffline" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x226, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetSilent()
{
    FUNCTION_ENTRY( "GetSilent" );

    BOOL result;
    InvokeHelper(0x227, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetSilent(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetSilent" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x227, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetRegisterAsBrowser()
{
    FUNCTION_ENTRY( "GetRegisterAsBrowser" );

    BOOL result;
    InvokeHelper(0x228, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetRegisterAsBrowser(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetRegisterAsBrowser" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x228, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetRegisterAsDropTarget()
{
    FUNCTION_ENTRY( "GetRegisterAsDropTarget" );

    BOOL result;
    InvokeHelper(0x229, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetRegisterAsDropTarget(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetRegisterAsDropTarget" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x229, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetTheaterMode()
{
    FUNCTION_ENTRY( "GetTheaterMode" );

    BOOL result;
    InvokeHelper(0x22a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetTheaterMode(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetTheaterMode" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x22a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetAddressBar()
{
    FUNCTION_ENTRY( "GetAddressBar" );

    BOOL result;
    InvokeHelper(0x22b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetAddressBar(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetAddressBar" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x22b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


BOOL CWebBrowser::GetResizable()
{
    FUNCTION_ENTRY( "GetResizable" );

    BOOL result;
    InvokeHelper(0x22c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);

    FUNCTION_EXIT;
    return result;
}


void CWebBrowser::SetResizable(BOOL bNewValue)
{
    FUNCTION_ENTRY( "SetResizable" );

    static BYTE parms[] =
        VTS_BOOL;
    InvokeHelper(0x22c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
         bNewValue);

    FUNCTION_EXIT;
}


