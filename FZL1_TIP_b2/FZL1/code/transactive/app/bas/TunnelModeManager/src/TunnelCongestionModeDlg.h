#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelCongestionModeDlg)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelCongestionModeDlg
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/data_access_interface/bas/src/CommonDefine.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerActionThread.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "app/bas/TunnelModeManager/src/TunnelModeBaseDialog.h"
#include "app/bas/TunnelModeManager/src/CongestionModeListCtrl.h"

namespace TA_IRS_App
{
	// CTunnelCongestionModeDlg dialog
	class CTunnelCongestionModeDlg : public CTunnelModeBaseDialog
									, public ActionThreadCallbackInterface
	{
		DECLARE_DYNAMIC(CTunnelCongestionModeDlg)

	public:
		CTunnelCongestionModeDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CTunnelCongestionModeDlg();

	// Dialog Data 
		enum { IDD = IDD_DIALOG_CONGESTION_MODE };
	public:
		// the implement of ActionThreadCallbackInterface
		void ActionThreadFunction(TunnelModeManagerAction action);
		void initializeDlgData();
	public:
		virtual bool canCloseWindow();
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize getDlgInitSize();
        virtual std::string getApplicationCaption();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSearchButtonClick();
		afx_msg void OnExecuteModeClick();
		afx_msg void OnQuit();
		afx_msg void OnDlgTrackNumberChanged();
		DECLARE_MESSAGE_MAP()
	protected:
		TA_IRS_Core::CongestionModeInfo_SmartPtr getSelectCongestionModeInfo(unsigned long pkey);
		void fillwithSectionAndLineTypeByTrackName();
		void setSelItemByData(CComboBox* comboBox, unsigned long data);

		void removeDuplicateItems(TA_IRS_Core::CongestionModeInfoVct& congestionModes);
		void processTrackNumberFromHMI();
	private:
		CCongestionModeListCtrl m_modeListCtrl;
		CComboBox m_Sections;
		CComboBox m_LineType;
		CEdit m_trackNumber;
		
		std::map<unsigned long, std::string> m_lineTypeMap; 
		TunnelModeManagerActionThread* m_initializeThread;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelCongestionModeDlg
