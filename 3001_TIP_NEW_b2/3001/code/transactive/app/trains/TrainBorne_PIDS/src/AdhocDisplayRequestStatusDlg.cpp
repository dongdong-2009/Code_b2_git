#include "StdAfx.h"
#include "AdhocDisplayRequestStatusDlg.h"
#include <sstream>
#include "PIDSAdhocMessagePage.h"

namespace TA_IRS_App
{
    BEGIN_MESSAGE_MAP( AdhocDisplayRequestStatusDlg, DisplayRequestStatusDialog )

    END_MESSAGE_MAP()

    AdhocDisplayRequestStatusDlg::AdhocDisplayRequestStatusDlg( std::string dialogName, UINT nDialogId )
            : DisplayRequestStatusDialog( dialogName, nDialogId )
    {
    }

    AdhocDisplayRequestStatusDlg::~AdhocDisplayRequestStatusDlg( void )
    {
    }

    void AdhocDisplayRequestStatusDlg::DoDataExchange( CDataExchange* pDX )
    {
        DisplayRequestStatusDialog::DoDataExchange( pDX );
        DDX_Control( pDX, IDC_ADHOC_REQUEST_STATUS_CONTENT, m_messageContent );
        DDX_Control( pDX, IDC_ADHOC_REQUEST_STATUS_FONT, m_fontSize );
        DDX_Control( pDX, IDC_ADHOC_REQUEST_STATUS_JF, m_justification );
        DDX_Control( pDX, IDC_ADHOC_REQUEST_STATUS_IT, m_intensity );
        DDX_Control( pDX, IDC_ADHOC_REQUEST_STATUS_MODE, m_displayMode );
        DDX_Control( pDX, IDC_CONTINUOUS_BTN, m_continuousBtn );
        DDX_Control( pDX, IDC_PERRUN_BTN, m_perrunBtn );
        DDX_Control( pDX, IDC_EDIT_MESSAGE_PER_RUN, m_perrun );
    }

    BOOL AdhocDisplayRequestStatusDlg::OnInitDialog()
    {
        DisplayRequestStatusDialog::OnInitDialog();
        return TRUE;
    }

    void AdhocDisplayRequestStatusDlg::showDialog()
    {
        updateViewArea();
        DisplayRequestStatusDialog::showDialog();
    }

    void AdhocDisplayRequestStatusDlg::clearViewArea()
    {
        m_messageContent.SetWindowText( "" );
        m_continuousBtn.SetCheck( BST_UNCHECKED );
        m_perrunBtn.SetCheck( BST_UNCHECKED );
        m_perrun.SetWindowText( "" );

        m_displayMode.SetWindowText( "" );
        m_fontSize.SetWindowText( "" );
        m_justification.SetWindowText( "" );
        m_intensity.SetWindowText( "" );
    }

    void AdhocDisplayRequestStatusDlg::updateViewArea()
    {
        //first clear all
        clearViewArea();

        m_displayMode.SetWindowText( PIDSAdhocMessagePage::m_displayModeToNameMap[m_currentAdhocMessage.displayMode].c_str() );
        m_fontSize.SetWindowText( PIDSAdhocMessagePage::m_fontSizeToNameMap[m_currentAdhocMessage.fontSize].c_str() );
        m_justification.SetWindowText( PIDSAdhocMessagePage::m_justificationToNameMap[m_currentAdhocMessage.justification].c_str() );
        m_intensity.SetWindowText( PIDSAdhocMessagePage::m_intensityToNameMap[m_currentAdhocMessage.intensity].c_str() );
        m_messageContent.SetWindowText( m_currentAdhocMessage.messageContent.c_str() );

        if ( m_currentDisplayParameters.perRun == 0)
        {
            m_continuousBtn.SetCheck( BST_CHECKED );
        }
        else
        {
            m_perrunBtn.SetCheck( BST_CHECKED );
            std::ostringstream ss;
            ss << static_cast<unsigned int>( m_currentDisplayParameters.perRun );
            m_perrun.SetWindowText( ss.str().c_str() );
        }
    }

    void AdhocDisplayRequestStatusDlg::setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                                          PIDSAdhocMessagePage::AdHocMessage currentAdhocMessage,
                                                          const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                          time_t timestamp,
                                                          const std::string& sessionId,
                                                          TA_Base_Core::TisCommandType originalCommand,
                                                          unsigned long maximumRequestTime )
    {
        m_currentAdhocMessage = currentAdhocMessage;
        m_currentDisplayParameters = displayParameters;
        DisplayRequestStatusDialog::setDisplayRequest( trainDetailsList, timestamp, sessionId, originalCommand, maximumRequestTime );
    }

}

