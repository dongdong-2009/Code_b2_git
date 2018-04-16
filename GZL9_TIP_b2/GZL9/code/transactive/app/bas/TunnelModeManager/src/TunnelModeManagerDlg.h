#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeManagerDlg)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeManagerDlg
#if _MSC_VER > 1000
#pragma once
#endif
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

namespace TA_IRS_App
{
	// CTunnelModeManagerDlg dialog
	class CTunnelModeBaseDialog;
	class CTunnelModeSelectDlg;
	class CTunnelModeManagerDlg : public TA_Base_Bus::TransActiveDialog
	{
	// Construction
	public:
		friend CTunnelModeSelectDlg;
		CTunnelModeManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
		enum { IDD = IDD_TUNNELMODEMANAGER_MAIN_PANNEL };

		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
	protected:
		HICON m_hIcon;

		// Generated message map functions
		virtual BOOL OnInitDialog();
		//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg virtual void OnClose();
		DECLARE_MESSAGE_MAP()

	protected:
		void InitializeImplement();
		void ShowImplementDialog();
	private:
		CTunnelModeBaseDialog* m_implementDialog;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeManagerDlg