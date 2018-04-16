// PIDSMessageTab.cpp : implementation file
//

#include "stdafx.h"
#include "PIDSMessageTabCtrl.h"
#include "core/utilities/src/DebugUtil.h"


// PIDSMessageTab
namespace TA_IRS_App
{
    PIDSMessageTabCtrl::PIDSMessageTabCtrl()
    {
        FUNCTION_ENTRY( "PIDSMessageTabCtrl" );

        AddPage( m_predefPage, PIDSPredefinedMessagePage::IDD, CString( "Predefined" ) );
        AddPage( m_adhocPage, PIDSAdhocMessagePage::IDD, CString( "Adhoc" ) );

        FUNCTION_EXIT
    }

    void PIDSMessageTabCtrl::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener )
    {
        m_predefPage.setMessageSelectionListener( messageSelectionListener );
        m_adhocPage.setMessageSelectionListener( messageSelectionListener );
    }

    void PIDSMessageTabCtrl::Init()
    {
        FUNCTION_ENTRY( "Init" );

        m_predefPage.Init();
        m_adhocPage.Init();

        FUNCTION_EXIT
    }

    PIDSMessageTabCtrl::~PIDSMessageTabCtrl()
    {
        FUNCTION_ENTRY( "~PIDSMessageTabCtrl" );
        FUNCTION_EXIT

    }

    PIDSPredefinedMessagePage& PIDSMessageTabCtrl::getPredefinedPage()
    {
        FUNCTION_ENTRY( "getPredefinedPage" );

        return m_predefPage;

        FUNCTION_EXIT
    }

    PIDSAdhocMessagePage& PIDSMessageTabCtrl::getAdhocPage()
    {
        FUNCTION_ENTRY( "getAdhocPage" );

        return m_adhocPage;

        FUNCTION_EXIT
    }

    BOOL PIDSMessageTabCtrl::OnSelchange( NMHDR* pNMHDR, LRESULT* pResult )
    {
        FUNCTION_ENTRY( "OnSelchange" );

        BOOL res = CTisDialogCtrl::OnSelchange( pNMHDR, pResult );

        CDialog* newPage = getActiveDialog();

        if ( newPage == &m_predefPage )
        {
            m_predefPage.windowShown();
        }
        else
        {
            m_adhocPage.windowShown();
        }

        FUNCTION_EXIT;
        return res;
    }

    BEGIN_MESSAGE_MAP( PIDSMessageTabCtrl, CTisDialogCtrl)
        ON_NOTIFY_REFLECT_EX( TCN_SELCHANGE, OnSelchange )
    END_MESSAGE_MAP()
    // PIDSMessageTab message handlers
}








