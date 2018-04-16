// FrameRepeatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "FrameRepeatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameRepeatDlg dialog


CFrameRepeatDlg::CFrameRepeatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrameRepeatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrameRepeatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFrameRepeatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrameRepeatDlg)
	DDX_Control(pDX, IDC_LST_FRAMES, m_listFrames);
	DDX_Control(pDX, IDC_LSTFRAMEREPEAT, m_listFrameRepeat);
	DDX_Control(pDX, IDC_EDT_FrameNum, m_editFrameCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrameRepeatDlg, CDialog)
	//{{AFX_MSG_MAP(CFrameRepeatDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_ADDALL, OnBtnAddall)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	ON_BN_CLICKED(IDC_BTN_RMVALL, OnBtnRmvall)
	ON_BN_CLICKED(IDC_BUTTON1, OnAddXID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameRepeatDlg message handlers

void CFrameRepeatDlg::SetMaxFrame(long lFrameCount)
{
	m_lFrameCount = lFrameCount;
}

BOOL CFrameRepeatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	CString strData;
	strData.Format("%d",m_lFrameCount);
	m_editFrameCount.SetWindowText(strData.GetBuffer(0));
	
	m_listFrames.ResetContent();
	m_vctrFrameNames.clear();

	for (int nCounter = 1; nCounter <= m_lFrameCount; nCounter++)
	{
		strData.Format("Frame %d", nCounter);
		m_listFrames.AddString(strData.GetBuffer(0));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrameRepeatDlg::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	
	
	int nSelCount = m_listFrames.GetSelCount();
	int * nFrameSelected = new int [nSelCount];

	m_listFrames.GetSelItems(nSelCount,nFrameSelected);

	for (int nCount = 0; nCount < nSelCount; nCount++)
	{
		char * pszData = new char [1023];
		m_listFrames.GetText(*(nFrameSelected + nCount),pszData);
		
		if (isDuplicated(pszData))
			continue;

		m_listFrameRepeat.AddString(pszData);
		
		std::string strFrameName = pszData;
		m_vctrFrameNames.push_back(strFrameName);
		
		delete [] pszData;
			
	}

	delete [] nFrameSelected;

}

bool CFrameRepeatDlg::isDuplicated(char * pszFrameName)
{
	bool bDuplicated = false;
	
	std::string strFrameName = pszFrameName;
	std::vector <std::string>::iterator frameIterator;
	
	frameIterator = std::find (m_vctrFrameNames.begin(),m_vctrFrameNames.end(),strFrameName);

	if (frameIterator != m_vctrFrameNames.end())
		bDuplicated = true; 

	return bDuplicated;
}

void CFrameRepeatDlg::OnBtnAddall() 
{
	// TODO: Add your control notification handler code here

	for (int nCounter = 0; nCounter < m_lFrameCount; nCounter++)
	{
		
		char * pszData = new char [1023];
		m_listFrames.GetText(nCounter,pszData);
		
		if (isDuplicated(pszData))
			continue;
		
		m_listFrameRepeat.AddString(pszData);
		
		std::string strFrameName = pszData;
		m_vctrFrameNames.push_back(strFrameName);

		delete [] pszData;
	}

}

void CFrameRepeatDlg::OnBtnRemove() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_listFrameRepeat.GetCurSel();
	
	if (LB_ERR == nIndex)
	{
		::MessageBox (NULL,"No Selected Item of Frame List","ERROR",MB_OK);
		return;
	}
	
	char * pszData = new char [1023];
	
	m_listFrameRepeat.GetText(nIndex,pszData);

	std::string strFrameName = pszData;
	
	m_listFrameRepeat.DeleteString(nIndex);
	
	std::vector<std::string>::iterator frameIterator;
	frameIterator = std::find (m_vctrFrameNames.begin(),m_vctrFrameNames.end(),strFrameName);

	if (frameIterator != m_vctrFrameNames.end())
	{
		m_vctrFrameNames.erase(frameIterator);
	}

	delete [] pszData;
}

void CFrameRepeatDlg::OnBtnRmvall() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listFrameRepeat.GetCount();

	for (int nCounter = 0; nCounter < nCount; nCounter++)
	{
		
		char * pszData = new char [1023];
		m_listFrameRepeat.GetText(0,pszData);
		
		m_listFrameRepeat.DeleteString(0);
		
		std::string strFrameName = pszData;
		
		std::vector<std::string>::iterator frameIterator;
		frameIterator = std::find (m_vctrFrameNames.begin(),m_vctrFrameNames.end(),strFrameName);
		
		if (frameIterator != m_vctrFrameNames.end())
		{
			m_vctrFrameNames.erase(frameIterator);
		}

		delete [] pszData;
	}
}

void CFrameRepeatDlg::setListener(INotifyFrameRepetition *listener)
{
	m_listener = listener;
}

void CFrameRepeatDlg::OnOK() 
{
	// TODO: Add extra validation here

	if (m_vctrFrameNames.empty())
		CDialog::OnOK();
	
	m_vctrFrameRepeat.clear ();

	std::vector <std::string>::iterator frameIterator;
	frameIterator = m_vctrFrameNames.begin();
	
	while (frameIterator != m_vctrFrameNames.end())
	{
		
		CString strData = frameIterator->c_str ();
		
		strData = strData.Mid(strlen ("Frame "));
		
		long lFrameID = atol (strData.GetBuffer(0));
		
		/*CString strFormat;

		strFormat.Format("%d",lFrameID);

		::MessageBox (NULL,strFormat.GetBuffer(0),"Duplicated",MB_OK);*/

		m_vctrFrameRepeat.push_back(lFrameID);
		
		frameIterator++;
	}


	m_listener->notifyFrameRepeat(m_vctrFrameRepeat);

	CDialog::OnOK();
}

void CFrameRepeatDlg::OnAddXID() 
{
	// TODO: Add your control notification handler code here
	m_frameRandGenrator.attachObserver(this);
	m_frameRandGenrator.DoModal();	
}

void CFrameRepeatDlg::notifyGeneratedFrame(std::vector<std::string> vctrRandFrame)
{
	
	for (int nCounter = 0; nCounter < vctrRandFrame.size(); nCounter++)
	{	
		if (isDuplicated((char *)vctrRandFrame.at (nCounter).c_str()))
		{
			//::MessageBox (NULL,vctrRandFrame.at (nCounter).c_str(),"Duplicated",MB_OK);
			continue;
		}
	
		m_listFrameRepeat.AddString(vctrRandFrame.at (nCounter).c_str());
		
		std::string strFrameName = vctrRandFrame.at (nCounter);
		m_vctrFrameNames.push_back(strFrameName);
	}
	
}

