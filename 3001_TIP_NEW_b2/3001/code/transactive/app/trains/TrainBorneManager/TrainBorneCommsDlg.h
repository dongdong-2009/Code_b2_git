// TrainBorneManagerDlg.h : header file
//

#ifndef _TRAIN_BORN_COMMS_DLG_
#define _TRAIN_BORN_COMMS_DLG_

#define TRAIN_INBOUND_MSG				(WM_APP+1)
#define TRAIN_OUTBOUND_MSG				(WM_APP+2)
#define TRAIN_UPDATE_SELECTION_MSG      (WM_APP+3)


#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/radio/radiosearchpage/src/isearchpagehelper.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "TrainBorneTabManager.h"
#include "TrainListRecoveryThread.h"
//#include "VideoSourcesListCtrl.h"

// CTrainBorneTestDlg dialog
namespace TA_IRS_App
{
	class TrainDetailTest;
	class TBTrainSelectorGUI;
    class TrainDvaVersionsPage;
	class CTrainBorneCommsDlg :  public CDialog
	{
		// Construction
	public:
		CTrainBorneCommsDlg(CWnd* pParent);	// standard constructor
		~CTrainBorneCommsDlg();

		// Dialog Data
		enum { IDD = IDD_TRAINBORNEMANAGER_COMMS_DIALOG };
		TrainBorneTabManager*  GetTrainBorneManager() {return &m_TrainBorneTabManager;}
        void setTrainDvaVersionPage( TrainDvaVersionsPage* pPage );

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

		CTabCtrl m_TbTrainBorne;
		CButton    m_allOutboundButton;
		CButton    m_allInboundButton;
		CButton    m_clearTrainSelectionButton;
		CButton    m_selectAllTrainsButton;

		afx_msg LRESULT OnUpdateInboundList(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnUpdateOutboundList(WPARAM wParam, LPARAM lParam);

	public:
		afx_msg void OnDestroy();
		afx_msg void OnLbnSelchangeTrainList();
		afx_msg void OnBnClickedBuApply();
		afx_msg void OnBnClickedBuHelp();
		afx_msg void OnBnClickedBuClose();
		afx_msg void OnTcnSelchangeTbTrainBorne(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnButtonalloutbound();
		afx_msg void OnButtonallinbound();
		afx_msg void OnButtonalltrains();
		afx_msg void OnButtonclearall();
		afx_msg void OnClickStnInboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnClickStnOutboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult);
	private:	
		afx_msg void OnLvnBegindragInboundList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnLvnBegindragOutboundList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg LRESULT OnTbPrepared( WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTbWillClosed( WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTbProcessStatusChanged( WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTbCanApply( WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTrainCtrlUpdate(WPARAM wParam,LPARAM lParam );
	private:
		TrainBorneTabManager m_TrainBorneTabManager; 
		void initialiseTrainSelector();

		void initUIDepotCtrl();

		TBTrainSelectorGUI* m_trainSelectorGUI;
		TA_IRS_Bus::TrainSelector*  m_trainSelector;

		TrainDetailTest* m_detailDlg;
		TA_IRS_Bus::TrainListCtrl* m_inboundTrainList;
		TA_IRS_Bus::TrainListCtrl* m_outboundTrainList;
		TA_IRS_Bus::TrainListCtrl& getInboundTrainList();
		TA_IRS_Bus::TrainListCtrl& getOutboundTrainList();
		TA_IRS_Bus::CommonTypes::TrainIdList m_selTrainIdList;
		//	VideoSourcesListCtrl    m_inboundTrainsListCtrl;
		//  VideoSourcesListCtrl    m_outboundTrainsListCtrl;
		CString m_strDragText;
		bool m_bIsDraging;
		CImageList* m_imgDrag;
        TrainDvaVersionsPage* m_pTrainDvaPage;
		//Update check and recovery the train list at an interval time
		TrainListRecoveryThread* m_trainListRecoveryThread;
		DECLARE_MESSAGE_MAP()
	};
}
#endif