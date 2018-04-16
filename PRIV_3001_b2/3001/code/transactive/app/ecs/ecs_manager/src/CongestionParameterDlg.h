/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/CongestionParameterDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_CONGESTIONPARAMETERDLG_H__50229F95_7434_41CF_A857_7628F8D44E94__INCLUDED_)
#define AFX_CONGESTIONPARAMETERDLG_H__50229F95_7434_41CF_A857_7628F8D44E94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/AuditMessageUtility.h"
//TD18095, jianghp, to fix the performance of showing manager application
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/message/types/ECSAudit_MessageTypes.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    class IConfigGlobalParameter;
}

namespace TA_IRS_App
{
	class MappedECSDataCache;

	class CCongestionParameterDlg : public TA_Base_Bus::TransActiveDialog, 
		                            public TA_Base_Core::RunParamUser,
									public virtual IBaseView
	{
	
	public:
		CCongestionParameterDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL);   // standard constructor
		
		/**
		 *	onRunParamChange()
		 *
		 *	Callback which notifies us of a change in a run parameter's value
		 *	See TA_Base_Core::RunParamUser
		 *
		 */
		virtual void onRunParamChange(const std::string& name, const std::string& value);

		virtual void init();

	private:

        
        TA_Base_Core::IConfigGlobalParameter* m_thresholdStallTimeInSecs;
        TA_Base_Core::IConfigGlobalParameter* m_thresholdTemperature;
        TA_Base_Core::IConfigGlobalParameter* m_temperatureDeadband;
        TA_Base_Core::IConfigGlobalParameter* m_congestionUsesStallTime;
        TA_Base_Core::IConfigGlobalParameter* m_congestionUsesTemperature;

		struct CONGESTION_PARAMS
		{
			bool bUsesStallTime;
			bool bUsesTrainTemp;
			int	 nStallTime;
			int  nTrainTemp;
            int  nTempDeadBand;
		};

		CONGESTION_PARAMS m_inParams;
		CONGESTION_PARAMS m_outParams;

		std::auto_ptr<MappedECSDataCache> m_pMappedECSDataCache; 

		//{{AFX_DATA(CCongestionParameterDlg)
	enum { IDD = IDD_CONGESTION_PARAMS_DIALOG };
	CButton	m_ApplyButton;
		CButton	m_TrainTemp_Check;
		CSliderCtrl	m_Temp_Slider;
        CSliderCtrl	m_Temp_DeadBand_Slider;
		CEdit	m_StatusEdit;
		CButton	m_StallTime_Check;
		CStatic	m_TempLabel;
        CStatic	m_DeadBandLabel;
		CSpinButtonCtrl	m_SecondSpin;
		CSpinButtonCtrl	m_MinuteSpin;
		CEdit	m_SecondEdit;
		CEdit	m_MinuteEdit;
	//}}AFX_DATA


	
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCongestionParameterDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	
	protected:
		HICON m_hIcon;

		// Generated message map functions
		//{{AFX_MSG(CCongestionParameterDlg)
		afx_msg void OnClose();
		virtual BOOL OnInitDialog();
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);		
		afx_msg void OnStallTimeCheck();
		afx_msg void OnTrainTempCheck();
		afx_msg void OnAppAbout();
		afx_msg void OnAppHelp();
		afx_msg void OnApply();
		afx_msg void OnChangeMinute();
		afx_msg void OnChangeSecond();
		afx_msg LRESULT onReadCongestionParameters(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onCheckPermissions(WPARAM wParam, LPARAM lParam);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnHelpButton(); //TD16730
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	private:
		void CheckPermissions();
		bool m_isAllowedToSetParameter;
		bool m_hasChanged;
		bool m_isInitialised;
		void SaveCongestionParameters();
		void WriteStatusLine(const CString &lineOfText);
		void EnableTempRelatedGUI(bool enable);
		void EnableStallTimeRelatedGUI(bool enable);		
        
        void cleanupGlobalParamters();
        void initialiseGlobalParameters();
	};

} // namespace

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONGESTIONPARAMETERDLG_H__50229F95_7434_41CF_A857_7628F8D44E94__INCLUDED_)
