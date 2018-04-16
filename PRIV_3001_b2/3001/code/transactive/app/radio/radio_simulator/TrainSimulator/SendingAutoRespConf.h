// TrainSimulatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "afxwin.h"

#include "IView.h"
#include "IController.h"
#include "GlobalDataTypes.h"

#include <fstream>
#include <afx.h>

#include "CTrainStatusConfig.h"
#include "AutoSendDataModel.h"

using namespace TA_IRS_App;

class CSendingAutoRespConf : public CDialog, public IView
{
// Construction
	public:
		CSendingAutoRespConf(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
		enum { IDD = IDD_DIALOG_AUTOSENDCONF };
	
	// controls
	private:
		CButton m_chkPecAnswer;
		CButton m_chkPecHold;
		CButton m_chkPecReset;

		CButton m_chkPaLive;
		CButton m_chkPaPreRecorded;
		CButton m_chkPaLiveReset;
		CButton m_chkPaPreRecordedReset;
		CButton m_chkPaContinue;
		CButton m_chkPaPreRecEmergency;

		CButton m_btnSendStateChange;
		CButton m_btnSendStateChangeTkeCtrl;
		
		CButton m_chkTtisLibDownload;
		CButton m_chkPreRecordedCompl;
		
		CButton m_chkChangeOverCtrl;
		CButton m_chkTtisChangeOver;
		CButton m_chkPecChangeOver;
		CButton m_chkPaLiveChangeOver;
		CButton m_chkPaPreRecChangeOver;
		CButton m_chkTtisUpdateChangeOver;
		CButton m_chkTtisCmdRecvFailed;
		CComboBox m_cmboPaLiveAutoSendOption;
	
	// mvc objects
		IController * m_controller;
		

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	protected:
		HICON m_hIcon;

		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		
		DECLARE_MESSAGE_MAP()

	// events
	public:
		void OnCheckPECBoxTick ();
		void OnCheckPABoxTick ();
		void OnCheckTTISBoxTick ();
		void OnBtnSendState ();
		void OnBtnSendStateTkeCtrl ();
		void OnBtnChangeOverStatusTick ();
		void OnCbnSelchangeComboPALiveTrainCount ();
	
	// view methods
		virtual void addListener(IController* controller);
		virtual void updateUI(MessageInfo);
	
	// data model helper
	AutoSendingPecConfInfo createAutoSendingPECData ();
	AutoSendingPAConfInfo createAutoSendingPAData ();
	AutoSendingTTISConfInfo createAutoSendingTtisData ();
	
};
