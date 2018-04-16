// VideoDisplayManagerDlg.cpp : implementation file
//

#include "stdafx.h"

#include "core/data_access_interface/entity_access/src/iconsole.h"
#include "core/data_access_interface/entity_access/src/consoleaccessfactory.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "VideoDisplayManagerDlg.h"

//#include "TrainBorne_DisplayManager/SwitchAgentCommunicator.h"
#include "DisplayItem/DisplayItemManager.h"
#include "VideoDisplayPresenter.h"
#include "DlgAlarmEventTest.h"

// VideoDisplayManagerDlg dialog
namespace
{
	bool isInStageOne()
	{
		TA_Base_Core::IGlobalParameter* stageParameter = NULL;
		try
		{
			stageParameter =
				TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem("VssStageMode", 25);
		}
		catch(const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", e.what());
		}
		catch(const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
		}
		return (stageParameter != NULL && stageParameter->getValue() == "1");
	}
}

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(VideoDisplayManagerDlg, ITrainBorne)

    VideoDisplayManagerDlg::VideoDisplayManagerDlg(CWnd* pParent /*=NULL*/)
        ://m_locationKey(atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str()))
        ITrainBorne(VideoDisplayManagerDlg::IDD, pParent),
        m_presenter(NULL),
        m_bMsgFromParent(false),
        m_dlgAlarmEventTest(NULL)
    {
        TA_Base_Core::IConsole* console;
        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
        UINT dlgId = 0;
        m_locationname = console->getLocationName();
        if(m_locationname == LOCATION_NAME_OCC)
        {
			dlgId = isInStageOne() ? IDD_VIDEODISPLAYMANAGER_DIALOG_KOCC : IDD_VIDEODISPLAYMANAGER_DIALOG_OCC;
        }
        else if(m_locationname == LOCATION_NAME_DPT_1)
        {
            dlgId = IDD_VIDEODISPLAYMANAGER_DIALOG_DPT;
        }
        else if(m_locationname == LOCATION_NAME_DPT_2)
        {
            dlgId = IDD_VIDEODISPLAYMANAGER_DIALOG_DPT;
        }
        else if(m_locationname == LOCATION_NAME_DPT_3)
        {
            dlgId = IDD_VIDEODISPLAYMANAGER_DIALOG_DPT;
        }
        else
        {
            dlgId = IDD_VIDEODISPLAYMANAGER_DIALOG_PSC;
        }
        TB_SetTemplateID(dlgId);
    }

    VideoDisplayManagerDlg::~VideoDisplayManagerDlg()
    {
    }

    void VideoDisplayManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //  DDX_Control(pDX, IDC_VD_LT_MONITOR, m_LtMonitor);
        //  DDX_Radio(pDX, IDC_VD_RA_ASSIGN, m_Operation);
    }

    void VideoDisplayManagerDlg::TB_TrainSelected(unsigned long trainID)
    {

    }

    bool VideoDisplayManagerDlg::TB_DoApply()
    {
        return true;
    }

    BOOL VideoDisplayManagerDlg::OnInitDialog()
    {
        __super::OnInitDialog();

        // TODO:  Add extra initialization here
        UINT msgCtrlID = 0;
		UINT dlgID = TB_GetTemplateID();
		if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_OCC)
        {
            msgCtrlID = IDC_VD_OCC_LT_DETAILS;
            m_groupCtrlID = IDC_VD_ST_GROUP_OCC;
        }
		else if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_KOCC)
        {
            msgCtrlID = IDC_VD_KOCC_LT_DETAILS;
            m_groupCtrlID = IDC_VD_ST_GROUP_KOCC;
        }
		else if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_DPT)
        {
            msgCtrlID = IDC_VD_DPT_LT_DETAILS;
            m_groupCtrlID = IDC_VD_ST_GROUP_DPT;
        }
        else
        {
            msgCtrlID = IDC_VD_PSC_LT_DETAILS;
            m_groupCtrlID = IDC_VD_ST_GROUP_PSC;
        }
        m_presenter = new VideoDisplayPresenter(this, m_parentGUI, m_parentSelector, msgCtrlID);
        if(TA_Base_Core::RunParams::getInstance().isSet("TestAlarmEvent"))
        {
            m_dlgAlarmEventTest = new DlgAlarmEventTest(m_presenter);
            m_dlgAlarmEventTest->Create(IDD_VIDEODISPLAYMANAGER_ALARMEVENT_TEST);
            m_dlgAlarmEventTest->ShowWindow(SW_SHOW);
        }
        UpdateData(FALSE);

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    LRESULT VideoDisplayManagerDlg::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
    {
        setupMonitors();
        SetTimer(1, 1000, NULL);
        return 0l;
    }

    LRESULT VideoDisplayManagerDlg::OnTVSSCommand(WPARAM wParam, LPARAM lParam)
    {
        m_presenter->OnTVSSCommand(wParam);
        return 0l;
    }

    void VideoDisplayManagerDlg::setupMonitors()
    {
        MonitorScreen* screen = m_presenter->getMonitorScreen();
        if(NULL == screen)
            return;
        UINT screenID, screenTextID, index = 1;
        CString strScreenName;
		UINT dlgID = TB_GetTemplateID();
		if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_OCC)
        {
            for(screenID = IDC_VD_OCC_WALL_MON_1, screenTextID = IDC_VD_OCC_WALL_MON_TEXT_1; screenID <= IDC_VD_OCC_WALL_MON_18; screenID++, screenTextID++, index++)
            {
                strScreenName.Format("WallMonitor%d", screenID);
                std::string name(strScreenName);
                screen->insertScreen(name, screenID, screenTextID, index, ScreenWall);
            }
            screen->insertScreen("ConsoleMonitor1", IDC_VD_OCC_CONSOLE_MON_1, IDC_VD_OCC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            screen->insertScreen("OverAllMonitor1", IDC_VD_OCC_OVERALL_MON_1, IDC_VD_OCC_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
		else if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_KOCC)
        {
            for(screenID = IDC_VD_KOCC_WALL_MON_1, screenTextID = IDC_VD_KOCC_WALL_MON_TEXT_1; screenID <= IDC_VD_KOCC_WALL_MON_4; screenID++, screenTextID++, index++)
            {
                strScreenName.Format("WallMonitor%d", screenID);
                std::string name(strScreenName);
                screen->insertScreen(name, screenID, screenTextID, index, ScreenWall);
            }
            screen->insertScreen("ConsoleMonitor1", IDC_VD_KOCC_CONSOLE_MON_1, IDC_VD_KOCC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            screen->insertScreen("OverAllMonitor1", IDC_VD_KOCC_OVERALL_MON_1, IDC_VD_KOCC_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
		else if(dlgID == IDD_VIDEODISPLAYMANAGER_DIALOG_DPT)
        {
            screen->insertScreen("ConsoleMonitor1", IDC_VD_DPT_CONSOLE_MON_1, IDC_VD_DPT_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            screen->insertScreen("ConsoleMonitor2", IDC_VD_DPT_CONSOLE_MON_2, IDC_VD_DPT_CONSOLE_MON_TEXT_2, 2, ScreenConsole);
            screen->insertScreen("OverAllMonitor1", IDC_VD_DPT_OVERALL_MON_1, IDC_VD_DPT_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
        else
        {
            screen->insertScreen("ConsoleMonitor1", IDC_VD_PSC_CONSOLE_MON_1, IDC_VD_PSC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            screen->insertScreen("ConsoleMonitor2", IDC_VD_PSC_CONSOLE_MON_2, IDC_VD_PSC_CONSOLE_MON_TEXT_2, 2, ScreenConsole);
            screen->insertScreen("ConsoleMonitor3", IDC_VD_PSC_CONSOLE_MON_3, IDC_VD_PSC_CONSOLE_MON_TEXT_3, 3, ScreenConsole);
        }
        m_presenter->SetupMonitorScreen();

    }

    HBRUSH VideoDisplayManagerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
    {
        HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

        // TODO:  Change any attributes of the DC here

        // TODO:  Return a different brush if the default is not desired
        if(nCtlColor == CTLCOLOR_STATIC && (NULL == pWnd || (NULL != pWnd && pWnd->GetDlgCtrlID() != m_groupCtrlID)))
        {
            pDC->SetBkMode(TRANSPARENT);
            return (HBRUSH)GetStockObject(NULL_BRUSH);
        }
        return hbr;
    }

    BEGIN_MESSAGE_MAP(VideoDisplayManagerDlg, CDialog)
        ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
        ON_MESSAGE(WM_TVSS_COMMAND, OnTVSSCommand)
        ON_WM_TIMER()
        ON_WM_CLOSE()
        ON_WM_CTLCOLOR()
        ON_WM_MOUSEMOVE()
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_RBUTTONDOWN()
        ON_WM_RBUTTONUP()
    END_MESSAGE_MAP()

    void VideoDisplayManagerDlg::OnDestory()
    {
        if(NULL != m_presenter)
        {
            delete m_presenter;
            m_presenter = NULL;
        }
        KillTimer(1);
        CDialog::OnDestroy();
    }

    void VideoDisplayManagerDlg::TB_PageActived(bool bSelected)
    {
    }

    void VideoDisplayManagerDlg::TB_MouseAction(UINT action, UINT flag, CPoint pt)
    {
        m_bMsgFromParent = true;
        switch(action)
        {
            case WM_MOUSEMOVE:
                OnMouseMove(flag, pt);
                break;
            case WM_LBUTTONUP:
                OnLButtonUp(flag, pt);
                break;
            default:
                break;
        }
        m_bMsgFromParent = false;
    }

    void VideoDisplayManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
    {
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if(!m_bMsgFromParent)
            ClientToScreen(&point);
        m_presenter->OnMouseMove(nFlags, point, m_bMsgFromParent);
        CDialog::OnMouseMove(nFlags, point);
    }

    void VideoDisplayManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
    {
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if(!m_bMsgFromParent)
            ClientToScreen(&point);
        m_presenter->OnLButtonDown(nFlags, point, m_bMsgFromParent);
        CDialog::OnLButtonDown(nFlags, point);
    }

    void VideoDisplayManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
    {
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if(!m_bMsgFromParent)
            ClientToScreen(&point);
        m_presenter->OnLButtonUp(nFlags, point, m_bMsgFromParent);
        CDialog::OnLButtonUp(nFlags, point);
    }

    void VideoDisplayManagerDlg::OnRButtonDown(UINT nFlags, CPoint point)
    {
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if(!m_bMsgFromParent)
            ClientToScreen(&point);
        m_presenter->OnRButtonDown(nFlags, point, m_bMsgFromParent);
        CDialog::OnRButtonDown(nFlags, point);
    }

    void VideoDisplayManagerDlg::OnRButtonUp(UINT nFlags, CPoint point)
    {
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if(!m_bMsgFromParent)
            ClientToScreen(&point);
        m_presenter->OnRButtonUp(nFlags, point, m_bMsgFromParent);
        CDialog::OnRButtonUp(nFlags, point);
    }

    void VideoDisplayManagerDlg::OnTimer(UINT_PTR nIDEvent)
    {
        // TODO: Add your message handler code here and/or call default
        switch(nIDEvent)
        {
            case 1:
                {
                    MonitorScreen* screen = m_presenter->getMonitorScreen();
                    if(NULL == screen)
                        return;
                    screen->drawAll();
                }
                break;
            default:
                break;
        }

        CDialog::OnTimer(nIDEvent);
    }

}
// VideoDisplayManagerDlg message handlers
