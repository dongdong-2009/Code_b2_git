// TrainSimulatorDlg.cpp : implementation file
//


#include "SendingAutoRespConf.h"
#include "boost\shared_ptr.hpp"

#include <atlbase.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSendingAutoRespConf::CSendingAutoRespConf(CWnd* pParent /*=NULL*/)
	:	CDialog(CSendingAutoRespConf::IDD, pParent),
		m_controller(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSendingAutoRespConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
 	DDX_Control(pDX, IDC_CHECK_PECANSWER, m_chkPecAnswer);
	DDX_Control(pDX, IDC_CHECK_PECHOLD, m_chkPecHold);
	DDX_Control(pDX, IDC_CHECK_PECRESET, m_chkPecReset);
	DDX_Control(pDX, IDC_CHECK_PALIVE, m_chkPaLive);
	DDX_Control(pDX, IDC_CHECK_PAPRERECORDED, m_chkPaPreRecorded);
	DDX_Control(pDX, IDC_CHECK_PALIVERESET, m_chkPaLiveReset);
	DDX_Control(pDX, IDC_CHECK_PAPRERECORDEDRESET, m_chkPaPreRecordedReset);
	DDX_Control(pDX, IDC_CHECK_PACONTINUE, m_chkPaContinue);
	DDX_Control(pDX, IDC_BTN_SENDSTATE,m_btnSendStateChange);
	DDX_Control(pDX, IDC_BTN_SENDSTATE2,m_btnSendStateChangeTkeCtrl);
	DDX_Control(pDX, IDC_CHECK_TTISLIB,m_chkTtisLibDownload);
	DDX_Control(pDX, IDC_CHK_PRE_COMP,m_chkPreRecordedCompl);
	DDX_Control(pDX, IDC_Chk_ChangeOver,m_chkChangeOverCtrl);
	DDX_Control(pDX, IDC_CHECK_TTIS_CHANGEOVER,m_chkTtisChangeOver);
	DDX_Control(pDX, IDC_CHECK_PECANS_CHANGEOVER,m_chkPecChangeOver);
	DDX_Control(pDX, IDC_CHECK_PALIVE_CHANGEOVER,m_chkPaLiveChangeOver);
	DDX_Control(pDX, IDC_CHECK_PAPREREC_CHANGEOVER,m_chkPaPreRecChangeOver);
	DDX_Control(pDX, IDC_CHECK_TTISUPGRADDE_CHANGEOVER,m_chkTtisUpdateChangeOver);
	DDX_Control(pDX,IDC_CHK_TTIS_CMD_RCV_FAIL,m_chkTtisCmdRecvFailed);
	DDX_Control(pDX,IDC_CMB_TrainPACount,m_cmboPaLiveAutoSendOption);
	DDX_Control(pDX,IDC_CHECK_PAPREEMERGENCY,m_chkPaPreRecEmergency);
	
}

BEGIN_MESSAGE_MAP(CSendingAutoRespConf, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
// 	//}}AFX_MSG_MAP
 	ON_BN_CLICKED(IDC_CHECK_PECANSWER, &CSendingAutoRespConf::OnCheckPECBoxTick)
	ON_BN_CLICKED(IDC_CHECK_PECHOLD, &CSendingAutoRespConf::OnCheckPECBoxTick)
	ON_BN_CLICKED(IDC_CHECK_PECRESET, &CSendingAutoRespConf::OnCheckPECBoxTick)
	ON_BN_CLICKED(IDC_CHECK_PALIVE, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_CHECK_PAPRERECORDED, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_CHECK_PALIVERESET, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_CHECK_PAPRERECORDEDRESET, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_CHECK_PACONTINUE, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_BTN_SENDSTATE, &CSendingAutoRespConf::OnBtnSendState)
	ON_BN_CLICKED(IDC_BTN_SENDSTATE2, &CSendingAutoRespConf::OnBtnSendStateTkeCtrl)
	ON_BN_CLICKED(IDC_CHECK_TTISLIB, &CSendingAutoRespConf::OnCheckTTISBoxTick)
	ON_BN_CLICKED(IDC_CHK_PRE_COMP, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_BN_CLICKED(IDC_Chk_ChangeOver, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHECK_TTIS_CHANGEOVER, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHECK_PECANS_CHANGEOVER, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHECK_PALIVE_CHANGEOVER, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHECK_PAPREREC_CHANGEOVER, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHECK_TTISUPGRADDE_CHANGEOVER, &CSendingAutoRespConf::OnBtnChangeOverStatusTick)
	ON_BN_CLICKED(IDC_CHK_TTIS_CMD_RCV_FAIL, &CSendingAutoRespConf::OnCheckTTISBoxTick)
	ON_BN_CLICKED(IDC_CHECK_PAPREEMERGENCY, &CSendingAutoRespConf::OnCheckPABoxTick)
	ON_CBN_SELCHANGE(IDC_CMB_TrainPACount, &CSendingAutoRespConf::OnCbnSelchangeComboPALiveTrainCount)

END_MESSAGE_MAP()


// CTrainSimulatorDlg message handlers

BOOL CSendingAutoRespConf::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	
	
	IConfigDataItemPtrType autoSendInfo = m_controller->getUpdateConfForAutoChanging(EPecType);
	PecAutoSendInfo * autoSendingPtr = (PecAutoSendInfo * )autoSendInfo.get();

	m_chkPecAnswer.SetCheck(autoSendingPtr->getAutoSendPecAnswer());
	m_chkPecHold.SetCheck(autoSendingPtr->getAutoSendPecHold());
	m_chkPecReset.SetCheck(autoSendingPtr->getAutoSendPecReset());


	autoSendInfo = m_controller->getUpdateConfForAutoChanging(EPAType);
	PAAutoSendInfo * autoPASendingPtr = (PAAutoSendInfo * )autoSendInfo.get();

	m_chkPaLive.SetCheck(autoPASendingPtr->getAutoSendPALive());
	m_chkPaPreRecorded.SetCheck(autoPASendingPtr->getAutoSendPAPreRecorded());
	m_chkPaLiveReset.SetCheck(autoPASendingPtr->getAutoSendPALiveReset());
	m_chkPaPreRecordedReset.SetCheck(autoPASendingPtr->getAutoSendPAPreRecordedReset());
	m_chkPaContinue.SetCheck(autoPASendingPtr->getAutoSendPAContinue());
	m_chkPreRecordedCompl.SetCheck(autoPASendingPtr->getAutoSendPAPreRecCompl());
	m_chkPaPreRecEmergency.SetCheck(autoPASendingPtr->getAutoSendPAPreRecEmergency());
	
	autoSendInfo = m_controller->getUpdateConfForAutoChanging(ETTISType);
	TTISAutoSendInfo * autoTTISSendingPtr = (TTISAutoSendInfo * )autoSendInfo.get();
	m_chkTtisLibDownload.SetCheck(autoTTISSendingPtr->getAutoSendTTISLib());
	m_chkTtisCmdRecvFailed.SetCheck(autoTTISSendingPtr->getSendTtisCmdRecvFailed());

	autoSendInfo = m_controller->getUpdateConfForAutoChanging(EChangeOver);
	ChangeOverConfig * changeOverDataPtr = (ChangeOverConfig * )autoSendInfo.get();
	m_chkChangeOverCtrl.SetCheck(changeOverDataPtr->getStatusFailure());
	m_chkTtisChangeOver.SetCheck(changeOverDataPtr->getTtisStatusFailure());
	m_chkPecChangeOver.SetCheck(changeOverDataPtr->getPECAnsStatusFailure());
	m_chkPaLiveChangeOver.SetCheck(changeOverDataPtr->getPALiveStatusFailure());
	m_chkPaPreRecChangeOver.SetCheck(changeOverDataPtr->getPAPreRecStatusFailure());
	m_chkTtisUpdateChangeOver.SetCheck(changeOverDataPtr->getUpgradeTtisStatusFailure());
	
	int nCount = m_cmboPaLiveAutoSendOption.GetCount();

	for (int nCounter = 0; nCounter < nCount; nCounter++)
	{
		m_cmboPaLiveAutoSendOption.DeleteString(nCounter);
	}

	m_cmboPaLiveAutoSendOption.AddString(_T(ALLTRAIN));
	m_cmboPaLiveAutoSendOption.AddString(_T(ONETRAIN));
	m_cmboPaLiveAutoSendOption.AddString(_T(TWOTRAIN));

	if (m_chkPaLive.GetCheck() == BST_CHECKED)
	{
		m_cmboPaLiveAutoSendOption.EnableWindow(TRUE);
	}
	else
	{
		m_cmboPaLiveAutoSendOption.EnableWindow(FALSE);
	}
	
	USES_CONVERSION;
	autoSendInfo = m_controller->getUpdateConfForAutoChanging(ETrainCount);
	TrainCountForAutoSend * trainCountAutoSend = (TrainCountForAutoSend * )autoSendInfo.get();
	m_cmboPaLiveAutoSendOption.SelectString(0,A2W (trainCountAutoSend->getTrainCount().c_str()));
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSendingAutoRespConf::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSendingAutoRespConf::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSendingAutoRespConf::OnCheckPECBoxTick ()
{
	m_controller->onAutoSendConfPECClick(createAutoSendingPECData ());
}

void CSendingAutoRespConf::OnCheckPABoxTick ()
{
	m_controller->onAutoSendConfPaClick(createAutoSendingPAData());
}

void CSendingAutoRespConf::OnCheckTTISBoxTick ()
{
	m_controller->onAutoSendConfTTISClick(createAutoSendingTtisData());
}

void CSendingAutoRespConf::addListener(IController* controller)
{
	m_controller = controller;
}

void CSendingAutoRespConf::updateUI(MessageInfo)
{
	
}

AutoSendingPecConfInfo CSendingAutoRespConf::createAutoSendingPECData ()
{
	AutoSendingPecConfInfo autoSendingData;

	if (BST_CHECKED == m_chkPecAnswer.GetCheck())
	{
		autoSendingData.bAutoSendPecAnswer = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPecAnswer = FALSE;
	}

	if (BST_CHECKED == m_chkPecHold.GetCheck())
	{
		autoSendingData.bAutoSendPecHold = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPecHold = FALSE;
	}

	if (BST_CHECKED == m_chkPecReset.GetCheck())
	{
		autoSendingData.bAutoSendPecReset = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPecReset = FALSE;
	}

	return autoSendingData;
}

AutoSendingPAConfInfo CSendingAutoRespConf::createAutoSendingPAData ()
{
	AutoSendingPAConfInfo autoSendingData;

	if (BST_CHECKED == m_chkPaLive.GetCheck())
	{
		autoSendingData.bAutoSendLive = TRUE;
		m_cmboPaLiveAutoSendOption.EnableWindow(TRUE);
	}
	else
	{
		autoSendingData.bAutoSendLive = FALSE;
		m_cmboPaLiveAutoSendOption.EnableWindow(FALSE);
	}

	if (BST_CHECKED == m_chkPaPreRecorded.GetCheck())
	{
		autoSendingData.bAutoSendPreRecorded = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPreRecorded = FALSE;
	}

	if (BST_CHECKED == m_chkPaLiveReset.GetCheck())
	{
		autoSendingData.bAutoSendLiveReset = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendLiveReset = FALSE;
	}

	if (BST_CHECKED == m_chkPaPreRecorded.GetCheck())
	{
		autoSendingData.bAutoSendPreRecorded = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPreRecorded = FALSE;
	}
	
	if (BST_CHECKED == m_chkPaPreRecordedReset.GetCheck())
	{
		autoSendingData.bAutoSendPreRecordedReset = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPreRecordedReset = FALSE;
	}

	if (BST_CHECKED == m_chkPaContinue.GetCheck())
	{
		autoSendingData.bContinue = TRUE;
	}
	else
	{
		autoSendingData.bContinue = FALSE;
	}
	
	if (BST_CHECKED == m_chkPreRecordedCompl.GetCheck())
	{
		autoSendingData.bAutoSendPreRecCompl = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendPreRecCompl = FALSE;
	}
	
	if (BST_CHECKED == m_chkPaPreRecEmergency.GetCheck())
	{
		autoSendingData.bPreRecEmergency = TRUE;
	}
	else
	{
		autoSendingData.bPreRecEmergency = FALSE;
	}


	return autoSendingData;
}

AutoSendingTTISConfInfo CSendingAutoRespConf::createAutoSendingTtisData ()
{
	AutoSendingTTISConfInfo autoSendingData;

	if (BST_CHECKED == m_chkTtisLibDownload.GetCheck())
	{
		autoSendingData.bAutoSendTTISLib = TRUE;
	}
	else
	{
		autoSendingData.bAutoSendTTISLib = FALSE;
	}

	if (BST_CHECKED == m_chkTtisCmdRecvFailed.GetCheck())
	{
		autoSendingData.bSendTtisCmdRecvFailed = TRUE;
	}
	else
	{
		autoSendingData.bSendTtisCmdRecvFailed = FALSE;
	}

	return autoSendingData;
}

void CSendingAutoRespConf::OnBtnSendState ()
{
	m_controller->onSendState(true);	
}


void CSendingAutoRespConf::OnBtnSendStateTkeCtrl ()
{
	m_controller->onSendState(false);	
}

void CSendingAutoRespConf::OnBtnChangeOverStatusTick ()
{
	ChangeOverConInfo changeOverInfo;

	if (BST_CHECKED == m_chkChangeOverCtrl.GetCheck())
	{
		changeOverInfo.bStatus = TRUE;
	}
	else
	{
		changeOverInfo.bStatus = FALSE;
	}

	if (BST_CHECKED == m_chkTtisChangeOver.GetCheck())
	{
		changeOverInfo.bTtisStatus = TRUE;
	}
	else
	{
		changeOverInfo.bTtisStatus = FALSE;
	}

	if (BST_CHECKED == m_chkPecChangeOver.GetCheck())
	{
		changeOverInfo.bPecAnsStatus = TRUE;
	}
	else
	{
		changeOverInfo.bPecAnsStatus = FALSE;
	}

	if (BST_CHECKED == m_chkPaLiveChangeOver.GetCheck())
	{
		changeOverInfo.bPaLiveStatus = TRUE;
	}
	else
	{
		changeOverInfo.bPaLiveStatus = FALSE;
	}

	if (BST_CHECKED == m_chkPaPreRecChangeOver.GetCheck())
	{
		changeOverInfo.bPaPreRecStatus = TRUE;
	}
	else
	{
		changeOverInfo.bPaPreRecStatus = FALSE;
	}
	
	if (BST_CHECKED == m_chkTtisUpdateChangeOver.GetCheck())
	{
		changeOverInfo.bUpdateTtisStatus = TRUE;
	}
	else
	{
		changeOverInfo.bUpdateTtisStatus = FALSE;
	}


	m_controller->onChangeOverClick(changeOverInfo);
}


void CSendingAutoRespConf::OnCbnSelchangeComboPALiveTrainCount ()
{
	TrainCountInfo trainCountInfo;
	CString cstrData;
	
	int nIndex = m_cmboPaLiveAutoSendOption.GetCurSel();
	int nLen = m_cmboPaLiveAutoSendOption.GetLBTextLen (nIndex);
	m_cmboPaLiveAutoSendOption.GetLBText(nIndex,cstrData.GetBuffer(nLen));
	
	USES_CONVERSION;
	trainCountInfo.strTrainCount = W2A (cstrData.GetBuffer(0));

	m_controller->onTrainCountCmboSelChange(trainCountInfo);
	
}


