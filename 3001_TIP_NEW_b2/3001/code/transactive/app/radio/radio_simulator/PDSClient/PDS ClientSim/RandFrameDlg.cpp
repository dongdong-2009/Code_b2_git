// RandFrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "RandFrameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRandFrameDlg dialog


CRandFrameDlg::CRandFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRandFrameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRandFrameDlg)
	//}}AFX_DATA_INIT

	m_lPreviousData = -1;

}


void CRandFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRandFrameDlg)
	DDX_Control(pDX, IDC_EDT_RAND_ID, m_editFrameRand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRandFrameDlg, CDialog)
	//{{AFX_MSG_MAP(CRandFrameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRandFrameDlg message handlers

BOOL CRandFrameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_vctrRandFrameID.clear();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRandFrameDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (m_editFrameRand.m_hWnd != pMsg->hwnd)
	{
		return CDialog::PreTranslateMessage(pMsg);
	}

	
	if (pMsg->message == WM_KEYDOWN)
	{
		// allow 0 to 9 and comma
		if (((pMsg->wParam >= 48) && (pMsg->wParam <= 57)) || pMsg->wParam == 0xBC)
		{
			
			if ((m_lPreviousData == pMsg->wParam) && (pMsg->wParam == 0xBC))
			{
				return TRUE;
			}
			
			m_lPreviousData = pMsg->wParam;
			
		
		}
		else if  ((pMsg->wParam == 8) || (pMsg->wParam == 0x25) || (pMsg->wParam == 0x27)
				  || (pMsg->wParam == 0x2E))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CRandFrameDlg::OnOK() 
{
	// TODO: Add extra validation here

	char * pszData = new char [1024];

	m_editFrameRand.GetWindowText(pszData,1024);

	CString strData = pszData;
	delete [] pszData;

	int nFind = strData.Find(",");
	int nStart = 0;
	
	char * pszTemp = new char [1023];

	while (-1 != nFind)
	{
		CString strTemp = strData.Mid (nStart,nFind);
		strData = strData.Mid(nFind + 1);
		nFind = strData.Find(",");
		
		sprintf (pszTemp,"Frame %s",strTemp.GetBuffer(0));
		
		std::string strData = pszTemp;
		m_vctrRandFrameID.push_back(strData);
	}
	
	delete [] pszTemp;
	pszTemp = new char [1023];
	
	sprintf (pszTemp,"Frame %s",strData.GetBuffer(0));

	std::string strLastData = pszTemp;
	m_vctrRandFrameID.push_back(strLastData);
	
/*	for (int nCounter = 0; nCounter < m_vctrRandFrameID.size(); nCounter++)
	{
		::MessageBox (NULL,m_vctrRandFrameID.at (nCounter).c_str(),"INFO",MB_OK);
	}*/

	delete [] pszTemp;
	

	m_frameRandGen->notifyGeneratedFrame(m_vctrRandFrameID);
	CDialog::OnOK();
}

void CRandFrameDlg::attachObserver(IFrameRandGenerator * ptrNotifier)
{
	m_frameRandGen = ptrNotifier;
}
