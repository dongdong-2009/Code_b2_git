// BASManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/BASManagerDlg.h"
#include "app/bas/BASModeManager/src/ModeDeviceDlg.h"
#include "app/bas/BASModeManager/src/BASAgentHelper.h"
#include "app/bas/BASModeManager/src/SystemControlTypeDlg.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/bas/bas_agent/idl/src/IStationBasAgentCorbaDef.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxyFactory.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"
#include "core/exceptions/idl/src/CommonExceptionsCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/utilities/src/TA_String.h"
#include <vector>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Namespace_TA_IRS_APP_Begin
using namespace TA_Base_Core;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implement
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CBASManagerDlg Dialog
CBASManagerDlg::CBASManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent /*=NULL*/)
: TA_Base_Bus::TransActiveDialog(controlClass, CBASManagerDlg::IDD, pParent)
,m_initializeThread(NULL),m_changeStationThread(NULL),m_rightsMgr(NULL), m_resourceKey(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	TA_Base_Bus::ResizingProperties properties;
	properties.canMaximise	= true;
	properties.maxHeight	= -1;
	properties.maxWidth = -1;
	properties.minWidth = 654;
	properties.minHeight = 458;
	setResizingProperties(properties);
}

void CBASManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	TA_Base_Bus::TransActiveDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_cbStation);
	DDX_Control(pDX, IDC_COMBO_SUBSYSTEM, m_cbSubsystem);
	DDX_Control(pDX, IDC_COMBO_MODE_TYPE, m_cbModeType);
	DDX_Control(pDX, IDC_BTN_AUTOMANU_CFG, m_btnAutoManual);
    DDX_Control(pDX, IDC_BTN_REVOKE_MODE, m_btnRevokeMode);
	DDX_Control(pDX, IDC_BTN_RECOVER, m_btnFireRecover);
	DDX_Control(pDX, IDC_BTN_EXECUTE_MODE, m_btnExecute);
	DDX_Control(pDX, IDC_BTN_DEVICE_LIST, m_btnDeviceList);
	DDX_Control(pDX, IDC_LIST_SUBSYSTEMS, m_lstSubsytems);
	DDX_Control(pDX, IDC_LIST_MODES, m_lstModes);
}

BEGIN_MESSAGE_MAP(CBASManagerDlg, TA_Base_Bus::TransActiveDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_SUBSYSTEM, &CBASManagerDlg::OnSubsystemChanged)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION, &CBASManagerDlg::OnStationChanged)
	ON_BN_CLICKED(IDCANCEL, &CBASManagerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_APPLY_FILTER, &CBASManagerDlg::OnBnClickedBtnApplyFilter)
	ON_BN_CLICKED(IDC_BTN_DEVICE_LIST, &CBASManagerDlg::OnBnClickedBtnDeviceList)
	ON_BN_CLICKED(IDC_BTN_EXECUTE_MODE, &CBASManagerDlg::OnBnClickedBtnExecuteMode)	
	ON_BN_CLICKED(IDC_BTN_SEARCH_MODE, &CBASManagerDlg::OnBnClickedBtnSearchMode)
	ON_BN_CLICKED(IDC_BTN_RECOVER, &CBASManagerDlg::OnBnClickedBtnRecover)
	ON_BN_CLICKED(IDC_BTN_AUTOMANU_CFG, &CBASManagerDlg::OnBnClickedBtnAutomanuCfg)
    ON_BN_CLICKED(IDC_BTN_REVOKE_MODE, &CBASManagerDlg::OnBnClickedBtnRevokeMode)
	ON_MESSAGE(Msg_InitializeDialogData, &CBASManagerDlg::InitializeDialogData)
	ON_MESSAGE(Msg_UpdateCurrentMode, &CBASManagerDlg::OnUpdateCurrentMode)
	ON_MESSAGE(Msg_UpdateExpectMode, &CBASManagerDlg::OnUpdateExpectMode)
	ON_MESSAGE(Msg_UpdateSubsystemAutoManual, &CBASManagerDlg::OnUpdateSubsystemAutoManual)
	ON_MESSAGE(Msg_UpdateModeSatus, &CBASManagerDlg::OnUpdateModeSatus)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODES, &CBASManagerDlg::OnHdnItemclickModeList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SUBSYSTEMS, &CBASManagerDlg::OnHdnItemclickSubsystemList)


	///}}AFX_MSG_MAP	
END_MESSAGE_MAP()


// CBASManagerDlg Message Mapping
BOOL CBASManagerDlg::OnInitDialog()
{
	// calculate the fix left area for dialog resize
	calculateLeftFixedArea();

	TA_Base_Bus::TransActiveDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO: add the initialize code	
	// setup the list ctrl header
	setupListCtrls();

	m_modeListImageList.Create(1,16,ILC_COLOR24,0,0);
	m_lstModes.SetImageList(&m_modeListImageList, LVSIL_SMALL);

	// register mode status and subsystem modes update message
	SubSystemStatusInstance::instance()->registerListener(this);
	ModeStatusListenerManagerInstance::instance()->addListener(this);

	if (TA_Base_Core::RunParams::getInstance().get(RPARAM_MANAGERSTATUS).compare(MANAGERSTATUS_FIRE) == 0)
	{
		m_originalStationStatus = TA_IRS_Core::FireStatus_Fire;
		changeToFireMode();
	}
	else
	{
		m_originalStationStatus = TA_IRS_Core::FireStatus_Normal;
		changeToNormalMode();
	}

	//create the initialize thread
	m_initializeThread = new BasManagerActionThread(this);
	m_initializeThread->setAction(BasManagerAction_Initialize);
	m_initializeThread->start();

	TA_Base_Bus::RightsLibraryFactory rightsFactory;
	m_rightsMgr = rightsFactory.buildRightsLibrary();

	// resize the dialog
	PostMessage(WM_SIZE, 0, 0);

	// start the timers
	SetTimer(ModeListCtrlColorTimer, ModeListCtrlColorTimerInternal, NULL);

	return TRUE;
}

void CBASManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		TA_Base_Bus::TransActiveDialog::OnPaint();
	}
}

//
//
HCURSOR CBASManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBASManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	FUNCTION_ENTRY("OnSize");
	TA_Base_Bus::TransActiveDialog::OnSize(nType, cx, cy);
	if (FALSE != IsWindow(this->m_hWnd) &&
		BAS_IsWindow(IDC_LIST_MODES) && BAS_IsWindow(IDC_STATIC_STATION_MODE) && BAS_IsWindow(IDC_LIST_SUBSYSTEMS))
	{
		// get the dlg client Rect
		CRect clientRect;
		GetClientRect(&clientRect);

		// calculate the right area and move modes list to right area
		CRect CRectRightArea;
		CRectRightArea.left = m_dlgFixSizeInfo.leftTopFixArea.right + gDlgLeftBlank;
		CRectRightArea.top = clientRect.top + gDlgTopBlank;
		CRectRightArea.bottom = clientRect.bottom - 2*gDlgBottomBlank - m_dlgFixSizeInfo.btnExecuteMode.Height();
		CRectRightArea.right = clientRect.right - gDlgRightBlank;
		m_lstModes.MoveWindow(CRectRightArea, FALSE);
		//m_lstModes.resizeLastColumnWidth();
		unsigned long modeList_cx = CRectRightArea.right;
		unsigned long modeList_cy = CRectRightArea.bottom;
		BAS_MoveFixedSizeCtrl(IDC_BTN_EXECUTE_MODE, 
			(modeList_cx - 2*m_dlgFixSizeInfo.btnExecuteMode.Width() - 2*gDlgLeftBlank),
			modeList_cy + gDlgBottomBlank );
		BAS_MoveFixedSizeCtrl(IDC_BTN_DEVICE_LIST,
			(modeList_cx - m_dlgFixSizeInfo.btnExecuteMode.Width()),
			modeList_cy + gDlgBottomBlank);

		// calculate the left middle area and move the mode status list
		CRect LeftMidRect;
		CRect leftMideListRect;
		LeftMidRect.left = m_dlgFixSizeInfo.leftTopFixArea.left;
		LeftMidRect.top = m_dlgFixSizeInfo.leftTopFixArea.bottom + gDlgTopBlank;
		LeftMidRect.right = m_dlgFixSizeInfo.leftTopFixArea.right;
		LeftMidRect.bottom = clientRect.bottom - m_dlgFixSizeInfo.leftBottomFixArea.Height() 
			- m_dlgFixSizeInfo.btnFireMode.Height() - 3 * gDlgTopBlank;
		GetDlgItem(IDC_STATIC_STATION_MODE)->MoveWindow(LeftMidRect, FALSE);
		leftMideListRect.top = LeftMidRect.top + gDlgTopBlank * 2;
		leftMideListRect.left = LeftMidRect.left + gDlgLeftBlank;
		leftMideListRect.right = LeftMidRect.right - gDlgRightBlank;
		leftMideListRect.bottom = LeftMidRect.bottom - gDlgBottomBlank;
		m_lstSubsytems.MoveWindow(leftMideListRect, FALSE);

		// move the left bottom ctrls
		unsigned long search_cx = LeftMidRect.left;
		unsigned long search_cy = LeftMidRect.bottom + gDlgTopBlank;
		BAS_MoveFixedSizeCtrl(IDC_STATIC_SEARCH, 
			search_cx,
			search_cy);
		BAS_MoveFixedSizeCtrl(IDC_STATIC_MODECODE, 
			search_cx + (m_dlgFixSizeInfo.modeCodeStatic.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
			search_cy + (m_dlgFixSizeInfo.modeCodeStatic.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
		BAS_MoveFixedSizeCtrl(IDC_EDIT_MODE_NUM, 
			search_cx + (m_dlgFixSizeInfo.searchText.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
			search_cy + (m_dlgFixSizeInfo.searchText.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
		BAS_MoveFixedSizeCtrl(IDC_BTN_SEARCH_MODE, 
			search_cx + (m_dlgFixSizeInfo.searchButton.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
			search_cy + (m_dlgFixSizeInfo.searchButton.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
		BAS_MoveFixedSizeCtrl(IDC_BTN_AUTOMANU_CFG,
			search_cx + (m_dlgFixSizeInfo.btnAutoManual.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
			search_cy + (m_dlgFixSizeInfo.btnAutoManual.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
        BAS_MoveFixedSizeCtrl(IDC_BTN_REVOKE_MODE,
            search_cx + (m_dlgFixSizeInfo.btnRevokeMode.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
            search_cy + (m_dlgFixSizeInfo.btnRevokeMode.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
		BAS_MoveFixedSizeCtrl(IDC_BTN_RECOVER,
			search_cx + (m_dlgFixSizeInfo.btnFireMode.left - m_dlgFixSizeInfo.leftBottomFixArea.left),
			search_cy + (m_dlgFixSizeInfo.btnFireMode.top - m_dlgFixSizeInfo.leftBottomFixArea.top));
		Invalidate();//Cuilike++
	}	
	FUNCTION_EXIT;
}

void CBASManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ModeListCtrlColorTimer)
	{
		m_lstModes.RefreshGUI(nIDEvent);
	}
	else if (ManagerQuitColorTimer)
	{
		if (true == managerCanQuitNow())
		{
			PostMessage(WM_CLOSE, 0, 0);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CBASManagerDlg::OnClose()
{
	if (true == BasManagerDataHelperInstance::instance()->getIsManagerInFireMode() && m_lstModes.GetItemCount() > 0)
	{
		bool bHaveUnprocessMode = false;
		for (int index = 0; index < m_lstModes.GetItemCount(); index++)
		{
			CString strStatus = m_lstModes.GetItemText(index, ModesListCtrl::COLUMN_INDEX_MODE_STATUS);
			int modeStatus = ModeRunningStatusConfigManager::getModeStatusByName(strStatus.GetBuffer());
			if (eModeExecutionSuccess != modeStatus)
			{
				bHaveUnprocessMode = true;
				break;
			}			
		}

		if (bHaveUnprocessMode)
		{
			if(IDYES == MessageBox(_T("当前有未处理的火灾模式，真的要退出吗?"), MessageBox_Caption, MB_ICONQUESTION | MB_YESNO))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "There have some station fire mode need process, but operator ignore it. following is the modes info");
				logCurrentModesInfo();
			}
			else
			{
				return;
			}
		}
	}

	KillTimer(ModeListCtrlColorTimer);
	KillTimer(ManagerQuitColorTimer);
	if (false == managerCanQuitNow())
	{
		SetTimer(ManagerQuitColorTimer,ManagerQuitColorTimerInternal, NULL);
	}
	else
	{
		TA_Base_Bus::TransActiveDialog::OnClose();
	}	
}

bool CBASManagerDlg::managerCanQuitNow()
{
	bool canQuit = true;
	if (NULL != m_initializeThread)
	{
		TA_Base_Core::Thread::ThreadState_t threadStatus = m_initializeThread->getCurrentState();
		if (TA_Base_Core::Thread::THREAD_STATE_RUNNING == threadStatus || TA_Base_Core::Thread::THREAD_STATE_NEW == threadStatus)
		{
			canQuit = false;
		}
	}
	return canQuit;
}

void CBASManagerDlg::OnBnClickedCancel()
{
	FUNCTION_ENTRY("OnBnClickedCancel");
	// stop the timers
	KillTimer(ModeListCtrlColorTimer);
	FUNCTION_EXIT;
	TA_Base_Bus::TransActiveDialog::OnCancel();
}

void CBASManagerDlg::OnBnClickedBtnApplyFilter()
{
	FUNCTION_ENTRY("OnBnClickedBtnApplyFilter");
	using namespace TA_IRS_Core;
	// TODO: Add your control notification handler code here
	m_lstModes.DeleteAllItems();

	//update button status when station changed,because no select any modes
	updateControlModeButtonsStatus();

	// determine the station selected
    int nCurSelStationIdx = m_cbStation.GetCurSel();

    if ( CB_ERR == nCurSelStationIdx )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No Station Selected (%d) when apply BAS mode filter", nCurSelStationIdx );
        return;
    }

    unsigned long locationKey = static_cast<unsigned long>(m_cbStation.GetItemData(nCurSelStationIdx));

    // determine the subsystem selected
    unsigned long ulSubSystemKey = static_cast<unsigned long>(m_cbSubsystem.GetItemData(m_cbSubsystem.GetCurSel()));

	unsigned long modeTypeKey = 0;
	if (m_cbModeType.GetCurSel() == CB_ERR)
	{
		modeTypeKey = BasManagerDataHelperInstance::instance()->getAllModeTypeItemKey();
	}
	else
	{
		modeTypeKey = static_cast<unsigned int>(m_cbModeType.GetItemData(m_cbModeType.GetCurSel()));
		// determine the mode type selected
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"filter modes use station:%lu, subsystem:%lu, modetype:%lu",
		locationKey, ulSubSystemKey, modeTypeKey);

	// is the merge subsystem
	MapOrderToSubsystem subSystems = BasManagerDataHelperInstance::instance()->getAllSubsystems(locationKey);

	std::vector<unsigned long> subsystemKeyVct;
	for (MapOrderToSubsystem::iterator it = subSystems.begin(); it != subSystems.end(); it++)
	{
        if (!it->second->getIsMerge() && (it->second->getParentKey() == ulSubSystemKey))
		{
			subsystemKeyVct.push_back(it->second->getKey());
		}
    }

	// if not the merge subsystem, then use the selected subsystems
	if (subsystemKeyVct.empty())
	{
		subsystemKeyVct.push_back(m_cbSubsystem.GetItemData(m_cbSubsystem.GetCurSel()));
	}

	std::vector<Mode_SmartPtr> modes;
	BAS_TRY
	{
		modes = BasManagerDataHelperInstance::instance()->getModes(locationKey, subsystemKeyVct, modeTypeKey);
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;

	if (BAS_WhetherHaveError)
	{
		MessageBox(_T("没有找到符合条件的模式"), MessageBox_Caption, MB_ICONERROR);
	}
	else
	{
		KillTimer(ModeListCtrlColorTimer);
		fillModeListCtrl(modes);
		SetTimer(ModeListCtrlColorTimer, ModeListCtrlColorTimerInternal, NULL);
	}
	FUNCTION_EXIT;
}


void CBASManagerDlg::setupListCtrls()
{
	FUNCTION_ENTRY("setupListCtrls");
	// Current Subsystem Status List
	m_lstSubsytems.InsertColumn(ReportListCtrl::COLUMN_INDEX_REPORT_SUBSYSTEM, _T("子系统"), LVCFMT_LEFT, 100);
	m_lstSubsytems.InsertColumn(ReportListCtrl::COLUMN_INDEX_REPORT_TARGETMODE, _T("HMI下发模式"), LVCFMT_LEFT, 100);
	m_lstSubsytems.InsertColumn(ReportListCtrl::COLUMN_INDEX_REPORT_CURRENTMODE, _T("当前模式"), LVCFMT_LEFT, 80);
	m_lstSubsytems.InsertColumn(ReportListCtrl::COLUMN_INDEX_REPORT_AUTOMANUAL, _T("系统控制方式"), LVCFMT_LEFT, 100);

	// Mode List
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_INDEX, _T("模式编号"), LVCFMT_LEFT, 80);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_VALUE, _T("模式值"), LVCFMT_LEFT, 70);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_LOCATION, _T("车站"), LVCFMT_LEFT,100);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_SUBSYSTEM, _T("子系统"), LVCFMT_LEFT, 130);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_TYPE, _T("模式类型"), LVCFMT_LEFT, 70);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_DESC, _T("模式描述"), LVCFMT_LEFT, 320);
	m_lstModes.InsertColumn(ModesListCtrl::COLUMN_INDEX_MODE_STATUS, _T("模式状态"), LVCFMT_LEFT, 70);
	FUNCTION_EXIT;
}

void CBASManagerDlg::executeThreadFunction(BasManagerAction action)
{
	FUNCTION_ENTRY("executeThreadFunction");
	switch (action)
	{
	case BasManagerAction_Initialize:
		InitializeDialogData();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "InitializeDialogData Finished");
		break;
	case BasManagerAction_ChangeStation:
		subscribeSubsystemUpdate();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "subscribeSubsystemUpdate Finished");
		break;
	}

	FUNCTION_EXIT;
}

void CBASManagerDlg::InitializeDialogData()
{
	InitializeDialogData(static_cast<WPARAM>(0), static_cast<LPARAM>(0));
}

void CBASManagerDlg::prepareClose()
{
	FUNCTION_ENTRY("prepareClose");
	BAS_TRY
	{
		// unsubscribe corba message
        unsubscribeStationFireMessage();

        if (NULL != m_initializeThread)
        {
            m_initializeThread->terminateAndWait();
            delete m_initializeThread;
            m_initializeThread = NULL;
        }
        if (NULL != m_changeStationThread)
        {
            m_changeStationThread->terminateAndWait();
            delete m_changeStationThread;
            m_changeStationThread = NULL;
        }

		// There should do some thing
		m_basManagerSubscriber.stopSubscriber();
		DeviceEquipmentManagerInstance::instance()->resetListener(NULL);
		DeviceEquipmentManagerInstance::instance()->prepareClose();
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;

	LOG_GENERIC(SourceInfo ,TA_Base_Core::DebugUtil::DebugInfo, "BasModeMangaer have been prepare to close");
	FUNCTION_EXIT;
}

LRESULT CBASManagerDlg::InitializeDialogData(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("InitializeDialogData");
	using namespace TA_IRS_Core;

	// clear the data first
	m_cbStation.ResetContent();
	m_cbSubsystem.ResetContent();
	m_cbModeType.ResetContent();

	//Initial buttons status
	if (!isActionPermitted(TA_Base_Bus::aca_SET_MODE_ECS))
	{
		m_btnFireRecover.EnableWindow(FALSE);
	}
	updateControlSubsystemButtonsStatus();
	updateControlModeButtonsStatus();

	unsigned long locationKey = 0;
	{
		// Station Selector
		int index = 0;
		if (BasManagerDataHelperInstance::instance()->isAtStation())
        {
            ILocation_SmartPtr LocationPtr = BasManagerDataHelperInstance::instance()->getLocalLocation();
            if ( NULL != LocationPtr.get() )
            {
                index = m_cbStation.InsertString(m_cbStation.GetCount(), _T(LocationPtr->getDisplayName().c_str()));
                m_cbStation.SetItemData(index, LocationPtr->getKey());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Local Location is not loaded" );
            }
		}
		else
		{
            std::vector<ILocation_SmartPtr> locations = BasManagerDataHelperInstance::instance()->getAllLocations();
			for (std::vector<ILocation_SmartPtr>::iterator it = locations.begin(); it != locations.end(); it++)
			{
				if ((*it)->getLocationType() == TA_Base_Core::ILocation::STATION)
				{
					index = m_cbStation.InsertString(m_cbStation.GetCount(), _T((*it)->getDisplayName().c_str()));
					m_cbStation.SetItemData(index, (*it)->getKey());
				}
			}

            index = 0;

            if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_FIRELOCATIONKEY))
            {
                unsigned long ulFireLocation = BasManagerDataHelperInstance::instance()->getFireLocationKey();
                int nFireStationIndex = getStationComboxIndexByData( ulFireLocation );

                if ( CB_ERR != nFireStationIndex )
                {
                    index = nFireStationIndex;
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No location key (%lu) found in Station ComboBox", ulFireLocation );
                }
            }
		}

		m_cbStation.SetCurSel(index);
		locationKey = static_cast<unsigned long>(m_cbStation.GetItemData(index));
		m_lstSubsytems.prepareChangeToLocation(locationKey);
	}

	{
		// Subsystem Selector	
		updateSubsystemList(locationKey);

		OnSubsystemChanged();
    }

    if (TA_Base_Core::RunParams::getInstance().get(RPARAM_MANAGERSTATUS).compare(MANAGERSTATUS_FIRE) == 0)
    {
        processStationFireFromCommandLine();
    }

    // subscribe corba message
    subscribeStationFireMessage();

	// initialize the message and data point lib factory
	changeToLocationStatus( locationKey );
    subscribeSubsystemUpdate();

	FUNCTION_EXIT;
	return 0;
}

LRESULT CBASManagerDlg::OnUpdateCurrentMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("UpdateCurrentMode");
	using namespace TA_IRS_Core;
	unsigned long modekey = static_cast<unsigned long>(wParam);
    unsigned long subsystemKey = static_cast<unsigned long>(lParam);
	Mode_SmartPtr mode_ptr = BasManagerDataHelperInstance::instance()->getModeByKey(modekey);

	if (NULL != mode_ptr.get())
	{
		m_lstSubsytems.UpdateCurrentMode(mode_ptr);
	}
	else
	{
        m_lstSubsytems.UpdateCurrentModeFail(subsystemKey);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "not find the mode to set current mode. modekey=%d", modekey);
	}

	FUNCTION_EXIT;
	return 0;
}

LRESULT CBASManagerDlg::OnUpdateExpectMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("UpdateExpectMode");
	using namespace TA_IRS_Core;
	unsigned long modekey = static_cast<unsigned long>(wParam);
	unsigned long subsystemKey = static_cast<unsigned long>(lParam);
	Mode_SmartPtr mode_ptr = BasManagerDataHelperInstance::instance()->getModeByKey(modekey);

	if (NULL != mode_ptr.get())
	{
		m_lstSubsytems.UpdateExpectMode(mode_ptr);
	}
	else
	{
		m_lstSubsytems.UpdateExpectModeFail(subsystemKey);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "not find the mode to set current mode. modekey = %d so we clear the running mode subsystemKey:%d", modekey, subsystemKey);
	}
	FUNCTION_EXIT;
	return 0;
}

LRESULT CBASManagerDlg::OnUpdateSubsystemAutoManual(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("OnUpdateSubsystemAutoManual");

	unsigned long autoManual = static_cast<unsigned long>(wParam);
	unsigned long SubsystemKey = static_cast<unsigned long>(lParam);
	m_lstSubsytems.UpdateAutoManual(autoManual, SubsystemKey);

	FUNCTION_EXIT;
	return 0;
}

LRESULT CBASManagerDlg::OnUpdateModeSatus(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("UpdateModeSatus");

	unsigned long modekey = static_cast<unsigned long>(wParam);
	int status = static_cast<int>(lParam);
	m_lstModes.UpdateModeSatus(modekey, status);

    //Update button status.
    updateControlModeButtonsStatus();

	FUNCTION_EXIT;
	return 0;
}

void CBASManagerDlg::calculateLeftFixedArea()
{
	FUNCTION_ENTRY("calculateLeftFixedArea");
	static bool bInit = false;

	if (false == bInit)
	{		
		GetDlgItem(IDC_STATIC_FILTER)->GetWindowRect(&m_dlgFixSizeInfo.leftTopFixArea);
		ScreenToClient(&m_dlgFixSizeInfo.leftTopFixArea);
		GetDlgItem(IDC_STATIC_SEARCH)->GetWindowRect(&m_dlgFixSizeInfo.leftBottomFixArea);
		ScreenToClient(&m_dlgFixSizeInfo.leftBottomFixArea);
		GetDlgItem(IDC_BTN_RECOVER)->GetWindowRect(&m_dlgFixSizeInfo.btnFireMode);
		ScreenToClient(&m_dlgFixSizeInfo.btnFireMode);
		GetDlgItem(IDC_BTN_AUTOMANU_CFG)->GetWindowRect(&m_dlgFixSizeInfo.btnAutoManual);
		ScreenToClient(&m_dlgFixSizeInfo.btnAutoManual);
		GetDlgItem(IDC_BTN_REVOKE_MODE)->GetWindowRect(&m_dlgFixSizeInfo.btnRevokeMode);
		ScreenToClient(&m_dlgFixSizeInfo.btnRevokeMode);
		GetDlgItem(IDC_STATIC_MODECODE)->GetWindowRect(&m_dlgFixSizeInfo.modeCodeStatic);	
		ScreenToClient(&m_dlgFixSizeInfo.modeCodeStatic);
		GetDlgItem(IDC_EDIT_MODE_NUM)->GetWindowRect(&m_dlgFixSizeInfo.searchText);		
		ScreenToClient(&m_dlgFixSizeInfo.searchText);
		GetDlgItem(IDC_BTN_SEARCH_MODE)->GetWindowRect(&m_dlgFixSizeInfo.searchButton);		
		ScreenToClient(&m_dlgFixSizeInfo.searchButton);
		GetDlgItem(IDC_BTN_EXECUTE_MODE)->GetWindowRect(&m_dlgFixSizeInfo.btnExecuteMode);
		ScreenToClient(&m_dlgFixSizeInfo.btnExecuteMode);
		GetDlgItem(IDC_BTN_DEVICE_LIST)->GetWindowRect(&m_dlgFixSizeInfo.btnDeviceList);
		ScreenToClient(&m_dlgFixSizeInfo.btnDeviceList);

		bInit = true;
	}
	FUNCTION_EXIT;
}
void CBASManagerDlg::OnBnClickedBtnDeviceList()
{
	// TODO: Add your control notification handler code here
	FUNCTION_ENTRY("OnBnClickedBtnDeviceList");
	// get the select mode item
	POSITION position = m_lstModes.GetFirstSelectedItemPosition();
	int index = m_lstModes.GetNextSelectedItem(position);

	BAS_TRY
	{
		if(index >= 0)
		{
			unsigned long modeKey = static_cast<unsigned long>(m_lstModes.GetItemData(index));
			Mode_SmartPtr modePtr = BasManagerDataHelperInstance::instance()->getModeByKey(modeKey);

			CModeDeviceDlg deviceDlg(modePtr);
			deviceDlg.DoModal();
		}
		else
		{
			MessageBox(_T("您还没有选择模式，请先选择一个模式"), MessageBox_Caption, MB_ICONERROR);
		}
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;

	if(BAS_WhetherHaveError)
	{
		MessageBox(_T("程序某些功能运行异常，不能正常初始化设备列表！"), MessageBox_Caption, MB_ICONERROR);
	}
	FUNCTION_EXIT;
}

void CBASManagerDlg::OnSubsystemChanged()
{
	FUNCTION_ENTRY("OnSubsystemChanged");

    int nCurSelStationIdx = m_cbStation.GetCurSel();

    if ( CB_ERR == nCurSelStationIdx )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No Station Selected (%d) when subsystem changed", nCurSelStationIdx );
        return;
    }

	unsigned long locationKey = m_cbStation.GetItemData(nCurSelStationIdx);
	unsigned long subsystemKey = m_cbSubsystem.GetItemData(m_cbSubsystem.GetCurSel());
	m_cbModeType.ResetContent();
	BAS_TRY
	{
		SubSystemModeTypeMap typeMap = BasManagerDataHelperInstance::instance()->getSubsystemModeTypeMappingByLocation(locationKey);

		TA_IRS_Core::modeTypes modetypes = typeMap[subsystemKey];

		ModeTypeMap modeTypeMap = BasManagerDataHelperInstance::instance()->getAllModeTypes();

		for (TA_IRS_Core::modeTypes::iterator it = modetypes.begin(); it != modetypes.end(); it++)
		{
			ModeTypeMap::iterator modeTypeIter = modeTypeMap.find(*it) ;
			if (modeTypeIter != modeTypeMap.end())
			{
				int index = m_cbModeType.InsertString(m_cbModeType.GetCount(), _T(modeTypeIter->second.modeTypeName.c_str()));
				m_cbModeType.SetItemData(index, modeTypeIter->first );
			}
		}

		if (m_cbModeType.GetCount() > 0)
		{
			m_cbModeType.SetCurSel(0);
		}
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;
	FUNCTION_EXIT;
}

void CBASManagerDlg::OnStationChanged()
{
    FUNCTION_ENTRY("OnStationChanged");

    int nCurSelStationIdx = m_cbStation.GetCurSel();

    if ( CB_ERR == nCurSelStationIdx )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No Station Selected (%d) change station", nCurSelStationIdx );
        return;
    }

    unsigned long locationKey = m_cbStation.GetItemData(nCurSelStationIdx);

    changeToLocationStatus(locationKey);
    updateSubsystemList(locationKey);
    OnSubsystemChanged();
    m_lstModes.DeleteAllItems();

	//update button status when station changed,because no select any subsystem and modes
	updateControlSubsystemButtonsStatus();
	updateControlModeButtonsStatus();

	if (NULL == m_changeStationThread || m_changeStationThread->getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING)
	{
		delete m_changeStationThread;
		m_changeStationThread = NULL;
		setCurrentSelectLocation(locationKey);
		m_changeStationThread = new BasManagerActionThread(this);
		m_changeStationThread->setAction(BasManagerAction_ChangeStation);
		m_changeStationThread->start();
	}
	else
	{
		locationKey = getCurrentSelectLocationKey();
		ILocation_SmartPtr locationPtr = BasManagerDataHelperInstance::instance()->getLocationByKey(locationKey);
		CString strMessage;
		if (NULL == locationPtr.get())
		{
			strMessage.Format("程序正在准备数据,请稍后再试");
		}
		else
		{
			CString strLocationName = locationPtr->getDisplayName().c_str();
			strMessage.Format("程序正在准备%s的数据,请稍后再试", strLocationName.GetBuffer());
		}		
		MessageBox(strMessage, MessageBox_Caption, MB_ICONWARNING | MB_OK);
	}
	FUNCTION_EXIT;
}

void CBASManagerDlg::updateModeStatus(unsigned long modeKey, int status)
{
	FUNCTION_ENTRY("updateModeStatus");
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Receive a mode status update message modeKey=%d", modeKey);
	UpdateModeStatusToGUI(modeKey, status);
	FUNCTION_EXIT;
}

void CBASManagerDlg::updateSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType)
{
	FUNCTION_ENTRY("updateSubsystemMode");

	unsigned long modeKey;
	unsigned long currentSelLocation = getCurrentSelectLocationKey();
	if (currentSelLocation != locationKey)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"receive a subsystem mode update, but not current show location. current location:%d  -- receive data location:%d subsystem:%d", currentSelLocation, locationKey, subsystemKey);
		return;
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Receive a subsystem mode change message locationKey=%u subsystemKey=%u", locationKey, subsystemKey);
	}

	BAS_TRY
	{
		modeKey = SubSystemStatusInstance::instance()->getSubsystemMode(locationKey, subsystemKey, modeType);
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;
	if (BAS_WhetherHaveError)
	{
		return;// the BAS_CATCH had record the logs
	}

	// update data to GUI
	switch(modeType)
	{
	case ModeSet::modeType_RunningMode:
		UpdateExpectModeToGUI(modeKey, subsystemKey);
		break;
	case ModeSet::modeType_CurrentMode:
		UpdateCurrentModeToGUI(modeKey, subsystemKey);
		break;
	case ModeSet::modeType_AutoManul:
		UpdateSubsystemAutoManualToGUI(modeKey, subsystemKey);
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown subsystem mode type.");
	}
	FUNCTION_EXIT;
}

void CBASManagerDlg::OnBnClickedBtnExecuteMode()
{
	// TODO: Add your control notification handler code here
	FUNCTION_ENTRY("OnBnClickedBtnExecuteMode");
	// get the select mode item
	POSITION position = m_lstModes.GetFirstSelectedItemPosition();
	int index = m_lstModes.GetNextSelectedItem(position);

	if(index < 0)
	{
		MessageBox(_T("请先选择一个将要执行的模式"), MessageBox_Caption, MB_ICONERROR);
		return;
	}

	unsigned long modeKey = static_cast<unsigned long>(m_lstModes.GetItemData(index));
	Mode_SmartPtr modePtr = BasManagerDataHelperInstance::instance()->getModeByKey(modeKey);			
	unsigned long subsystemKey = modePtr->getSubsystemKey();
	unsigned long locationKey = modePtr->getLocationKey();
	std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	CString strExceptionString = "";
	try
	{
        bool isModeExecuteTimeout = SubSystemStatusInstance::instance()->getSubsystemModeTimeoutFlag(locationKey, subsystemKey);
        TA_IRS_Bus::BasAgentProxyFactoryInstance::instance()->getBasAgentProxyByLocationKey(locationKey)->executeStationMode(subsystemKey, modePtr->getValue(), isModeExecuteTimeout, session);
		//BASAgentHelperInstance::getInstance().executeStationMode(locationKey, subsystemKey, modePtr->getValue(), true, session);
		// do the mode execute action
		//ModeStatusManagerInstance::instance()->updateModeStatus(modeKey, MRS_Running);
		//BasAlarmEventGeneratorInstance::instance()->generateExecuteModeEvent(location_Ptr, modePtr);
	}
    catch(const TA_IRS_Bus::BasNotDeployStationException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		strExceptionString = "此站点还没有部署，请联系管理员！";
	}
    catch(const TA_IRS_Bus::BasAgentInvalidException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		strExceptionString = "执行模式过程中系统异常，无法连接到服务器！";
	}
	catch(const TA_Base_Core::TransactiveException& ex)
	{
		strExceptionString = ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		strExceptionString = "执行模式过程中系统异常";
	}
    catch(const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Access denied", ex.reason.in());
        strExceptionString = "无权限执行模式";
    }
	catch(const TA_Base_Core::NotAvailableException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", ex.reason.in());
		strExceptionString = "执行模式过程中数据配置异常";
	}
	catch(const std::exception& ex)
	{
		strExceptionString = ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
		strExceptionString = "执行模式过程中数据异常";
	}
	catch(const TA_IRS_Bus::IStationBasAgentCorbaDef::StationInLocalMode& ex)
	{
		strExceptionString = static_cast< const char* >(ex.what);
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::IStationBasAgentCorbaDef::StationInLocalMode", strExceptionString.GetBuffer());
		strExceptionString.Format("%s 模式只能在HMI模式控制方式时执行",getSubsystemNameByKey(locationKey,subsystemKey).c_str());		
	}
	catch(const TA_IRS_Bus::IStationBasAgentCorbaDef::StationModeInProgress& ex)
	{
		strExceptionString = static_cast< const char* >(ex.what);
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::IStationBasAgentCorbaDef::StationModeInProgress", strExceptionString.GetBuffer());
		strExceptionString.Format("%s 当前有正在执行的模式",getSubsystemNameByKey(locationKey,subsystemKey).c_str());
	}
	catch(const CORBA::Exception& ex)
	{
		strExceptionString = static_cast< const char* >(ex._name());
		LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", strExceptionString.GetBuffer());
		strExceptionString = "执行模式过程中通信异常";
	} 
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur.");
		strExceptionString = "执行模式过程中出现未知异常";
	}

	if(false == strExceptionString.IsEmpty())
	{
		MessageBox(strExceptionString, MessageBox_Caption, MB_OK | MB_ICONERROR);
	}
	FUNCTION_EXIT
}

bool CBASManagerDlg::isActionPermitted(unsigned long action)
{
	bool allowed = false;
	std::string strReason = "";
	try
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);	
		if (0 == m_resourceKey)
		{
			IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( BasManagerDataHelperInstance::instance()->getManagerEntityKey());
			if (resource != NULL)
			{
				m_resourceKey = resource->getKey();
			}
			delete resource;
		}

		allowed = m_rightsMgr->isActionPermittedOnResource(
			session, m_resourceKey, action, strReason, decisionModule);

		if (!allowed)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Not permitted to perform action=%d on BasModeManager as %s", 
				action, strReason.c_str());
		}
	}
	catch (const RightsException& err)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", err.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when isActionPermitted");
	}
	return allowed;
}

void CBASManagerDlg::changeToNormalMode()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Manager change to Normal mode.");
	((CListCtrl*)GetDlgItem(IDC_LIST_MODES))->DeleteAllItems();
	((CComboBox*)GetDlgItem(IDC_COMBO_STATION))->EnableWindow(TRUE);
	((CComboBox*)GetDlgItem(IDC_COMBO_SUBSYSTEM))->EnableWindow(TRUE);
	((CComboBox*)GetDlgItem(IDC_COMBO_MODE_TYPE))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_MODE_NUM))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BTN_SEARCH_MODE))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BTN_APPLY_FILTER))->EnableWindow(TRUE);
	SetWindowText("车站模式管理器");
	BasManagerDataHelperInstance::instance()->setManagerinFireMode(false);
}

void CBASManagerDlg::changeToFireMode()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Manager change to Fire mode.");
	((CListCtrl*)GetDlgItem(IDC_LIST_MODES))->DeleteAllItems();
	((CComboBox*)GetDlgItem(IDC_COMBO_STATION))->EnableWindow(FALSE);
	((CComboBox*)GetDlgItem(IDC_COMBO_SUBSYSTEM))->EnableWindow(FALSE);
	((CComboBox*)GetDlgItem(IDC_COMBO_MODE_TYPE))->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT_MODE_NUM))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_SEARCH_MODE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_APPLY_FILTER))->EnableWindow(FALSE);
	SetWindowText("车站模式管理器");
	BasManagerDataHelperInstance::instance()->setManagerinFireMode(true);
}

void CBASManagerDlg::subscribeStationFireMessage()
{
    if (!BasManagerDataHelperInstance::instance()->isAtStation())
    {
        MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
            TA_Base_Core::ECSAgentComms::EcsStationFireNotification, this, NULL);
    }
    else
    {
        NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, TA_Base_Core::toString(BasManagerDataHelperInstance::instance()->getManagerLocationKey()) );
        FilterData filterData;
        filterData.push_back(&regionFilter);
        MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
            TA_Base_Core::ECSAgentComms::EcsStationFireNotification, this, &filterData);
    }

}

void CBASManagerDlg::unsubscribeStationFireMessage()
{
	MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void CBASManagerDlg::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	std::string strMessageTypeKey = message.messageTypeKey.in();
	if (strMessageTypeKey.compare(TA_Base_Core::ECSAgentComms::EcsStationFireNotification.getTypeKey()) == 0)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receive a station fire notification message.");
		TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyEvent* event;
		if ((message.messageState >>= event) != 0)
		{
			unsigned long locationKey = event->locationKey;
            if (getCurrentSelectLocationKey() != locationKey)
            {
                return;
            }
			unsigned long modeValue = event->modeValue;
			TA_IRS_Core::BasStationFireStatus status = (event->status == TA_IRS_Bus::IStationBasAgentCorbaDef::StationFire_Fire) ? TA_IRS_Core::FireStatus_Fire : TA_IRS_Core::FireStatus_Normal;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receive a station fire notification message LocationKey[%u] ModeValue[%u] Status[%s].",
				locationKey, modeValue, TA_IRS_Core::FireStatus_Fire == status ? "Fire" : "Normal");
			try
			{
				sendStationFireACKEvent(locationKey, modeValue);//send the ack message first then prcess the fire message.
				receiveStationFireMessage(locationKey, modeValue, status);
			}
			catch(TA_Base_Core::TransactiveException& err)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when process the received message.");
			}
		}
	}
}

int CBASManagerDlg::getStationComboxIndexByData(DWORD data)
{
	int index = CB_ERR;

	CComboBox* comboHandle = (CComboBox*)GetDlgItem(IDC_COMBO_STATION);
	for (int nLoop = 0; nLoop < comboHandle->GetCount(); nLoop++ )
	{
		if (comboHandle->GetItemData(nLoop) == data)
		{
			index = nLoop;
			break;
		}
	}

	return index;
}

void CBASManagerDlg::receiveStationFireMessage(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status)
{
    if (!BasManagerDataHelperInstance::instance()->getIsManagerInFireMode())
    {
        return;
    }

	Mode_SmartPtr modePtr = BasManagerDataHelperInstance::instance()->getModeByValue(locationKey, modeValue);

	if (NULL != modePtr.get())
	{
		if (locationKey != getCurrentSelectLocationKey())
		{
			int locationIndex = getStationComboxIndexByData(locationKey);
            if ( CB_ERR == locationIndex )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No location key (%lu) found in Station ComboBox", locationKey );
                return;
            }
			m_cbStation.SetCurSel(locationIndex);
			OnStationChanged();
		}

		unsigned long listCount = m_lstModes.GetItemCount();
		bool bFound = false;
		for (int index = 0; index < listCount; index++)
		{
			if( m_lstModes.GetItemData(index) == modePtr->getKey())
			{
				bFound = true;
				if (TA_IRS_Core::FireStatus_Normal == status)
				{
					m_lstModes.DeleteItem(index);
					break;
				}
			}
		}

		if (false == bFound && TA_IRS_Core::FireStatus_Fire == status)
		{
			ModeTypeMap modeTypeMap = BasManagerDataHelperInstance::instance()->getAllModeTypes();
			int i = m_lstModes.GetItemCount();
			m_lstModes.InsertItem(i, _T(modePtr->getName().c_str()));
			m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_VALUE, _T(BASManagerUtility::unsignedlongToString(modePtr->getValue()).c_str()));
			m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_LOCATION, _T(BasManagerDataHelperInstance::instance()->getLocationDispNameByKey(locationKey).c_str()));
			m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_SUBSYSTEM, _T(modePtr->getSubsystemName().c_str()));		
			if (modeTypeMap.find(modePtr->getTypeKey()) != modeTypeMap.end())
			{
				m_lstModes.SetItemText(i,ModesListCtrl::COLUMN_INDEX_MODE_TYPE, _T(modeTypeMap[modePtr->getTypeKey()].modeTypeName.c_str()));
			}

			m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_DESC, _T(modePtr->getDescription().c_str()));
			m_lstModes.SetItemData(i, modePtr->getKey());
			updateListCtrlModeStatus();
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Receive a unknown mode value LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
	}

	if (m_lstModes.GetItemCount() == 0 && true == BasManagerDataHelperInstance::instance()->getIsManagerInFireMode())
	{
		changeToNormalMode();
	}
}

void CBASManagerDlg::logCurrentModesInfo()
{
	try
	{
		for (int index = 0; index < m_lstModes.GetItemCount(); index++)
		{
			CString strLocation = m_lstModes.GetItemText(index, ModesListCtrl::COLUMN_INDEX_MODE_LOCATION);
			CString strModeName = m_lstModes.GetItemText(index, ModesListCtrl::COLUMN_INDEX_MODE_INDEX);
			CString strModeDesc = m_lstModes.GetItemText(index, ModesListCtrl::COLUMN_INDEX_MODE_DESC);
			CString strStatus = m_lstModes.GetItemText(index, ModesListCtrl::COLUMN_INDEX_MODE_STATUS);

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User ignore Station Fire Location[%s] Mode[%s] ModeDescription[%s] Status[%s]",
				strLocation.GetBuffer(), strModeName.GetBuffer(), strModeDesc.GetBuffer(), strStatus.GetBuffer());
		}
	}
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknwon exception when logCurrentModesInfo");
	}

}

void CBASManagerDlg::sendStationFireACKEvent(unsigned long locationKey, unsigned long modeValue)
{
	CORBA::Any data;
	TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyACKEvent event;

	event.locationKey = locationKey;
	event.modeValue = modeValue;
	data <<= event;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::ECSAgentComms::EcsStationFireNotification);
	if (NULL != commsMessageSender)
	{
		unsigned long locationKey = BasManagerDataHelperInstance::instance()->getManagerLocationKey();			
		unsigned long entityKey = BasManagerDataHelperInstance::instance()->getManagerEntityKey();

		commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::ECSAgentComms::EcsStationFireACK, entityKey, data, NULL);
	} 
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "can not get the TA_Base_Core::ECSAgentComms::EcsLaunchApplication comms sender");
	}
}

std::string CBASManagerDlg::getSubsystemNameByKey(unsigned long locationKey, unsigned long subsystemKey)
{
	std::string strSubsystemName = "未知系统";
	SubSystem_SmartPtr subSystemPtr = BasManagerDataHelperInstance::instance()->getSubsystemByKey(locationKey, subsystemKey); 
	if (NULL != subSystemPtr.get())
	{
		strSubsystemName = subSystemPtr->getName();
	}
	return strSubsystemName;
}

void CBASManagerDlg::OnBnClickedBtnSearchMode()
{
	// TODO: Add your control notification handler code here
	m_lstModes.DeleteAllItems();
	updateControlModeButtonsStatus();

	CString strCodeName = "";
	GetDlgItem(IDC_EDIT_MODE_NUM)->GetWindowText(strCodeName);
	std::string strModeCodeName = strCodeName.GetBuffer(0);
	if (false != strModeCodeName.empty())
	{
		OnBnClickedBtnApplyFilter();
		return;
	}

	std::vector<Mode_SmartPtr> vctModes;
	BAS_TRY
	{
		vctModes = BasManagerDataHelperInstance::instance()->getModeByModeCodeName(strModeCodeName, getCurrentSelectLocationKey());
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;

	if (BAS_WhetherHaveError)
	{
		MessageBox(_T("查找匹配模式过程中异常"), MessageBox_Caption, MB_ICONERROR);
	}
	else
	{
		fillModeListCtrl(vctModes);
	}	
}

void CBASManagerDlg::fillModeListCtrl(const std::vector<TA_IRS_Core::Mode_SmartPtr> modes)
{
	FUNCTION_ENTRY("fillModeListCtrl");
	BasManagerDataHelper* managerDataHelper = BasManagerDataHelperInstance::instance();

	std::vector<TA_IRS_Core::Mode_SmartPtr>::const_iterator modeIter;
	int i = 0;
	ModeTypeMap modeTypeMap = BasManagerDataHelperInstance::instance()->getAllModeTypes();
	for (modeIter = modes.begin(); modeIter != modes.end(); ++modeIter)
	{
		m_lstModes.InsertItem(i, _T((*modeIter)->getName().c_str()));
		m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_VALUE, _T(BASManagerUtility::unsignedlongToString((*modeIter)->getValue()).c_str()));
		m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_LOCATION, _T(managerDataHelper->getLocationDispNameByKey((*modeIter)->getLocationKey()).c_str()));
		m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_SUBSYSTEM, _T((*modeIter)->getSubsystemName().c_str()));		
		if (modeTypeMap.find((*modeIter)->getTypeKey()) != modeTypeMap.end())
		{
			m_lstModes.SetItemText(i,ModesListCtrl::COLUMN_INDEX_MODE_TYPE, _T(modeTypeMap[(*modeIter)->getTypeKey()].modeTypeName.c_str()));
		}

		m_lstModes.SetItemText(i, ModesListCtrl::COLUMN_INDEX_MODE_DESC, _T((*modeIter)->getDescription().c_str()));
		m_lstModes.SetItemData(i, (*modeIter)->getKey());
		++i;
	}

	// update the mode list ctrl - mode status
	updateListCtrlModeStatus();
	FUNCTION_EXIT;
}

void CBASManagerDlg::updateListCtrlModeStatus()
{
	int count = m_lstModes.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		unsigned long modeKey = static_cast<unsigned long>(m_lstModes.GetItemData(i));
		int status = ModeStatusManagerInstance::instance()->getModeStatus(modeKey);
		UpdateModeStatusToGUI(modeKey, status);
	}
}

void CBASManagerDlg::OnBnClickedBtnRecover()
{
	// TODO: Add your control notification handler code here
	// back over the mode to normal mode
	BAS_TRY
	{
		unsigned long locationKey = m_cbStation.GetItemData(m_cbStation.GetCurSel());
		m_basManagerSubscriber.setRecoverFromFireMode(locationKey);
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;
	if (BAS_WhetherHaveError)
	{
		CString strErrorMsg;
		//strErrorMsg.Format("设置火灾恢复失败，原因：\n%s", BAS_GetExceptionString);
		strErrorMsg = "设置火灾恢复失败";
		MessageBox(strErrorMsg, MessageBox_Caption, MB_OK | MB_ICONERROR);
	}

}

void CBASManagerDlg::OnBnClickedBtnRevokeMode()
{
    unsigned long locationKey = m_cbStation.GetItemData(m_cbStation.GetCurSel());
    POSITION pos = m_lstSubsytems.GetFirstSelectedItemPosition();
    if (NULL != pos)
    {
        BAS_TRY
        {
            int item = m_lstSubsytems.GetNextSelectedItem(pos);
            unsigned long subsystemKey = m_lstSubsytems.GetItemData(item);
            int newValue = eSetControlTypeRevoke;
            m_basManagerSubscriber.setSubsystemAutoCfgValue(locationKey, subsystemKey, newValue);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set the revoked mode config: newValue[%f]", newValue);
        }
        BAS_CATCH_UNDEPLOY_EXCEPTION;

        if (BAS_WhetherHaveError)
        {
            CString strErrorMsg;
            strErrorMsg.Format("设置撤销模式失败");
            MessageBox(strErrorMsg , MessageBox_Caption, MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBox("请先选择一个子系统", MessageBox_Caption, MB_OK | MB_ICONWARNING);
    }
}

void CBASManagerDlg::OnBnClickedBtnAutomanuCfg()
{
	// TODO: Add your control notification handler code here
	// pop up the inspect panel or set the selected subsystem
	unsigned long locationKey = m_cbStation.GetItemData(m_cbStation.GetCurSel());
	POSITION pos = m_lstSubsytems.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
        std::string currentValueLabel = "";
		int newValue = 0;
		BAS_TRY
		{
			int item = m_lstSubsytems.GetNextSelectedItem(pos);
			unsigned long subsystemKey = m_lstSubsytems.GetItemData(item);
			currentValueLabel = m_lstSubsytems.GetItemText(item,m_lstSubsytems.COLUMN_INDEX_REPORT_AUTOMANUAL).GetBuffer();

            CSystemControlTypeDlg systemControlTypeDlg(m_lstSubsytems.GetItemText(item,m_lstSubsytems.COLUMN_INDEX_REPORT_SUBSYSTEM).GetBuffer());
            int nOperatorSel = systemControlTypeDlg.DoModal();
            if ( IDCANCEL == nOperatorSel )
            {
                return;
            }
            newValue = systemControlTypeDlg.getSystemControlValue();
            m_basManagerSubscriber.setSubsystemAutoCfgValue(locationKey, subsystemKey, newValue);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set the auto or manually config:  newValue[%f]", newValue);
		}
		BAS_CATCH_UNDEPLOY_EXCEPTION;

		if (BAS_WhetherHaveError)
		{
			CString strErrorMsg;
			strErrorMsg.Format("设置系统控制方式失败");
			MessageBox(strErrorMsg , MessageBox_Caption, MB_OK | MB_ICONERROR);
		}
		else
		{
			CString strErrorMsg;
			strErrorMsg.Format("设置系统控制方式成功，由[%s]设置为[%s]", currentValueLabel.c_str(), BASManagerUtility::systemControlModeValueToString(newValue).c_str());
			MessageBox(strErrorMsg , MessageBox_Caption, MB_OK);
		}
	}
	else
	{
		MessageBox("请先选择一个子系统", MessageBox_Caption, MB_OK | MB_ICONWARNING);
	}
}


void CBASManagerDlg::UpdateCurrentModeToGUI(unsigned long modeKey, unsigned long subsystemKey)
{
	FUNCTION_ENTRY("UpdateCurrentMode");
	PostMessage(Msg_UpdateCurrentMode, static_cast<WPARAM>(modeKey), static_cast<LPARAM>(subsystemKey));
	FUNCTION_EXIT;
}

void CBASManagerDlg::UpdateExpectModeToGUI(unsigned long modeKey, unsigned long subsystemKey)
{
	FUNCTION_ENTRY("UpdateExpectMode");
	PostMessage(Msg_UpdateExpectMode, static_cast<WPARAM>(modeKey), static_cast<LPARAM>(subsystemKey));
	FUNCTION_EXIT;
}

void CBASManagerDlg::UpdateSubsystemAutoManualToGUI(unsigned long autoManual, unsigned long subsystemKey)
{
	FUNCTION_ENTRY("UpdateExpectMode");
	PostMessage(Msg_UpdateSubsystemAutoManual, static_cast<WPARAM>(autoManual), static_cast<LPARAM>(subsystemKey));
	FUNCTION_EXIT;
}

void CBASManagerDlg::UpdateModeStatusToGUI(unsigned long modeKey, int status)
{
	FUNCTION_ENTRY("UpdateModeSatus");
	PostMessage(Msg_UpdateModeSatus, static_cast<WPARAM>(modeKey), static_cast<LPARAM>(status));
	FUNCTION_EXIT;
}

bool CBASManagerDlg::isLaunchInStation(ILocation_SmartPtr locationPtr)
{
	FUNCTION_ENTRY("isLaunchInStation");	
	FUNCTION_EXIT;
	return locationPtr->getLocationType() == TA_Base_Core::ILocation::STATION ? true : false;
}

void CBASManagerDlg::OnHdnItemclickModeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_ENTRY("OnHdnItemclickModeList");

	*pResult = 0;

	//update buttons status
	updateControlModeButtonsStatus();

	FUNCTION_EXIT;
}

void CBASManagerDlg::OnHdnItemclickSubsystemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_ENTRY("OnHdnItemclickSubsystemList");

	*pResult = 0;

	//update button status
	updateControlSubsystemButtonsStatus();

	FUNCTION_EXIT;
}

void CBASManagerDlg::subscribeSubsystemUpdate()
{
    m_basManagerSubscriber.startSubscriber( getCurrentSelectLocationKey() );
}

void CBASManagerDlg::changeToLocationStatus(unsigned long locationKey)
{
	FUNCTION_ENTRY("changeToLocationStatus");

	setCurrentSelectLocation(locationKey);
	BAS_TRY
	{
		m_lstSubsytems.prepareChangeToLocation(locationKey);
		for (int index = 0; index < m_lstSubsytems.GetItemCount(); index++)
		{			
			unsigned long subsystemKey = m_lstSubsytems.GetItemData(index);
			unsigned long modeKey = SubSystemStatusInstance::instance()->getSubsystemMode(locationKey, subsystemKey, ModeSet::modeType_RunningMode);
			{
				// update the running modes
				Mode_SmartPtr runningMode = BasManagerDataHelperInstance::instance()->getModeByLocAndKey(locationKey, modeKey);
				if (NULL != runningMode.get())
				{
					m_lstSubsytems.UpdateExpectMode(runningMode);
				}
			}

			{
				// update the current modes
				unsigned long modeKey = SubSystemStatusInstance::instance()->getSubsystemMode(locationKey, subsystemKey, ModeSet::modeType_CurrentMode);
				Mode_SmartPtr currentMode = BasManagerDataHelperInstance::instance()->getModeByLocAndKey(locationKey, modeKey);
				if (NULL != currentMode.get())
				{
					m_lstSubsytems.UpdateCurrentMode(currentMode);
				}
			}

			{
				// update the auto status
				unsigned long autoManual = SubSystemStatusInstance::instance()->getSubsystemMode(locationKey, subsystemKey, ModeSet::modeType_AutoManul);
				m_lstSubsytems.UpdateAutoManual(autoManual, subsystemKey);
			}
		}
	}
	BAS_CATCH_UNDEPLOY_EXCEPTION;
	if (BAS_WhetherHaveError)
	{
		MessageBox(_T("程序某些功能异常，可能将不会更新模式状态！"), _T(MessageBox_Caption), MB_ICONERROR);
	}

	FUNCTION_EXIT;
}

void CBASManagerDlg::setCurrentSelectLocation(unsigned long locationKey)
{
	TA_THREADGUARD(m_locationSelectLock);
	m_currentSelectLocationKey = locationKey;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Change to location = %lu", locationKey);
}

unsigned long CBASManagerDlg::getCurrentSelectLocationKey()
{
	TA_THREADGUARD(m_locationSelectLock);
	return m_currentSelectLocationKey;
}

void CBASManagerDlg::updateSubsystemList(unsigned long locationKey)
{
	m_cbSubsystem.ResetContent();

	MapOrderToSubsystem subsystems = BasManagerDataHelperInstance::instance()->getAllSubsystems(locationKey);

	for (MapOrderToSubsystem::iterator it = subsystems.begin(); it != subsystems.end(); ++it)
	{
		// fill the LEFT subsystem CComboBox
		std::string strMergeName;
        if (!it->second->getIsMerge() && TA_Base_Core::DatabaseKey::isValidKey(it->second->getParentKey()))
		{
			continue;
		}
		else
		{
			strMergeName = it->second->getName();
		}
		int index = m_cbSubsystem.InsertString(m_cbSubsystem.GetCount(), _T(strMergeName.c_str()));
		m_cbSubsystem.SetItemData(index, it->second->getKey());
	}
	m_cbSubsystem.SetCurSel(0);
}

void CBASManagerDlg::updateControlSubsystemButtonsStatus()
{
	POSITION pos = m_lstSubsytems.GetFirstSelectedItemPosition();

	bool bEnableBtn = true;
	bEnableBtn &= (isActionPermitted(TA_Base_Bus::aca_SET_MODE_ECS));
	bEnableBtn &= (NULL != pos);

	m_btnAutoManual.EnableWindow(bEnableBtn);
    m_btnRevokeMode.EnableWindow(bEnableBtn);
}

void CBASManagerDlg::updateControlModeButtonsStatus()
{
	POSITION pos = m_lstModes.GetFirstSelectedItemPosition();

	//update device list button status
	bool bEnableDeviceBtn = true;
	bEnableDeviceBtn &= (NULL != pos);

	m_btnDeviceList.EnableWindow(bEnableDeviceBtn);

	//update mode execute button status
	bool bEnableExecuteBtn = true;
	bEnableExecuteBtn &= isActionPermitted(TA_Base_Bus::aca_SET_MODE_ECS);
	bEnableExecuteBtn &= (NULL != pos);
	int item = m_lstModes.GetNextSelectedItem(pos);
	CString strStatus = m_lstModes.GetItemText(item, ModesListCtrl::COLUMN_INDEX_MODE_STATUS);
	bool bEnableViaModeStatus = ((0 != strStatus.Compare(ModeRunningStatusConfigManager::getModeRunningStatusByType(eModeExecutionSuccess).c_str())) &&
        (0 != strStatus.Compare(ModeRunningStatusConfigManager::getModeRunningStatusByType(eModeExecuting).c_str())));
	bEnableExecuteBtn &= bEnableViaModeStatus;

	m_btnExecute.EnableWindow(bEnableExecuteBtn);
}

void CBASManagerDlg::processStationFireFromCommandLine()
{
    unsigned long loc = BasManagerDataHelperInstance::instance()->getFireLocationKey();
    unsigned long modeValue = BasManagerDataHelperInstance::instance()->getManagerModeValue();
    receiveStationFireMessage(loc, modeValue, TA_IRS_Core::FireStatus_Fire);
}

Namespace_TA_IRS_APP_End
