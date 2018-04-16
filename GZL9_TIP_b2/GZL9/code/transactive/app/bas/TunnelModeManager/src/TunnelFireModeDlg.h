#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelFireModeDlg)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelFireModeDlg
#if _MSC_VER > 1000
#pragma once
#endif

#include "app/bas/TunnelModeManager/src/TunnelModeManagerActionThread.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "app/bas/TunnelModeManager/src/TunnelModeBaseDialog.h"
#include "app/bas/TunnelModeManager/src/FireModeListCtrl.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerActionThread.h"
namespace TA_IRS_App
{
	// CTunnelFireModeDlg dialog
	class CTunnelFireModeDlg : public CTunnelModeBaseDialog
							  ,public ActionThreadCallbackInterface
	{
		DECLARE_DYNAMIC(CTunnelFireModeDlg)

	public:
		CTunnelFireModeDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CTunnelFireModeDlg();

	// Dialog Data
		enum { IDD = IDD_DIALOG_FIRE_MODE };
	public:
		virtual bool canCloseWindow();
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize getDlgInitSize();
        virtual std::string CTunnelFireModeDlg::getApplicationCaption();

	public:
		// the action threadcall back interface implement
		void ActionThreadFunction(TunnelModeManagerAction action);
	
		void InitializeDlgData();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSearchButtonClick();
		afx_msg void OnButtonExecuteClick();
		afx_msg void OnDlgTrackNumberChanged();
		afx_msg void OnQuit();
		DECLARE_MESSAGE_MAP()
	protected:
		TA_IRS_Core::TunnelFireModeInfo_SmartPtr getSelectFireModeInfo(unsigned long pkey);
		void fillwithSectionAndLineTypeAndShaftPositionByTrackName();
		void setSelItemByData(CComboBox* comboBox, unsigned long data);
		void updateShaftRadioButton(TA_IRS_Core::TrackInfoVct& trackInfoVct);

		void removeDuplicateItems(TA_IRS_Core::TunnelFireModeInfoVct& fireModes);
		void processTrackNumberFromHMI();
	private:
		CFireModeListCtrl m_modeListCtrl;
		CComboBox m_sections;
		CComboBox m_lineType;
		CComboBox m_fireCase;
		CButton m_firePositionLeft;
		CButton	m_firePositionRight;
		CEdit m_trackNumber;

		TunnelModeManagerActionThread* m_InitializeThread;
		std::map<unsigned long, std::string> m_lineTypeMap;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelFireModeDlg
