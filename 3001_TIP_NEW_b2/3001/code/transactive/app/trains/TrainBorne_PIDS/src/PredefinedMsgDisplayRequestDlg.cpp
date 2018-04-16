#include "StdAfx.h"
#include "PredefinedMsgDisplayRequestDlg.h"
#include <sstream>
namespace TA_IRS_App
{
    BEGIN_MESSAGE_MAP( PredefinedMsgDisplayRequestDlg, DisplayRequestStatusDialog )

    END_MESSAGE_MAP()

    PredefinedMsgDisplayRequestDlg::PredefinedMsgDisplayRequestDlg( std::string dialogName, UINT nDialogId )
            : DisplayRequestStatusDialog( dialogName, nDialogId )
    {

    }

    PredefinedMsgDisplayRequestDlg::~PredefinedMsgDisplayRequestDlg( void )
    {
    }

    BOOL PredefinedMsgDisplayRequestDlg::OnInitDialog()
    {
        DisplayRequestStatusDialog::OnInitDialog();
        return TRUE;
    }

    void PredefinedMsgDisplayRequestDlg::showDialog()
    {
        updateViewArea();
        DisplayRequestStatusDialog::showDialog();
    }

    void PredefinedMsgDisplayRequestDlg::DoDataExchange( CDataExchange* pDX )
    {
        DisplayRequestStatusDialog::DoDataExchange( pDX );
        DDX_Control( pDX, IDC_EDIT_MESSAGE_DESCRIPTION, m_messageDescription );
        DDX_Control( pDX, IDC_CONTINUOUS_BTN, m_continuousBtn );
        DDX_Control( pDX, IDC_PERRUN_BTN, m_perrunBtn );
        DDX_Control( pDX, IDC_EDIT_MESSAGE_PER_RUN, m_perrun );
    }

    void PredefinedMsgDisplayRequestDlg::clearViewArea()
    {
        m_messageDescription.SetWindowText( "" );
        m_continuousBtn.SetCheck( BST_UNCHECKED );
        m_perrunBtn.SetCheck( BST_UNCHECKED );
        m_perrun.SetWindowText( "" );
    }

    void PredefinedMsgDisplayRequestDlg::updateViewArea()
    {
        //first clear all
        clearViewArea();


        m_messageDescription.SetWindowText( m_currentPredefinedMessage.description.c_str() );
        if ( m_currentPredefinedMessage.perRun == 0 )
        {
            m_continuousBtn.SetCheck( BST_CHECKED );
        }
        else
        {
            m_perrunBtn.SetCheck( BST_CHECKED );
            std::ostringstream ss;
            ss << static_cast<unsigned short>(m_currentPredefinedMessage.perRun);
            m_perrun.SetWindowText( ss.str().c_str() );
        }
    }

    void PredefinedMsgDisplayRequestDlg::setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                                            TA_Base_Core::PredefinedMessage& message,
                                                            time_t timestamp,
                                                            const std::string& sessionId,
                                                            TA_Base_Core::TisCommandType originalCommand,
                                                            unsigned long maximumRequestTime )
    {
        m_currentPredefinedMessage = message;
        DisplayRequestStatusDialog::setDisplayRequest( trainDetailsList, timestamp, sessionId, originalCommand, maximumRequestTime );
    }

}

