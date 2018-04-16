#pragma once
#include "DisplayRequestStatusDialog.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
namespace TA_IRS_App
{
    class PredefinedMsgDisplayRequestDlg: public DisplayRequestStatusDialog
    {
    public:
        PredefinedMsgDisplayRequestDlg( std::string dialogName = "Predefined Message Display Request Status",
                                        UINT nDialogId = IDD );

        enum { IDD = IDD_PREDEFINED_MESSAGE_DISPLAY_REQUEST_DIALOG };
        virtual ~PredefinedMsgDisplayRequestDlg( void );

        void setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                TA_Base_Core::PredefinedMessage& message,
                                time_t timestamp,
                                const std::string& sessionId,
                                TA_Base_Core::TisCommandType originalCommand,
                                unsigned long maximumRequestTime );

    protected:
        //virtual BOOL OnInitDialog();
        virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
        virtual void updateViewArea();
        virtual BOOL OnInitDialog();
        virtual void showDialog();
        virtual void clearViewArea();
        DECLARE_MESSAGE_MAP()

    private:
        CEdit m_messageDescription;
        CButton m_continuousBtn;
        CButton m_perrunBtn;
        CEdit m_perrun;
        TA_Base_Core::PredefinedMessage m_currentPredefinedMessage;
    };
}

