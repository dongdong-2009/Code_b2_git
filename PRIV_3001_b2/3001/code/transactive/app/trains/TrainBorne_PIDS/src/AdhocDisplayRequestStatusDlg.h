#pragma once
#include "DisplayRequestStatusDialog.h"
#include "PIDSAdhocMessagePage.h"
namespace TA_IRS_App
{
    class AdhocDisplayRequestStatusDlg: public DisplayRequestStatusDialog
    {
    public:
        AdhocDisplayRequestStatusDlg( std::string dialogName = "Adhoc Message Display Request Status",
                                      UINT nDialogId = IDD );

        enum { IDD = IDD_ADHOC_MESSAGE_DISPLAY_REQUEST_DIALOG };
        virtual ~AdhocDisplayRequestStatusDlg( void );

        void setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                PIDSAdhocMessagePage::AdHocMessage currentAdhocMessage,
                                const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                time_t timestamp,
                                const std::string& sessionId,
                                TA_Base_Core::TisCommandType originalCommand,
                                unsigned long maximumRequestTime );

    protected:
        //virtual BOOL OnInitDialog();
        virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
        virtual void clearViewArea();
        virtual void updateViewArea();
        virtual BOOL OnInitDialog();
        virtual void showDialog();
        DECLARE_MESSAGE_MAP()

    private:
        CEdit   m_messageContent;
        CEdit	m_displayMode;
        CEdit	m_fontSize;
        CEdit	m_justification;
        CEdit	m_intensity;
        CButton m_continuousBtn;
        CButton m_perrunBtn;
        CEdit   m_perrun;
        PIDSAdhocMessagePage::AdHocMessage m_currentAdhocMessage;
        TA_Base_Core::TTISFreeTextMessageParameters m_currentDisplayParameters;
    };
}

