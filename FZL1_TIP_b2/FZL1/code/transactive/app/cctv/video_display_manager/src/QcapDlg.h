#pragma once
// QcapDlg dialog

namespace TA_IRS_App
{
	class QcapDlg : public CDialog
	{
		DECLARE_DYNAMIC(QcapDlg)

	public:
		QcapDlg(UINT dlgId, CWnd* pParent = NULL);   // standard constructor
		virtual ~QcapDlg();
		virtual void stop();
		virtual void play();
		void setPlayWindowHandle(HWND playWnd);
        virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
		afx_msg void OnDestroy();
		afx_msg void OnClose();
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		DECLARE_MESSAGE_MAP()
	private:
		void initHw();
		void uninitHw();
		PVOID m_hwHandle;
		HWND m_playWndHandle;
	};
}
