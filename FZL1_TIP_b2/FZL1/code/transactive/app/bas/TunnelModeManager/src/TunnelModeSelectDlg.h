#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeSelectDlg)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeSelectDlg
#if _MSC_VER > 1000
#pragma once
#endif
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "app/bas/TunnelModeManager/src/TunnelModeBaseDialog.h"
#include "bus/application_types/src/apptypes.h"
namespace TA_IRS_App
{
	// CTunnelModeSelectDlg dialog
	enum TunnelModeType{ TunnelMode_Fire, TunnelMode_Congestion };
	class CTunnelModeSelectDlg : public CTunnelModeBaseDialog
	{
		DECLARE_DYNAMIC(CTunnelModeSelectDlg)

	public:
		CTunnelModeSelectDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CTunnelModeSelectDlg();
	// Dialog Data
		enum { IDD = IDD_DIALOG_TUNNEL_SELECT };
	public:
		virtual bool canCloseWindow();
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize getDlgInitSize();
        virtual std::string getApplicationCaption();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnButtonFireModeClick();
		afx_msg void OnButtonCongestionModeClick();
		DECLARE_MESSAGE_MAP()
	private:
		bool launchSpecialApplication(TunnelModeType applicationType);
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeSelectDlg