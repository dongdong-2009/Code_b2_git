// WtSvrTst2View.cpp : implementation of the CWtSvrTst2View class
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include "opcda.h"
#include "opc_ae.h"
#include "opcerror.h"
#include "wtopcsvrapi.h"
#include "WtSvrTst2.h"

#include "WtSvrTst2Doc.h"
#include "WtSvrTst2View.h"
#include "taglistdlg.h"
#include "delimitdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {C6C83062-6C69-11d4-80FF-00C04F790F3B}
const GUID CLSID_WtSvrTst2 =
{ 0xc6c83062, 0x6c69, 0x11d4, { 0x80, 0xff, 0x0, 0xc0, 0x4f, 0x79, 0xf, 0x3b } };

void CALLBACK EXPORT EventCallback(char *pMsg)
{
	CWtSvrTst2App	*pApp;

	pApp = (CWtSvrTst2App *)AfxGetApp();
	if (pApp->pView != NULL)
		pApp->pView->EventHandler(pMsg);

}

void CALLBACK EXPORT WriteNotifyCallback(HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError)
{
	CWtSvrTst2App	*pApp;

	pApp = (CWtSvrTst2App *)AfxGetApp();
	if (pApp->pView != NULL)
		pApp->pView->WriteNotificationHandler(Handle, pNewValue, pDeviceError);

}

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2View

IMPLEMENT_DYNCREATE(CWtSvrTst2View, CFormView)

BEGIN_MESSAGE_MAP(CWtSvrTst2View, CFormView)
	//{{AFX_MSG_MAP(CWtSvrTst2View)
	ON_COMMAND(ID_SERVER_REGISTER, OnServerRegister)
	ON_COMMAND(ID_SERVER_UNREGISTER, OnServerUnregister)
	ON_COMMAND(ID_SERVER_CREATETAGS, OnServerCreatetags)
	ON_UPDATE_COMMAND_UI(ID_SERVER_CREATETAGS, OnUpdateServerCreatetags)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SUSPEND, OnSuspend)
	ON_COMMAND(ID_SERVER_SETDELIMITER, OnServerSetdelimiter)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2View construction/destruction

CWtSvrTst2View::CWtSvrTst2View()
	: CFormView(CWtSvrTst2View::IDD)
{
	//{{AFX_DATA_INIT(CWtSvrTst2View)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	CWtSvrTst2App *pApp;

	pApp = (CWtSvrTst2App *)AfxGetApp();
	pApp->pView = this;

	RandomUpdates = FALSE;
	UpdateFrequency = 1;
	pTagList = NULL;
	IsSuspended = TRUE;

}

CWtSvrTst2View::~CWtSvrTst2View()
{
	int	i;
	CTag	*pTag;

	if (pTagList != NULL)
		{
		for (i=0; i<TagCount; i++)
			{
			pTag = &pTagList[i];
			RemoveTag(pTag->Handle);
			VariantClear (&(pTag->Value));
			}
		delete [] pTagList;
		}


	UninitWTOPCsvr();
}

void CWtSvrTst2View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWtSvrTst2View)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CWtSvrTst2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWtSvrTst2View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	InitWTOPCsvr( (BYTE *)&CLSID_WtSvrTst2, 250 );
	EnableWriteNotification (WriteNotifyCallback, TRUE);
	EnableEventMsgs (EventCallback);
   /* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.
    */
	srand( (unsigned)time( NULL ) );

	LogFile.Open ("c:\\wtsvrtst.txt", CFile::modeWrite | CFile::modeCreate, &e); 

}

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2View printing

BOOL CWtSvrTst2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWtSvrTst2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWtSvrTst2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWtSvrTst2View::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2View diagnostics

#ifdef _DEBUG
void CWtSvrTst2View::AssertValid() const
{
	CFormView::AssertValid();
}

void CWtSvrTst2View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWtSvrTst2Doc* CWtSvrTst2View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWtSvrTst2Doc)));
	return (CWtSvrTst2Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2View message handlers

void CWtSvrTst2View::OnServerRegister() 
{
	CString	SvrName, SvrDescr, HelpPath;
	int	i;

	// get path to executable by parsing the help path
	// contained in CWinApp
	HelpPath = AfxGetApp()->m_pszHelpFilePath;
	i = HelpPath.ReverseFind('\\');
	HelpPath = HelpPath.Left(i+1);
	HelpPath += "WtSvrTst2.exe"; 
	
	SvrName = "WtSvrTst2";
	SvrDescr = "Hi-Performance Server to test WinTECH OPC Toolkit";
	UpdateRegistry ((BYTE *)&CLSID_WtSvrTst2, SvrName, SvrDescr, HelpPath);
}

void CWtSvrTst2View::OnServerUnregister() 
{
	CString	SvrName;

	SvrName = "WtSvrTst2";
	UnregisterServer ((BYTE *)&CLSID_WtSvrTst2, SvrName);
}

void CWtSvrTst2View::OnUpdateServerCreatetags(CCmdUI* pCmdUI) 
{
	if (pTagList == NULL)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CWtSvrTst2View::OnServerCreatetags() 
{
	CTagListDlg	dlg;
	int	t, n, r, u, len, j;
	int index, ticks;
	char	buf[100];
	VARIANT	var;
	CTag	*pTag;
	WCHAR	wide_str[100];
	CStatic	*pStatic;
	CTime	now;
	CString	TmpStr;
	CButton	*pButton;
	CTreeCtrl	*pTreeCtrl;
	TVINSERTSTRUCT tvInsert;
	HTREEITEM	hTree, hUnit, hTrain, hNode, hTag, hAttr;
	int	totaltagcount;

	dlg.m_booltype = TRUE;
	dlg.m_floattype = TRUE;
	dlg.m_inttype = TRUE;
	dlg.m_strtype = TRUE;
	dlg.m_randomupdates = RandomUpdates;
	dlg.m_updatefrequency = UpdateFrequency;
	dlg.m_tagcount = 2000;

	if (dlg.DoModal() != IDOK)
		return;

	SetCapture();
	AfxGetApp()->DoWaitCursor(1);

	if (dlg.m_dohashing)
		SetHashing (dlg.m_tagcount * 5);

	DoListUpdates = dlg.m_dolistupdates;

	VariantInit(&var);

	RandomUpdates = dlg.m_randomupdates;
	UpdateFrequency = dlg.m_updatefrequency;
	nAttr = dlg.m_booltype + dlg.m_floattype + dlg.m_inttype + dlg.m_strtype;
	if (nAttr == 0)
		{
		nAttr = 1;
		dlg.m_floattype = TRUE;
		}
	Attr[0] = dlg.m_booltype;
	Attr[1] = dlg.m_floattype;
	Attr[2] = dlg.m_inttype;
	Attr[3] = dlg.m_strtype;

	nTags = 50;

	nNodes = 5;

	nTrains = dlg.m_tagcount / (nNodes * nTags * nAttr);
	if (nTrains == 0)
		nTrains = 1;
	else if (nTrains > 10)
		nTrains = 10;

	nUnits = dlg.m_tagcount / (nTrains * nNodes * nTags * nAttr);
	if (nUnits == 0)
		nUnits = 1;

	TagCount = nUnits * nTrains * nNodes * nTags * nAttr;
	pTagList = new CTag[TagCount];
	if (pTagList == NULL)
		{
		AfxMessageBox ("Memory Allocation Error", MB_OK);
		return;
		}

	for (j=0; j<TagCount; j++)
		VariantInit (&pTagList[j].Value);
	
	TicCount = 0;
	AvgUpdateTime = 0;
	index = 0;
	ticks = GetTickCount();

	totaltagcount = 0;
	for (u=1; u<=nUnits; u++)
		for (r=1; r<=nTrains; r++)
			for (n=1; n<=nNodes; n++)
				for (t=1; t<=nTags; t++)
					{
					++totaltagcount;
					if (totaltagcount > dlg.m_tagcount)
						break;
					sprintf (buf, "UNIT%d", u);
					TmpStr = buf;
					TmpStr += SetWtOPCsvrQualifier(0);
					sprintf (buf, "TRAIN%d", r);
					TmpStr += buf;
					TmpStr += SetWtOPCsvrQualifier(0);
					sprintf (buf, "NODE%d", n);
					TmpStr += buf;
					TmpStr += SetWtOPCsvrQualifier(0);
					sprintf (buf, "TAG%d", t);					
					TmpStr += buf;
					TmpStr += SetWtOPCsvrQualifier(0);
					if (Attr[0])
						{
						pTag = &pTagList[index++];
						pTag->Name = TmpStr + "INPUT";
						var.vt = VT_BOOL;
						var.boolVal = 0;
						VariantCopy (&(pTag->Value), &var);
						pTag->Handle = CreateTag(pTag->Name, var, OPC_QUALITY_GOOD, TRUE );				// BOOL IsWritable
						}
					if (Attr[1])
						{
						pTag = &pTagList[index++];
						pTag->Name = TmpStr + "PV";
						var.vt = VT_R4;
						var.fltVal = 0.0;
						pTag->Handle = CreateTag(pTag->Name, var, OPC_QUALITY_GOOD, TRUE );				// BOOL IsWritable
						VariantCopy (&(pTag->Value), &var);
						}
					if (Attr[2])
						{
						pTag = &pTagList[index++];
						pTag->Name = TmpStr + "SP";
						var.vt = VT_I2;
						var.intVal = 0;
						VariantCopy (&(pTag->Value), &var);
						pTag->Handle = CreateTag(pTag->Name, var, OPC_QUALITY_GOOD, TRUE );				// BOOL IsWritable
						}
					if (Attr[3])
						{
						pTag = &pTagList[index++];
						pTag->Name = TmpStr + "ID";
						var.vt = VT_BSTR;
						sprintf (buf, "TicCount=%d", TicCount);
						len = strlen (buf);
						for (j=0; j<len+1; j++)
							wide_str[j] = buf[j];
						var.bstrVal = SysAllocString (wide_str);
						pTag->Handle = CreateTag(pTag->Name, var, OPC_QUALITY_GOOD, TRUE );				// BOOL IsWritable
						VariantCopy (&(pTag->Value), &var);
						VariantClear (&var);
						}
					} 

	VariantClear(&var);

	pStatic = (CStatic *)GetDlgItem (IDC_NUMBER);
	sprintf (buf, "Number of Tags: %d", TagCount);
	pStatic->SetWindowText (buf);

	pStatic = (CStatic *)GetDlgItem (IDC_FREQUENCY);
	sprintf (buf, "Update Frequency: %d secs", dlg.m_updatefrequency);
	pStatic->SetWindowText (buf);

	pStatic = (CStatic *)GetDlgItem (IDC_CREATETIME);
	sprintf (buf, "Tag List Creation Time: %d ms", GetTickCount()-ticks);
	pStatic->SetWindowText (buf);

	now = CTime::GetCurrentTime();
	StartTime = GetTickCount();
	pStatic = (CStatic *)GetDlgItem (IDC_STARTTIME);
	TmpStr = now.Format ("Test Start Time: %H:%M:%S");
	pStatic->SetWindowText (TmpStr);

	// build the tree control
	pTreeCtrl = (CTreeCtrl *)GetDlgItem (IDC_TAGTREE);
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = NULL;
	tvInsert.item.mask = TVIF_TEXT;
	tvInsert.item.pszText = _T("Root");
	hTree = pTreeCtrl->InsertItem(&tvInsert);

	index = 0;
	for (u=1; u<=nUnits; u++)
		{
		sprintf (buf, "Unit%d", u);
		hUnit = pTreeCtrl->InsertItem(TVIF_TEXT, _T(buf), 0, 0, 0, 0, 0, hTree, NULL);
		for (r=1; r<=nTrains; r++)
			{
			sprintf (buf, "Train%d", r);
			hTrain = pTreeCtrl->InsertItem(TVIF_TEXT, _T(buf), 0, 0, 0, 0, 0, hUnit, NULL);
			for (n=1; n<=nNodes; n++)
				{
				sprintf (buf, "Node%d", n);
				hNode = pTreeCtrl->InsertItem(TVIF_TEXT, _T(buf), 0, 0, 0, 0, 0, hTrain, NULL);
				for (t=1; t<=nTags; t++)
					{
					sprintf (buf, "Tag%d", t);
					hTag = pTreeCtrl->InsertItem(TVIF_TEXT, _T(buf), 0, 0, 0, 0, 0, hNode, NULL);
					if (Attr[0])
						{
						hAttr = pTreeCtrl->InsertItem(TVIF_TEXT, _T("INPUT"), 0, 0, 0, 0, 0, hTag, NULL);
						pTreeCtrl->SetItemData (hAttr, index++);
						}
					if (Attr[1])
						{
						hAttr = pTreeCtrl->InsertItem(TVIF_TEXT, _T("PV"), 0, 0, 0, 0, 0, hTag, NULL);
						pTreeCtrl->SetItemData (hAttr, index++);
						}
					if (Attr[2])
						{
						hAttr = pTreeCtrl->InsertItem(TVIF_TEXT, _T("SP"), 0, 0, 0, 0, 0, hTag, NULL);
						pTreeCtrl->SetItemData (hAttr, index++);
						}
					if (Attr[3])
						{
						hAttr = pTreeCtrl->InsertItem(TVIF_TEXT, _T("ID"), 0, 0, 0, 0, 0, hTag, NULL);
						pTreeCtrl->SetItemData (hAttr, index++);
						}
					} // Tag
				} // Node
			} // Train
		} // Unit

	UpdateTree();

	IsSuspended = FALSE;
	pButton = (CButton *)GetDlgItem (IDC_SUSPEND);
	pButton->EnableWindow();

	AfxGetApp()->DoWaitCursor(0);
	ReleaseCapture();

	SetTimer (1, dlg.m_updatefrequency*1000, NULL);

}


void CWtSvrTst2View::OnDestroy() 
{
	CWtSvrTst2App *pApp;

	pApp = (CWtSvrTst2App *)AfxGetApp();

	KillTimer (1);

	pApp->pView = NULL;
	
	LogFile.Close();

	RequestDisconnect();
	Sleep(2000);

	CFormView::OnDestroy();
	

}

void CWtSvrTst2View::OnTimer(UINT nIDEvent) 
{
	int	index, len, j, ticks;
	int	u, r, t, n, random;
	CTag	*pTag;
	char buf[100];
	WCHAR	wide_str[100];
	VARIANT var;
	CStatic	*pStatic;
	DWORD	now;
	
	if ((nIDEvent == 1) && (!IsSuspended))
		{
		VariantInit(&var);
		++TicCount;
		index = 0;
		ticks = GetTickCount();
		if (DoListUpdates)
			StartUpdateTags ();
		for (u=1; u<=nUnits; u++)
			for (r=1; r<=nTrains; r++)
				for (n=1; n<=nNodes; n++)
					for (t=1; t<=nTags; t++)
						{
						if (Attr[0])
							{
							pTag = &pTagList[index++];
							var.vt = VT_BOOL;
							if (RandomUpdates)
								{
								random = rand();
								var.boolVal = random & 1;
								}
							else
								var.boolVal = !(pTag->Value.boolVal);
							VariantCopy (&(pTag->Value), &var);
							if (DoListUpdates)
								UpdateTagToList (pTag->Handle, var, OPC_QUALITY_GOOD);
							else
								UpdateTag (pTag->Handle, var, OPC_QUALITY_GOOD);
							}
						if (Attr[1])
							{
							pTag = &pTagList[index++];
							var.vt = VT_R4;
							if (RandomUpdates)
								{
								random = rand();
								var.fltVal = (float)random / 100.0f;
								}
							else
								var.fltVal = pTag->Value.fltVal + 0.1f;
							VariantCopy (&(pTag->Value), &var);
							if (DoListUpdates)
								UpdateTagToList (pTag->Handle, var, OPC_QUALITY_GOOD);
							else
								UpdateTag (pTag->Handle, var, OPC_QUALITY_GOOD);
							}
						if (Attr[2])
							{
							pTag = &pTagList[index++];
							var.vt = VT_I2;
							if (RandomUpdates)
								{
								random = rand();
								var.intVal = random;
								}
							else
								var.intVal = pTag->Value.intVal + 1;
							VariantCopy (&(pTag->Value), &var);
							if (DoListUpdates)
								UpdateTagToList (pTag->Handle, var, OPC_QUALITY_GOOD);
							else
								UpdateTag (pTag->Handle, var, OPC_QUALITY_GOOD);
							}
						if (Attr[3])
							{
							pTag = &pTagList[index++];
							if (RandomUpdates)
								{
								random = rand();
								sprintf (buf, "Random=%d", random);
								}
							else
								sprintf (buf, "TicCount=%d", TicCount);
							len = strlen (buf);
							for (j=0; j<len+1; j++)
								wide_str[j] = buf[j];
							var.vt = VT_BSTR;
							var.bstrVal = SysAllocString (wide_str);
							VariantCopy (&(pTag->Value), &var);
							if (DoListUpdates)
								UpdateTagToList (pTag->Handle, var, OPC_QUALITY_GOOD);
							else
								UpdateTag (pTag->Handle, var, OPC_QUALITY_GOOD);
							VariantClear (&var);
							}
						}

		if (DoListUpdates)
			EndUpdateTags ();

		pStatic = (CStatic *)GetDlgItem (IDC_UPDATETIME);
		ticks = GetTickCount() - ticks;
		sprintf (buf, "Tag List Update Time: %d ms", ticks);
		pStatic->SetWindowText (buf);

		AvgUpdateTime = ((AvgUpdateTime * (TicCount-1)) + ticks) / TicCount;
		pStatic = (CStatic *)GetDlgItem (IDC_AVGUPDATE);
		ticks = (int)AvgUpdateTime;
		sprintf(buf, "Avg Update Time: %d ms", ticks);
		pStatic->SetWindowText (buf);

		pStatic = (CStatic *)GetDlgItem (IDC_UPDATES);
		sprintf (buf, "Number of Updates: %d", TicCount);
		pStatic->SetWindowText (buf);

		now = GetTickCount();
		pStatic = (CStatic *)GetDlgItem (IDC_CURRENTTIME);
		sprintf(buf, "Duration of Test: %d secs", (now - StartTime)/1000);
		pStatic->SetWindowText (buf);

		UpdateTree();

		}

	CFormView::OnTimer(nIDEvent);
}

void CWtSvrTst2View::OnSuspend() 
{
	CButton	*pButton;
	CStatic	*pStatic;
	CTime	now;
	CString	TmpStr;

	IsSuspended = !IsSuspended;
	pButton = (CButton *)GetDlgItem (IDC_SUSPEND);
	if (IsSuspended)
		pButton->SetWindowText ("Resume");
	else
		{
		pButton->SetWindowText ("Suspend Updates");
		now = CTime::GetCurrentTime();
		StartTime = GetTickCount();
		pStatic = (CStatic *)GetDlgItem (IDC_STARTTIME);
		TmpStr = now.Format ("Test Start Time: %H:%M:%S");
		pStatic->SetWindowText (TmpStr);
		TicCount = 0;
		AvgUpdateTime = 0;
	}
}


void CWtSvrTst2View::EventHandler (char *pMsg)
{
	CListBox	*pLB;

	LogFile.Write (pMsg, strlen(pMsg));
	LogFile.Write ("\r\n", 2);
	pLB = (CListBox *)GetDlgItem(IDC_EVENTLIST);
	while (pLB->GetCount() > 500)
		pLB->DeleteString(0);
	pLB->AddString (pMsg);
}


void CWtSvrTst2View::UpdateTree()
{
	CTreeCtrl *pTreeCtrl;
	HTREEITEM hItem;
	CTag	*pTag;
	char buf[100];

	pTreeCtrl = (CTreeCtrl *)GetDlgItem (IDC_TAGTREE);

	hItem = pTreeCtrl->GetFirstVisibleItem();
	while (hItem != NULL)
		{
		if (!pTreeCtrl->ItemHasChildren(hItem))
			{
			pTag = &pTagList[pTreeCtrl->GetItemData(hItem)];
			switch (pTag->Value.vt)
				{
				case VT_BOOL: if (pTag->Value.boolVal)
								  pTreeCtrl->SetItemText (hItem, "INPUT: [1]");
							else
								pTreeCtrl->SetItemText (hItem, "INPUT: [0]");
							break;
				case VT_R4:	sprintf (buf, "PV: [%3.2f]", pTag->Value.fltVal);
							pTreeCtrl->SetItemText (hItem, buf);
							break;
				case VT_I2:	sprintf (buf, "SP: [%d]", pTag->Value.intVal);
							pTreeCtrl->SetItemText (hItem, buf);
							break;
				case VT_BSTR: sprintf	(buf, "ID: [%ls]", pTag->Value.bstrVal );
							pTreeCtrl->SetItemText (hItem, buf);
							break;
				}
			}
		hItem = pTreeCtrl->GetNextVisibleItem(hItem);
		}
}

//
// Find the local tag that was written
// and update its value with the value from the dll.
//
void CWtSvrTst2View::WriteNotificationHandler(HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError)
{
	int	i;
	CTag	*pTag;

	for (i=0; i<TagCount; i++)
		{
		pTag = &pTagList[i];
		if (pTag->Handle == Handle)
			{
			VariantCopy (&(pTag->Value), pNewValue);
			*pDeviceError = S_OK;
			if (IsSuspended)
				UpdateTree();
			// otherwise just let it go until the next update cycle
			return;
			}
		}
	*pDeviceError = OPC_E_INVALIDHANDLE;
}



void CWtSvrTst2View::OnServerSetdelimiter() 
{
	CDelimitDlg dlg;	
	char	delimiter[2];

	delimiter[0] = SetWtOPCsvrQualifier(0);
	delimiter[1] = 0;
	dlg.m_delimiter = delimiter;
	if (dlg.DoModal() == IDOK)
		SetWtOPCsvrQualifier
		(dlg.m_delimiter[0]);
}
