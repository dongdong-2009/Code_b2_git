// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "opcda.h"
#include "opc_ae.h"
#include "wtopcsvrapi.h"
#include "WtSvrTst2.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CLOSE_APP, OnCloseApp)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSysCommand(UINT nID, LONG lParam)
{
   UINT nItemID = (nID & 0xFFF0);


  if (nItemID==SC_CLOSE)
   {
	  if (NumbrClientConnections() > 0)
		{
		if (MessageBox(_T("Are you sure you want to Abort the Server?"), 
				_T("OPC Clients are currently connected to this server."),
                MB_YESNO|MB_ICONQUESTION ) != IDYES)
		return;
		}
   }

   // call default functionality
   CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnCloseApp() 
{
  if (NumbrClientConnections() > 0)
	{
	if (MessageBox(_T("Are you sure you want to Abort the Server?"), 
				_T("OPC Clients are currently connected to this server."),
                MB_YESNO|MB_ICONQUESTION ) != IDYES)
	return;
	}

  OnClose();
}
