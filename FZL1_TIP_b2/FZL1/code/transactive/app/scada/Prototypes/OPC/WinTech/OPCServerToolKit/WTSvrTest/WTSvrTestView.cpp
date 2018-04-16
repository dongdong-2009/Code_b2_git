// WTSvrTestView.cpp : implementation of the CWTSvrTestView class
//

#include "stdafx.h"
#include "opcda.h"
#include "opc_ae.h"
#include "opcaedef.h"
#include "OPCERROR.h"
#include "WTOPCsvrAPI.h"
#include "WTOPCsvrextAPI.h"
#include "WTSvrTest.h"

#include "WTSvrTestDoc.h"
#include "WTSvrTestView.h"

#include "tagdlg.h"
#include "AeMsgDlg.h"
#include "mycallback.h"
#include "DelimitDlg.h"
#include "adduserdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {6764A030-070C-11d3-80D6-0060975858BE}
static const GUID CLSID_Svr = 
{ 0x6764a030, 0x70c, 0x11d3, { 0x80, 0xd6, 0x0, 0x60, 0x97, 0x58, 0x58, 0xbe } };



/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView
//
// Write Notification callback
// pass control to thge CView
// which contains the local tag list
//
void CALLBACK EXPORT WriteNotifyProc2(HANDLE Handle, VARIANT *pNewValue, DWORD *pDevError, CHAR *pUser)
{
	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();
//	AfxMessageBox (pUser, MB_OK);
	pApp->pView->NotificationHandler(Handle, pNewValue, pDevError);

}

void CALLBACK EXPORT DeviceReadProc(HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp)
{
	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();
	pApp->pView->DeviceReadHandler(Handle, pNewValue, pQuality, pTimestamp);

}


// Feb 28, 2000
//
// Code Added to allow dynamic Tag Creation
//
// If a client requests a tag which has not been previously defined
// this code will accept a callback notification from the WtOPCsvr.dll
// and create the tag on the fly.
//
// Unknown Tag callback
// pass control to the CView
// which contains the local tag list
//
void CALLBACK EXPORT UnknownTagProc (LPSTR Path, LPSTR Name)
{
	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();
	pApp->pView->UnknownTagHandler(Path, Name);

}

void CALLBACK EXPORT TagRemovedProc (HANDLE TagHandle, LPSTR Path, LPSTR Name)
{
	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();
	pApp->pView->RemovedTagHandler (TagHandle, Path, Name);

}


IMPLEMENT_DYNCREATE(CWTSvrTestView, CView)

BEGIN_MESSAGE_MAP(CWTSvrTestView, CView)
	//{{AFX_MSG_MAP(CWTSvrTestView)
	ON_COMMAND(ID_VIEW_DLLREVISION, OnViewDllrevision)
	ON_COMMAND(ID_EDIT_REGISTER, OnEditRegister)
	ON_COMMAND(ID_EDIT_UNREGISTER, OnEditUnregister)
	ON_COMMAND(ID_EDIT_ADDTAG, OnEditAddtag)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_EDIT_UPDATETAG, OnUpdateEditUpdatetag)
	ON_COMMAND(ID_EDIT_UPDATETAG, OnEditUpdatetag)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_DYNAMICTAGS, OnEditDynamictags)
	ON_COMMAND(ID_EDIT_FORCECLIENTREFRESH, OnEditForceclientrefresh)
	ON_COMMAND(ID_EDIT_USERAEMSG, OnEditUseraemsg)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_DELETETAG, OnEditDeletetag)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETETAG, OnUpdateEditDeletetag)
	ON_COMMAND(ID_FILE_SAVETAGLIST, OnFileSavetaglist)
	ON_COMMAND(ID_FILE_RESTORETAGLIST, OnFileRestoretaglist)
	ON_COMMAND(ID_EDIT_TAGDELIMITER, OnEditTagdelimiter)
	ON_COMMAND(ID_NTSECURITY_ENABLE, OnNtsecurityEnable)
	ON_UPDATE_COMMAND_UI(ID_NTSECURITY_ENABLE, OnUpdateNtsecurityEnable)
	ON_COMMAND(ID_NTSECURITY_DISABLE, OnNtsecurityDisable)
	ON_UPDATE_COMMAND_UI(ID_NTSECURITY_DISABLE, OnUpdateNtsecurityDisable)
	ON_COMMAND(ID_NTSECURITY_ADDUSER, OnNtsecurityAdduser)
	ON_UPDATE_COMMAND_UI(ID_NTSECURITY_ADDUSER, OnUpdateNtsecurityAdduser)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView construction/destruction

CWTSvrTestView::CWTSvrTestView()
{
	CWTSvrTestApp	*pApp;

	// Save a pointer to this CView in the App
	// (Lets everybody access this object)
	pApp = (CWTSvrTestApp *)AfxGetApp();
	pApp->pView = this;

	// SelectedIndex refers to the highlighted
	// CTag on the display
	SelectedIndex = -1;
	NTSecurityEnabled = FALSE;
}

CWTSvrTestView::~CWTSvrTestView()
{
	int	i;

	// delete all the CTags
	for (i=0; i<TagList.GetSize(); i++)
		delete (TagList.GetAt(i));
	TagList.RemoveAll();
}

BOOL CWTSvrTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


void CALLBACK EXPORT DisconnectProc(DWORD NumbrActiveClients)
{
	if (NumbrActiveClients == 0)
		{
		// If you want to end the server application
		// when the last client disconnects,
		// be sure to return from the callback
		// to release the client before shutting down.
		}
}

//
// Initilaze the WTOPCsvr Dll when the Window gets created
// save the initial screen and character sizes
//
int CWTSvrTestView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CDC	*pdc;
	TEXTMETRIC	tm; 
//	int	i;
//	CMyAECallback	*pCallback;

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ScreenX = lpCreateStruct->cx;
	ScreenY = lpCreateStruct->cy;

	pdc = GetDC();
 	pdc->GetTextMetrics (&tm);
	ReleaseDC(pdc);
	
	cxChar = tm.tmMaxCharWidth;
	cyChar = tm.tmHeight;

	// CLSID_Svr identifies this Server
	// (Created with guidgen.exe)
	// minimum server refresh rate is 250 ms
	InitWTOPCsvr ((BYTE *)&CLSID_Svr,250);

	EnableWriteNotification2 (&WriteNotifyProc2, TRUE);
	EnableDisconnectNotification (&DisconnectProc);
	EnableDeviceRead (&DeviceReadProc);

//	SetCaseSensitivity (FALSE);

//	for (i=0; i<5; i++)
//		AutoAddtag (i+1);

	//
	// If you want to override the default A&E operation of WtOPCsvr.dll
	// define your own callback object and overload the functions of interest.
//	pCallback = new (CMyAECallback);
//	SetAEServerCallback (TRUE, pCallback); 


	return 0;
}


void CWTSvrTestView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	ScreenX = cx;
	ScreenY = cy;
}

//
// Select a Tag by pressing left mouse button
//
void CWTSvrTestView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	idx;

	idx = point.y / cyChar;
	if (idx < TagList.GetSize())
		// mark current index as the selection
		SelectedIndex = idx;
	else
		SelectedIndex = -1;
	Invalidate(TRUE);
	
	CView::OnLButtonDown(nFlags, point);
}

//
// Edit selected tag on Dbl-Click
//
void CWTSvrTestView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnEditUpdatetag();
}

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView drawing

void CWTSvrTestView::OnDraw(CDC* pDC)
{
	int	i,nlines;
	CString tmpstr;
	CTag	*pTag;
	char	buf[80];
	int		MaxX;
	CSize	TextSize;
	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	CWTSvrTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//
	//	Display TagList
	//
	nlines = ScreenY / cyChar;
	MaxX = 0;
	// draw time and name first
	// find longest line to mark the x position
	// for drawing the values.
	for (i=0; i<nlines; i++)
		{
		if (i < TagList.GetSize())
			{
			pTag = (CTag *)TagList.GetAt(i);
			if (pTag == NULL)
				continue;
			if (i == SelectedIndex)
				{
				pDC->SelectStockObject (BLACK_PEN);
				pDC->SelectStockObject (BLACK_BRUSH);
				pDC->Rectangle (0, cyChar*i, ScreenX, cyChar*i+cyChar);
				pDC->SetTextColor(RGB(255,255,255));
				pDC->SetBkColor(RGB(0,0,0));
				}
			else
				{
				pDC->SetTextColor (RGB(0,0,0));
				pDC->SetBkColor (RGB(255,255,255));
				}
			tmpstr = pTag->Time.Format ("%H:%M:%S   ");
			tmpstr += pTag->Name;
    		TextSize = pDC->GetTextExtent (tmpstr, tmpstr.GetLength());
			if (TextSize.cx > MaxX)
				MaxX = TextSize.cx;
			pDC->TextOut (0, cyChar*i, tmpstr, tmpstr.GetLength());
			}
		}
	// line up values based on longest tag name
	for (i=0; i<nlines; i++)
		{
		if (i < TagList.GetSize())
			{
			pTag = (CTag *)TagList.GetAt(i);
			if (pTag == NULL)
				continue;
			if (i == SelectedIndex)
				{
				pDC->SetTextColor(RGB(255,255,255));
				pDC->SetBkColor(RGB(0,0,0));
				}
			else
				{
				pDC->SetTextColor (RGB(0,0,0));
				pDC->SetBkColor (RGB(255,255,255));
				}
			pApp->FormatVariant(&(pTag->Value), buf, 80);
			pDC->TextOut (MaxX + cxChar*2, cyChar*i, buf, strlen(buf));
			}
		}
}

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView printing

BOOL CWTSvrTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWTSvrTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWTSvrTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView diagnostics

#ifdef _DEBUG
void CWTSvrTestView::AssertValid() const
{
	CView::AssertValid();
}

void CWTSvrTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWTSvrTestDoc* CWTSvrTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWTSvrTestDoc)));
	return (CWTSvrTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestView message handlers

void CWTSvrTestView::OnViewDllrevision() 
{
	WORD	rev;	
	char buf[100] = {0};

	rev = WTOPCsvrRevision();
	sprintf (buf, "DLL Rev = %x", rev);
	AfxMessageBox (buf, MB_OK);

}

//
// Would generally register the server based on command
// line options during the InitInstance procedure in the App.
//
// This sample uses a menu option to write the registry entries.
//
void CWTSvrTestView::OnEditRegister() 
{
	CString	SvrName, SvrDescr, HelpPath;
	int	i;

	// get path to executable by parsing the help path
	// contained in CWinApp
	HelpPath = AfxGetApp()->m_pszHelpFilePath;
	i = HelpPath.ReverseFind('\\');
	HelpPath = HelpPath.Left(i+1);
	HelpPath += "WtSvrTest.exe"; 
	
	SvrName = "DLLTestSvr";
	SvrDescr = "OPC Server to Test WT Toolkit";
	UpdateRegistry ((BYTE *)&CLSID_Svr, SvrName, SvrDescr, HelpPath);
	
}

void CWTSvrTestView::OnEditUnregister() 
{
	CString	SvrName;

	SvrName = "DLLTestSvr";
	UnregisterServer ((BYTE *)&CLSID_Svr, SvrName);
	
}


//
// Add a Tag
// Specify a segmented name (ie. Plant1.ProcessA.Tag1)
// for a Hiearchial name space
//
void CWTSvrTestView::OnEditAddtag() 
{
	CTag	*pTag;
	CTagDlg	dlg;
	int	i;
	VARIANT	PropertyValue, TempV;
	WCHAR	*pWCHAR;
	CWTSvrTestApp	*pApp;
	SAFEARRAY	*pMyArray;
	SAFEARRAYBOUND MyBounds;
	HRESULT hr;
	long	lTemp;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	for (i=0; i<4; i++)
		{
		dlg.m_alarms[i] = 0.0;
		dlg.m_severity[i] = 200;
		dlg.m_enabled[i] = FALSE;
		}
	dlg.m_alarms[2] = 100.0;
	dlg.m_alarms[3] = 100.0;
	dlg.m_value.vt = VT_R4;
	dlg.m_value.fltVal = 0.0;

	dlg.m_ReadAccess = ACCESSLEVELNONE;
	dlg.m_WriteAccess = ACCESSLEVELNONE;
	
	dlg.m_doArray = FALSE;
	dlg.m_arraylength = 2;

	if (dlg.DoModal() != IDOK)
		return;


		pTag = new (CTag);
		pTag->Name = dlg.m_name;
		pTag->Description = dlg.m_description;
		pTag->Units = dlg.m_units;
		if (!dlg.m_doArray)
			hr = VariantCopy (&(pTag->Value), &(dlg.m_value));
		else
			{
			VariantInit(&TempV);
			MyBounds.cElements = dlg.m_arraylength;
			MyBounds.lLbound = 0;
			pMyArray = SafeArrayCreate (dlg.m_value.vt, 1, &MyBounds);
			for (i=0; i<dlg.m_arraylength; i++)
				{
				lTemp = i;
				switch (dlg.m_value.vt)
					{
					case VT_R4:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.fltVal);
						break;
					case VT_BOOL:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.boolVal);
						break;
					case VT_BSTR:
						SafeArrayPutElement (pMyArray, &lTemp, dlg.m_value.bstrVal);
						break;
					case VT_I2:
					default:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.intVal);
						break;
					}
				}
			V_VT(&TempV) = VT_ARRAY | dlg.m_value.vt;
			V_ARRAY(&TempV) = pMyArray;
			hr = VariantCopy (&(pTag->Value), &TempV);
			VariantClear (&TempV);
			}
		 
		pTag->Time = CTime::GetCurrentTime();
		for (i=0; i<4; i++)
			{
			pTag->alarms[i] = dlg.m_alarms[i];
			pTag->severity[i] = dlg.m_severity[i];
			pTag->enabled[i] = dlg.m_enabled[i];
			}
		pTag->ReadAccessLevel = dlg.m_ReadAccess;
		pTag->WriteAccessLevel = dlg.m_WriteAccess;

		pTag->Handle = CreateSecureTag (pTag->Name, pTag->Value, OPC_QUALITY_GOOD, pTag->ReadAccessLevel, pTag->WriteAccessLevel);

		TagList.Add ((CObject *)pTag);

		VariantInit(&PropertyValue);
		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Units);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 100, "EU Units", PropertyValue);
		VariantClear(&PropertyValue);

		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Description);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 101, "Item Description", PropertyValue);
		VariantClear(&PropertyValue);

		SetItemLevelAlarm (pTag->Handle, ID_LOLO_LIMIT, pTag->alarms[0], pTag->severity[0], pTag->enabled[0]);
		SetItemLevelAlarm (pTag->Handle, ID_LO_LIMIT, pTag->alarms[1], pTag->severity[1], pTag->enabled[1]);
		SetItemLevelAlarm (pTag->Handle, ID_HI_LIMIT, pTag->alarms[2], pTag->severity[2], pTag->enabled[2]);
		SetItemLevelAlarm (pTag->Handle, ID_HIHI_LIMIT, pTag->alarms[3], pTag->severity[3], pTag->enabled[3]);

	VariantClear (&dlg.m_value);
	Invalidate(TRUE);

}

void CWTSvrTestView::AutoAddtag(int tagno) 
{
	CTag	*pTag;
	int	i;
	CTagDlg	dlg;
	VARIANT	PropertyValue;
	WCHAR	*pWCHAR;
	CWTSvrTestApp	*pApp;
	char buf[100] = {0};

	pApp = (CWTSvrTestApp *)AfxGetApp();

	for (i=0; i<4; i++)
		{
		dlg.m_alarms[i] = 0.0;
		dlg.m_severity[i] = 200;
		dlg.m_enabled[i] = FALSE;
		}
	dlg.m_alarms[2] = 100.0;
	dlg.m_alarms[3] = 100.0;
	dlg.m_value.vt = VT_BSTR;
	sprintf (buf, "%d", tagno);
 	pWCHAR = pApp->WSTRFromCString (buf);
	dlg.m_value.bstrVal = SysAllocString (pWCHAR);
	pApp->WSTRFree (pWCHAR);

	dlg.m_ReadAccess = ACCESSLEVELNONE;
	dlg.m_WriteAccess = ACCESSLEVELNONE;
	
//	if (dlg.DoModal() != IDOK)
//		return;


	sprintf (buf, "tag%d", tagno);
	dlg.m_name = buf;


		pTag = new (CTag);
		pTag->Name = dlg.m_name;
		pTag->Description = dlg.m_description;
		pTag->Units = dlg.m_units;
		VariantCopy (&(pTag->Value), &dlg.m_value);
		pTag->Time = CTime::GetCurrentTime();
		for (i=0; i<4; i++)
			{
			pTag->alarms[i] = dlg.m_alarms[i];
			pTag->severity[i] = dlg.m_severity[i];
			pTag->enabled[i] = dlg.m_enabled[i];
			}
		pTag->ReadAccessLevel = dlg.m_ReadAccess;
		pTag->WriteAccessLevel = dlg.m_WriteAccess;

		pTag->Handle = CreateSecureTag (pTag->Name, pTag->Value, OPC_QUALITY_GOOD, pTag->ReadAccessLevel, pTag->WriteAccessLevel);

		TagList.Add ((CObject *)pTag);

		VariantInit(&PropertyValue);
		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Units);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 100, "EU Units", PropertyValue);
		VariantClear(&PropertyValue);

		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Description);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 101, "Item Description", PropertyValue);
		VariantClear(&PropertyValue);

		SetItemLevelAlarm (pTag->Handle, ID_LOLO_LIMIT, pTag->alarms[0], pTag->severity[0], pTag->enabled[0]);
		SetItemLevelAlarm (pTag->Handle, ID_LO_LIMIT, pTag->alarms[1], pTag->severity[1], pTag->enabled[1]);
		SetItemLevelAlarm (pTag->Handle, ID_HI_LIMIT, pTag->alarms[2], pTag->severity[2], pTag->enabled[2]);
		SetItemLevelAlarm (pTag->Handle, ID_HIHI_LIMIT, pTag->alarms[3], pTag->severity[3], pTag->enabled[3]);

	VariantClear (&dlg.m_value);
	Invalidate(TRUE);

}

//
// Disable the Update Menu item
// if no tag is selected
//
void CWTSvrTestView::OnUpdateEditUpdatetag(CCmdUI* pCmdUI) 
{
	if (SelectedIndex == -1)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
	
}

//
// Update a Tag's value
// cannot change the tag name here
//
void CWTSvrTestView::OnEditUpdatetag() 
{
	CTag	*pTag;
	CTagDlg	dlg;
	VARIANT	PropertyValue, TempV;
	int	i;
	WCHAR	*pWCHAR;
	CWTSvrTestApp	*pApp;
	SAFEARRAY	*pMyArray;
	SAFEARRAYBOUND MyBounds;
	HRESULT hr;
	long	lTemp;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	pTag = (CTag *)TagList.GetAt(SelectedIndex);
	if (pTag == NULL)
		return;

	dlg.m_name = pTag->Name;
	dlg.m_description = pTag->Description;
	dlg.m_units = pTag->Units;
	if ((pTag->Value.vt & VT_ARRAY) != VT_ARRAY)
		{
		VariantCopy (&dlg.m_value, &(pTag->Value));
		dlg.m_doArray = FALSE;
		dlg.m_arraylength = 2;
		}
	else
		{
		dlg.m_value.vt = VT_R4;
		dlg.m_value.fltVal = 0.0;
		dlg.m_doArray = TRUE;
		dlg.m_arraylength = 2;
		}
	for (i=0; i<4; i++)
		{
		dlg.m_alarms[i] = pTag->alarms[i];
		dlg.m_severity[i] = pTag->severity[i];
		dlg.m_enabled[i] = pTag->enabled[i];
		}
	dlg.m_ReadAccess = pTag->ReadAccessLevel;
	dlg.m_WriteAccess = pTag->WriteAccessLevel;
	if (dlg.DoModal() != IDOK)
		return;

		if (!dlg.m_doArray)
			hr = VariantCopy (&(pTag->Value), &(dlg.m_value));
		else
			{
			VariantInit(&TempV);
			MyBounds.cElements = dlg.m_arraylength;
			MyBounds.lLbound = 0;
			pMyArray = SafeArrayCreate (dlg.m_value.vt, 1, &MyBounds);
			for (i=0; i<dlg.m_arraylength; i++)
				{
				lTemp = i;
				switch (dlg.m_value.vt)
					{
					case VT_R4:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.fltVal);
						break;
					case VT_BOOL:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.boolVal);
						break;
					case VT_BSTR:
						SafeArrayPutElement (pMyArray, &lTemp, dlg.m_value.bstrVal);
						break;
					case VT_I2:
					default:
						SafeArrayPutElement (pMyArray, &lTemp, &dlg.m_value.intVal);
						break;
					}
				}
			V_VT(&TempV) = VT_ARRAY | dlg.m_value.vt;
			V_ARRAY(&TempV) = pMyArray;
			hr = VariantCopy (&(pTag->Value), &TempV);
			VariantClear (&TempV);
			}
	pTag->Description = dlg.m_description;
	pTag->Units = dlg.m_units;
	pTag->Time = CTime::GetCurrentTime();
	for (i=0; i<4; i++)
		{
		pTag->alarms[i] = dlg.m_alarms[i];
		pTag->severity[i] = dlg.m_severity[i];
		pTag->enabled[i] = dlg.m_enabled[i];
		}

	UpdateTag (pTag->Handle, pTag->Value, OPC_QUALITY_GOOD);

	VariantInit(&PropertyValue);
	PropertyValue.vt = VT_BSTR;
	pWCHAR = pApp->WSTRFromCString (pTag->Units);
	PropertyValue.bstrVal = SysAllocString (pWCHAR);
	pApp->WSTRFree (pWCHAR);
	SetTagProperties (pTag->Handle, 100, "EU Units", PropertyValue);
	VariantClear(&PropertyValue);

	PropertyValue.vt = VT_BSTR;
	pWCHAR = pApp->WSTRFromCString (pTag->Description);
	PropertyValue.bstrVal = SysAllocString (pWCHAR);
	pApp->WSTRFree (pWCHAR);
	SetTagProperties (pTag->Handle, 101, "Item Description", PropertyValue);
	VariantClear(&PropertyValue);

	SetItemLevelAlarm (pTag->Handle, ID_LOLO_LIMIT, pTag->alarms[0], pTag->severity[0], pTag->enabled[0]);
	SetItemLevelAlarm (pTag->Handle, ID_LO_LIMIT, pTag->alarms[1], pTag->severity[1], pTag->enabled[1]);
	SetItemLevelAlarm (pTag->Handle, ID_HI_LIMIT, pTag->alarms[2], pTag->severity[2], pTag->enabled[2]);
	SetItemLevelAlarm (pTag->Handle, ID_HIHI_LIMIT, pTag->alarms[3], pTag->severity[3], pTag->enabled[3]);

	Invalidate(TRUE);

}

void CWTSvrTestView::OnUpdateEditDeletetag(CCmdUI* pCmdUI) 
{
	if (SelectedIndex == -1)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CWTSvrTestView::OnEditDeletetag() 
{
	CWTSvrTestApp	*pApp;
	CTag	*pTag;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	pTag = (CTag *)TagList.GetAt(SelectedIndex);
	if (pTag == NULL)
		return;

	TagList.RemoveAt(SelectedIndex);
	RemoveTag (pTag->Handle);
	delete (pTag);
	SelectedIndex = -1;
	Invalidate(TRUE);

}


//
// Find the local tag that was written
// and update its value with the value from the dll.
//
void CWTSvrTestView::NotificationHandler(HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError)
{
	int	i;
	CTag	*pTag;

	for (i=0; i<TagList.GetSize(); i++)
		{
		pTag = (CTag *)TagList.GetAt(i);
		if (pTag->Handle == Handle)
			{
			VariantCopy (&(pTag->Value), pNewValue);
			UpdateTag (pTag->Handle, pTag->Value, OPC_QUALITY_GOOD);
			Invalidate(TRUE);
			*pDeviceError = S_OK;
			return;
			}
		}
	*pDeviceError = OPC_E_INVALIDHANDLE;
}

void CWTSvrTestView::DeviceReadHandler(HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp)
{
	int	i;
	CTag	*pTag;
	SYSTEMTIME	systime;

	for (i=0; i<TagList.GetSize(); i++)
		{
		pTag = (CTag *)TagList.GetAt(i);
		if (pTag->Handle == Handle)
			{
			VariantCopy  (pNewValue, &(pTag->Value));
			*pQuality = OPC_QUALITY_GOOD;
			GetSystemTime (&systime);
			SystemTimeToFileTime (&systime, pTimestamp);
			return;
			}
		}
	*pQuality = OPC_QUALITY_BAD;

}




//
// Add the Requested Tag Name
//
void CWTSvrTestView::UnknownTagHandler(LPSTR Path, LPSTR Name)
{
	CTag	*pTag;
	char buf[200];

	pTag = new (CTag);
	strncpy (buf, Path, 200);
	pTag->Name = buf;
	if (pTag->Name.GetLength() > 0)
		pTag->Name += ".";
	strncpy (buf, Name, 200);
	pTag->Name += buf;
	pTag->Time = CTime::GetCurrentTime();

	pTag->Handle = CreateSecureTag (pTag->Name, pTag->Value, OPC_QUALITY_GOOD, pTag->ReadAccessLevel, pTag->WriteAccessLevel);	
	TagList.Add ((CObject *)pTag);

	Invalidate(TRUE);
}

//
// Delete the Tag when last client is done with it
//
void CWTSvrTestView::RemovedTagHandler (HANDLE hTag, LPSTR Path, LPSTR Name)
{
	CTag	*pTag;
	int	i;

	for (i=0; i<TagList.GetSize(); i++)
		{
		pTag = (CTag *)TagList.GetAt(i);
		if (pTag->Handle == hTag)
			{
			TagList.RemoveAt(i);
			delete (pTag);
			RemoveTag (hTag);
			SelectedIndex = -1;
			Invalidate(TRUE);
			}
		}

}


void CWTSvrTestView::OnEditDynamictags() 
{
	EnableUnknownItemNotification (&UnknownTagProc);
	EnableItemRemovalNotification (&TagRemovedProc);
}


//
// Added Feb 29, 2000 to force the server to refresh
// its data to all attached clients.
// (Check to see if async timer is working within server code)
//
void CWTSvrTestView::OnEditForceclientrefresh() 
{
	RefreshAllClients();	
}

void CWTSvrTestView::OnEditUseraemsg() 
{
	CAEMsgDlg	dlg;
//	ONEVENTSTRUCT	ev;
	SYSTEMTIME	SystemTime;
	FILETIME	TimeStamp;

	CWTSvrTestApp	*pApp;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	GetSystemTime(&SystemTime);		// Get current UTC Time
	SystemTimeToFileTime(&SystemTime, &TimeStamp); // and store it

	dlg.m_severity = 200;
	if (dlg.DoModal() == IDOK)
		{
/*		ev.wChangeMask = 0;
		ev.wNewState = 0;
		ev.szSource = pApp->WSTRFromCString("User Message");
		ev.ftTime = TimeStamp;
		ev.szMessage = pApp->WSTRFromCString(dlg.m_msg);
		ev.dwEventType = OPC_SIMPLE_EVENT;
		ev.dwEventCategory = 1;
		ev.dwSeverity = dlg.m_severity;
		ev.szConditionName = pApp->WSTRFromCString("User Defined Condition");
		ev.szSubconditionName = pApp->WSTRFromCString("User Defined Subcondition");
		ev.wQuality = OPC_QUALITY_GOOD;
		ev.bAckRequired = FALSE;
		ev.ftActiveTime = TimeStamp;
		ev.dwCookie = 1;
		ev.dwNumEventAttrs = 0;
		ev.pEventAttributes = NULL;
		ev.szActorID = pApp->WSTRFromCString("");
		
		UserAEMessageEx (ev);	*/
		UserAEMessage (dlg.m_msg, dlg.m_severity);
		}
	
}


void CWTSvrTestView::OnDestroy() 
{
	RequestDisconnect();
	Sleep(2000);
	
	CView::OnDestroy();
	
}




void CWTSvrTestView::OnFileSavetaglist() 
{
	CString	FilePath, TmpStr;
	CFile	ConfigFile;
	CFileException	e;    
	int	i,j,nTags;
	CTag	*pTag;
	char	buf[100] = {0};

	CFileDialog dlg(FALSE,NULL,"*.cfg",
					(DWORD)OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					NULL,NULL);
	if (dlg.DoModal() != IDOK)
		return;

	FilePath = dlg.GetPathName();
	if (!ConfigFile.Open (FilePath, CFile::modeWrite | CFile::modeCreate, &e))
		{
		AfxMessageBox ("File Creation Error", MB_OK);
		return;
		}

	CArchive archive (&ConfigFile, CArchive::store);

	nTags = TagList.GetSize();
	archive << nTags;
	for (i=0; i<nTags; i++)
		{
		pTag = (CTag *)TagList.GetAt(i);
		archive << pTag->Name;
		archive << pTag->Description;
		archive << pTag->Units;
		archive << pTag->Value.vt;
		switch (pTag->Value.vt)
			{
			case VT_I2:
					archive << pTag->Value.iVal;
					break;
			case VT_BOOL:
					archive << pTag->Value.boolVal;
					break;
			case VT_R4:
					archive << pTag->Value.fltVal;
					break;
			case VT_BSTR:
					sprintf	(buf, "%ls ", pTag->Value.bstrVal );
					TmpStr = buf;
					archive << TmpStr;
					break;
			}
		for (j=0; j<4; j++)
			{
			archive << pTag->alarms[j];
			archive << pTag->severity[j];
			archive << pTag->enabled[j];
			}
		archive << pTag->ReadAccessLevel;
		archive << pTag->WriteAccessLevel;
		}

	archive.Close();
	ConfigFile.Close();

}

void CWTSvrTestView::OnFileRestoretaglist() 
{
	CString	FilePath, TmpStr;
	CFile	ConfigFile;
	CFileException	e; 
	int	i, j, nTags;
	CTag	*pTag;
	CWTSvrTestApp	*pApp;
	WCHAR	*pWCHAR;
	VARIANT	PropertyValue;

	pApp = (CWTSvrTestApp *)AfxGetApp();

	CFileDialog dlg(TRUE,NULL,"*.cfg",
					(DWORD)OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					NULL,NULL);
	if (dlg.DoModal() != IDOK)
		return;

	FilePath = dlg.GetPathName();
	if (!ConfigFile.Open (FilePath, CFile::modeRead, &e))
		{
		AfxMessageBox ("File Open Error", MB_OK);
		return;
		}

	CArchive archive (&ConfigFile, CArchive::load);

	for (i=0; i<TagList.GetSize(); i++)
		delete ((CTag *)TagList.GetAt(i));
	TagList.RemoveAll();

	archive >> nTags;
	for (i=0; i<nTags; i++)
		{
		pTag = new (CTag);
		archive >> pTag->Name;
		archive >> pTag->Description;
		archive >> pTag->Units;
		archive >> pTag->Value.vt;
		switch (pTag->Value.vt)
			{
			case VT_I2:
					archive >> pTag->Value.iVal;
					break;
			case VT_BOOL:
					archive >> pTag->Value.boolVal;
					break;
			case VT_R4:
					archive >> pTag->Value.fltVal;
					break;
			case VT_BSTR:
					archive >> TmpStr;
					pWCHAR = pApp->WSTRFromCString (TmpStr);
					pTag->Value.bstrVal = SysAllocString (pWCHAR);
					pApp->WSTRFree (pWCHAR);
					break;
			}
		for (j=0; j<4; j++)
			{
			archive >> pTag->alarms[j];
			archive >> pTag->severity[j];
			archive >> pTag->enabled[j];
			}

		archive >> pTag->ReadAccessLevel;
		archive >> pTag->WriteAccessLevel;

		pTag->Handle = CreateSecureTag (pTag->Name, pTag->Value, OPC_QUALITY_GOOD, pTag->ReadAccessLevel, pTag->WriteAccessLevel);
		TagList.Add ((CObject *)pTag);

		VariantInit(&PropertyValue);
		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Units);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 100, "EU Units", PropertyValue);
		VariantClear(&PropertyValue);

		PropertyValue.vt = VT_BSTR;
		pWCHAR = pApp->WSTRFromCString (pTag->Description);
		PropertyValue.bstrVal = SysAllocString (pWCHAR);
		pApp->WSTRFree (pWCHAR);
		SetTagProperties (pTag->Handle, 101, "Item Description", PropertyValue);
		VariantClear(&PropertyValue);

		SetItemLevelAlarm (pTag->Handle, ID_LOLO_LIMIT, pTag->alarms[0], pTag->severity[0], pTag->enabled[0]);
		SetItemLevelAlarm (pTag->Handle, ID_LO_LIMIT, pTag->alarms[1], pTag->severity[1], pTag->enabled[1]);
		SetItemLevelAlarm (pTag->Handle, ID_HI_LIMIT, pTag->alarms[2], pTag->severity[2], pTag->enabled[2]);
		SetItemLevelAlarm (pTag->Handle, ID_HIHI_LIMIT, pTag->alarms[3], pTag->severity[3], pTag->enabled[3]);
	
		}

	Invalidate(TRUE);

	archive.Close();
	ConfigFile.Close();
}




void CWTSvrTestView::OnEditTagdelimiter() 
{
	CDelimitDlg dlg;	
	char	delimiter[2];

	delimiter[0] = SetWtOPCsvrQualifier(0);
	delimiter[1] = 0;
	dlg.m_delimiter = delimiter;
	if (dlg.DoModal() == IDOK)
		SetWtOPCsvrQualifier(dlg.m_delimiter[0]);

}




CTag::CTag()
{
	VariantInit(&Value);
	Value.vt = VT_R4;
	Value.fltVal = 0.0;
	ReadAccessLevel = 0;
	WriteAccessLevel = 0;
}

CTag::~CTag()
{
	VariantClear(&Value);
}


void CWTSvrTestView::OnNtsecurityEnable() 
{
	NTSecurityEnabled = EnableNTSecurity (TRUE, RPC_C_AUTHN_LEVEL_NONE);
	
}


void CWTSvrTestView::OnNtsecurityDisable() 
{
	NTSecurityEnabled = FALSE;
	EnableNTSecurity (FALSE, RPC_C_AUTHN_LEVEL_NONE);
	
}


void CWTSvrTestView::OnNtsecurityAdduser() 
{
	CAddUserDlg dlg;
	
	if (dlg.DoModal() == IDOK)
		DefineNTUser (dlg.m_group, dlg.m_user, dlg.m_accessrights);


}

void CWTSvrTestView::OnUpdateNtsecurityEnable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!NTSecurityEnabled);
	
}

void CWTSvrTestView::OnUpdateNtsecurityDisable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(NTSecurityEnabled);
	
}

void CWTSvrTestView::OnUpdateNtsecurityAdduser(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(NTSecurityEnabled);
	
}
