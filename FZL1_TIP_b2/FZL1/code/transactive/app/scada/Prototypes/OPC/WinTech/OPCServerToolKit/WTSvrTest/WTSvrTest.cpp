// WTSvrTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "opcda.h"
#include "opc_ae.h"
#include "WTOPCsvrAPI.h"
#include "WTSvrTest.h"

#include "MainFrm.h"
#include "WTSvrTestDoc.h"
#include "WTSvrTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestApp

BEGIN_MESSAGE_MAP(CWTSvrTestApp, CWinApp)
	//{{AFX_MSG_MAP(CWTSvrTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestApp construction

CWTSvrTestApp::CWTSvrTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWTSvrTestApp object

CWTSvrTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestApp initialization

BOOL CWTSvrTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWTSvrTestDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWTSvrTestView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CWTSvrTestApp::ExitInstance() 
{
	UninitWTOPCsvr();
	
	return CWinApp::ExitInstance();
}

WCHAR *CWTSvrTestApp::WSTRFromCString(const CHAR *buf)
{
	int	length, i;
	WCHAR	*temp;

	length = strlen(buf) + 1;
	temp = new WCHAR[strlen(buf) + 1];
	
	if(temp)
		{
		for(i=0; i<length; i++) temp[i] = (WCHAR) buf[i];
		}

	return temp;
}

void CWTSvrTestApp::WSTRFree(WCHAR * c)
{
	if (c == NULL)
		return;

	delete c;
}


void CWTSvrTestApp::FormatVariant(VARIANT *pValue, char *buf, int bufsize)
{

	switch(pValue->vt)
		{
		case VT_I1:
		case VT_UI1:		// BYTE
			sprintf (buf, "%d ", pValue->bVal );
			break;
		case VT_I2:			// SHORT
			sprintf	(buf, "%d ", pValue->iVal );
			break;
		case VT_UI2:		// UNSIGNED SHORT
			sprintf	(buf, "%d ", pValue->uiVal );
			break;
		case VT_I4:			// LONG
			sprintf	(buf, "%ld ", pValue->lVal );
			break;
		case VT_UI4:		// UNSIGNED LONG
			sprintf	(buf, "%ld ", pValue->ulVal );
			break;
		case VT_INT:		// INTEGER
			sprintf	(buf, "%ld ", pValue->intVal );
			break;
		case VT_UINT:		// UNSIGNED INTEGER
			sprintf	(buf, "%ld ", pValue->uintVal );
			break;
		case VT_R4:			// FLOAT
			sprintf	(buf, "%5.2f ", pValue->fltVal );
			break;
		case VT_R8:			// DOUBLE
			sprintf	(buf, "%9.4f ", pValue->dblVal );
			break;
		case VT_BSTR:		//BSTR
			sprintf	(buf, "%ls ", pValue->bstrVal );
			break;
		case VT_BOOL:
			if (pValue->boolVal)
				strcpy (buf, "TRUE");
			else
				strcpy (buf, "FALSE");
			break;
		case VT_ARRAY | VT_I2:
			FormatIntArray (pValue, buf, bufsize);
			return;
		case VT_ARRAY | VT_R4:
			FormatFltArray (pValue, buf, bufsize);
			return;
		case VT_ARRAY | VT_BOOL:
			FormatBoolArray (pValue, buf, bufsize);
			return;
		default:
			sprintf	(buf, " unknown type:%d ", pValue->vt );
			break;
		}
}

void CWTSvrTestApp::FormatIntArray(VARIANT *pValue, char *buf, int bufsize)
{
	SAFEARRAY	*pMyArray;
	long	i, hLimit;
	short		value;
	int		bufindx;
	char	tmpbuf[80] = {0};

	bufsize -= 4;
	pMyArray = V_ARRAY(pValue);
	SafeArrayGetUBound (pMyArray, 1, &hLimit);
	bufindx = 0;
	for (i=0; i<=hLimit; i++)
		{
		SafeArrayGetElement (pMyArray, &i, &value);
		sprintf	(tmpbuf, "%d,", value);
		if (bufindx < (int)(bufsize - strlen(tmpbuf)))
			{
			strcpy (&buf[bufindx], tmpbuf);
			bufindx += strlen(tmpbuf);
			}
		else
			{
			strcpy (&buf[bufindx], ".etc");
			return;
			}
		}

}

void CWTSvrTestApp::FormatFltArray(VARIANT *pValue, char *buf, int bufsize)
{
	SAFEARRAY	*pMyArray;
	long	i, hLimit;
	float		value;
	int		bufindx;
	char	tmpbuf[80] = {0};

	bufsize -= 4;
	pMyArray = V_ARRAY(pValue);
	SafeArrayGetUBound (pMyArray, 1, &hLimit);
	bufindx = 0;
	for (i=0; i<=hLimit; i++)
		{
		SafeArrayGetElement (pMyArray, &i, &value);
		sprintf	(tmpbuf, "%5.2f,", value);
		if (bufindx < (int)(bufsize - strlen(tmpbuf)))
			{
			strcpy (&buf[bufindx], tmpbuf);
			bufindx += strlen(tmpbuf);
			}
		else
			{
			strcpy (&buf[bufindx], ".etc");
			return;
			}
		}
}

void CWTSvrTestApp::FormatBoolArray(VARIANT *pValue, char *buf, int bufsize)
{
	SAFEARRAY	*pMyArray;
	long	i, hLimit;
	bool		value;
	int		bufindx;
	char	tmpbuf[80];

	bufsize -= 4;
	pMyArray = V_ARRAY(pValue);
	SafeArrayGetUBound (pMyArray, 1, &hLimit);
	bufindx = 0;
	for (i=0; i<=hLimit; i++)
		{
		SafeArrayGetElement (pMyArray, &i, &value);
		if (value)
			strcpy (tmpbuf, "true,");
		else
			strcpy (tmpbuf, "false,");
		if (bufindx < (int)(bufsize - strlen(tmpbuf)))
			{
			strcpy (&buf[bufindx], tmpbuf);
			bufindx += strlen(tmpbuf);
			}
		else
			{
			strcpy (&buf[bufindx], ".etc");
			return;
			}
		}
}

void CWTSvrTestApp::FormatBstrArray(VARIANT *pValue, char *buf, int bufsize)
{
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWTSvrTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestApp commands

