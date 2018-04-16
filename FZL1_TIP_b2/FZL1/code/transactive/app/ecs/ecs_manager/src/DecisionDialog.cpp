/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/DecisionDialog.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Derivation of CDialog with the purpose of displaying listbox of possible actions for user
  * to choose from. The users choice is returned by index from DoModal. -1 is used to indicate
  * user has opted to cancel/choose none.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ecsmanager.h"
#include "app/ecs/ecs_manager/src/DecisionDialog.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    //
    // Constructor
    //
    DecisionDialog::DecisionDialog( CWnd* pParent, const std::string& decisionMessage, 
            const std::vector< std::string >& choices )
            : CDialog( DecisionDialog::IDD, pParent ), m_decisionStr( decisionMessage ),
            m_choices( choices )
    {
        FUNCTION_ENTRY( "Constructor" );

        //{{AFX_DATA_INIT(DecisionDialog)
        //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    //
    // DoDataExchange
    //
    void DecisionDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(DecisionDialog)
        DDX_Control(pDX, IDC_DECISIONMESSAGE, m_decisionMessageStatic);
        DDX_Control(pDX, IDC_DECISIONICON, m_decisionIconStatic);
        DDX_Control(pDX, IDOK, m_oKButton);
        DDX_Control(pDX, IDC_DECISIONLIST, m_decisionList);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(DecisionDialog, CDialog)
        //{{AFX_MSG_MAP(DecisionDialog)
        ON_BN_CLICKED(IDCANCEL, onCancel)
        ON_BN_CLICKED(IDOK, onOK)
        ON_LBN_SELCHANGE(IDC_DECISIONLIST, onDecisionListSelectionChange)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // DecisionDialog message handlers

    //
    // onCancel
    //
    void DecisionDialog::onCancel() 
    {
        FUNCTION_ENTRY( "onCancel" );

        EndDialog( -1 ); // no decision made.

        FUNCTION_EXIT;
    }


    //
    // onOK
    //
    void DecisionDialog::onOK() 
    {
        FUNCTION_ENTRY( "onOK" );

        if ( LB_ERR != m_decisionList.GetCurSel() )
        {
            EndDialog( m_decisionList.GetCurSel() );
        }
        else
        {
            // Should never get here as OK button is not enabled when nothing selected.
            // Anyhow, for completeness.
            EndDialog( -1 );
        }
        
        FUNCTION_EXIT;
    }


    //
    // onDecisionListSelectionChange
    //
    void DecisionDialog::onDecisionListSelectionChange() 
    {
        FUNCTION_ENTRY( "onDecisionLisSelectionChange" );

        m_oKButton.EnableWindow( LB_ERR != m_decisionList.GetCurSel() );    

        FUNCTION_EXIT;
    }


    //
    // onInitDialog
    //
    BOOL DecisionDialog::OnInitDialog() 
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();
        
        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
         // disable the X (top right window close button)
          pSysMenu->EnableMenuItem (SC_CLOSE, MF_BYCOMMAND|MF_DISABLED);
        }

        CString test = RunParams::getInstance().get( RPARAM_APPNAME ).c_str();

        SetWindowText( RunParams::getInstance().get( RPARAM_APPNAME ).c_str() );

        m_decisionIconStatic.SetIcon( AfxGetApp()->LoadStandardIcon( IDI_QUESTION  ) );
        m_decisionMessageStatic.SetWindowText( _T( m_decisionStr.c_str() ) );
        std::vector< std::string >::iterator choicesIt = m_choices.begin();
        while ( choicesIt != m_choices.end() )
        {
            m_decisionList.InsertString( ( choicesIt - m_choices.begin() ), 
                _T( ( *choicesIt ).c_str() ) );
            choicesIt++;
        }
        
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }

} // namespace TA_IRS_App
