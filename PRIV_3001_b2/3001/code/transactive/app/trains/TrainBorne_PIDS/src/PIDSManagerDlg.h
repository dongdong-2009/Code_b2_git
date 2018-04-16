#pragma once

#include "resource.h"
#include "app/trains/TrainBorneManager/ITrainBorne.h"
#include "PIDSMessageTabCtrl.h"
#include "ValidatingEdit.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectionListener.h"
#include "IMessageSelectionListener.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "DisplayAttributeManager.h"
#include "PIDSTrainSelectorGUI.h"


namespace TA_IRS_App
{
    class DisplayRequestManager;
    class PIDSManagerDlg : public ITrainBorne,
        public TA_IRS_Bus::ITrainSelectionListener,
        public IMessageSelectionListener

    {
    public:
        PIDSManagerDlg( CWnd* pParent = NULL ); // standard constructor
        virtual ~PIDSManagerDlg();
        virtual void TB_TrainSelected( TA_IRS_Bus::CommonTypes::TrainIdList selList );

        //Called by framework
        virtual bool TB_DoApply();
        virtual BOOL OnInitDialog();

        virtual void trainSelectionChanged( bool trainsAreSelected );
        virtual void messageSelectionChanged( bool messageIsSelected );
        virtual DisplayAttributeManager* getDisplayAttributeManager();
        virtual void TB_PageActived( bool bActived );
        virtual void TB_CanApply( bool bCanApplyNow );

        enum { IDD = IDD_PIDSMANAGER_DIALOG };
    protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support

        afx_msg void OnClose();

		afx_msg void OnReset();
        afx_msg void OnTimeTypeContinuous();
        afx_msg void OnTimeTypePerrun();
        afx_msg void OnDestroy();

        DECLARE_MESSAGE_MAP()
    private:
        //All resource in dialog
        PIDSMessageTabCtrl m_messageTab;
        CButton m_timeTypePerrun;
        CButton m_timeTypeContinuous;
        ValidatingEdit m_perRun;
        CButton m_reset;
        CStatic m_timeTypePerrunLabel;
		CStatic m_timeTypeContinuousLabel;
    private:
        bool m_trainSelected;
        bool m_messageSelected;
        bool m_canSendFreeTextDisplayRequest;
        bool m_canSendPredefinedDisplayRequest;
        bool m_canSendClearRequest;

        DisplayRequestManager *m_displayRequestManager;
        DisplayAttributeManager *m_displayAttributeManager;
        unsigned long m_locationKey; //location key, used by other function
    };
}
