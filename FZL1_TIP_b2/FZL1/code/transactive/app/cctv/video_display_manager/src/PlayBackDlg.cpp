#include "stdafx.h"
#include "app/cctv/video_display_manager/src/PlayBackDlg.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/VirtualMonitorAssigner.h"
#include "bus/cctv/video_switch_agent/idl/src/VideoOutputCorbaDef.h"
#include "bus/generic_gui/src/HelpLauncher.h"

// PlayBackDlg dialog
namespace TA_IRS_App
{
	IMPLEMENT_DYNAMIC(PlayBackDlg, QcapDlg)

	PlayBackDlg::PlayBackDlg(VirtualMonitorAssigner& virtualMonitorAssigner, CWnd* pParent /*=NULL*/)
		: QcapDlg(PlayBackDlg::IDD, pParent),
		  m_virtualMonitorAssigner(virtualMonitorAssigner)
	{
	}

	PlayBackDlg::~PlayBackDlg()
	{
	}

	void PlayBackDlg::DoDataExchange(CDataExchange* pDX)
	{
		QcapDlg::DoDataExchange(pDX);
		DDX_Control(pDX, ID_BTN_PLAYBACK_REWIND, m_btnRewind);
		DDX_Control(pDX, ID_BTN_PLAYBACK_PLAY, m_btnPlay);
		DDX_Control(pDX, ID_BTN_PLAYBACK_FORWARD, m_btnForward);
		DDX_Control(pDX, IDC_STATIC_PLAY, m_playBtnDescription);
	}


	BEGIN_MESSAGE_MAP(PlayBackDlg, QcapDlg)
		ON_WM_CLOSE()
		ON_WM_DESTROY()
		ON_BN_CLICKED(IDOK, &PlayBackDlg::OnBnClickedOk)
		ON_BN_CLICKED(IDCANCEL, &PlayBackDlg::OnBnClickedCancel)
		ON_BN_CLICKED(ID_BTN_PLAYBACK_HELP, &PlayBackDlg::OnBnClickedBtnPlaybackHelp)
		ON_BN_CLICKED(ID_BTN_PLAYBACK_FORWARD, &PlayBackDlg::OnBnClickedBtnPlaybackForward)
		ON_BN_CLICKED(ID_BTN_PLAYBACK_PLAY, &PlayBackDlg::OnBnClickedBtnPlaybackPlay)
		ON_BN_CLICKED(ID_BTN_PLAYBACK_REWIND, &PlayBackDlg::OnBnClickedBtnPlaybackRewind)
	END_MESSAGE_MAP()

	void PlayBackDlg::OnClose()
	{
		QcapDlg::OnClose();
	}

	void PlayBackDlg::OnDestroy()
	{
		QcapDlg::OnDestroy();
	}

	BOOL PlayBackDlg::OnInitDialog()
	{
		QcapDlg::OnInitDialog();
		HINSTANCE resourceInstance = AfxGetApp()->m_hInstance;
		HBITMAP bitmap = ::LoadBitmap(resourceInstance, MAKEINTRESOURCE(IDB_REWIND_BITMAP));
		m_btnRewind.SetBitmap(bitmap);
		m_bitmapPlay = ::LoadBitmap(resourceInstance, MAKEINTRESOURCE(IDB_PLAY_BITMAP));
		m_bitmapPause = ::LoadBitmap(resourceInstance, MAKEINTRESOURCE(IDB_PAUSE_BITMAP));
		m_btnPlay.SetBitmap(m_bitmapPlay);
		bitmap = ::LoadBitmap(resourceInstance, MAKEINTRESOURCE(IDB_FORWARD_BITMAP));
		m_btnForward.SetBitmap(bitmap);
		setPlayWindowHandle(GetDlgItem(IDC_PLAYBACK_VIDEO_WINDOW)->GetSafeHwnd());
		return TRUE;
	}

	void PlayBackDlg::stop()
	{
		QcapDlg::stop();
		updatePlayBtnStatus(PLAY);
	}

	void PlayBackDlg::play()
	{
		updatePlayBtnStatus(PAUSE);
		QcapDlg::play();
	}
	// PlayBackDlg message handlers

	void PlayBackDlg::updatePlayBtnStatus(PlayBtnStatus status)
	{
		if(PLAY == status)
		{
			m_btnPlay.SetBitmap(m_bitmapPlay);
			m_playBtnDescription.SetWindowText("²¥·Å");
		}
		else
		{
			m_btnPlay.SetBitmap(m_bitmapPause);
			m_playBtnDescription.SetWindowText("ÔÝÍ£");
		}
	}

	void PlayBackDlg::OnBnClickedOk()
	{
		QcapDlg::OnClose();
	}

	void PlayBackDlg::OnBnClickedCancel()
	{
		// TODO: Add your control notification handler code here
		QcapDlg::OnClose();
	}

	void PlayBackDlg::OnBnClickedBtnPlaybackHelp()
	{
		// TODO: Add your control notification handler code here
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

	void PlayBackDlg::OnBnClickedBtnPlaybackForward()
	{
		// TODO: Add your control notification handler code here
		m_virtualMonitorAssigner.playBackCtrl(m_virtualMonitorAssigner.getLocalMonitor(), TA_Base_Bus::VideoOutputCorbaDef::FAST_FORWARD_1_STEP);
	}

	void PlayBackDlg::OnBnClickedBtnPlaybackPlay()
	{
		if(m_btnPlay.GetBitmap() == m_bitmapPlay)
		{
			m_virtualMonitorAssigner.playBackCtrl(m_virtualMonitorAssigner.getLocalMonitor(), TA_Base_Bus::VideoOutputCorbaDef::RESUME_PLAYBACK);
			updatePlayBtnStatus(PAUSE);
		}
		else
		{
			m_virtualMonitorAssigner.playBackCtrl(m_virtualMonitorAssigner.getLocalMonitor(), TA_Base_Bus::VideoOutputCorbaDef::PAUSE_PLAYBACK);
			updatePlayBtnStatus(PLAY);
		}
	}

	void PlayBackDlg::OnBnClickedBtnPlaybackRewind()
	{
		// TODO: Add your control notification handler code here
		m_virtualMonitorAssigner.playBackCtrl(m_virtualMonitorAssigner.getLocalMonitor(), TA_Base_Bus::VideoOutputCorbaDef::FAST_BACKWARD_1_STEP);
	}
}

