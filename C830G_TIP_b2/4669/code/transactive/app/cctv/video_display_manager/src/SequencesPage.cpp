/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * TODO: <description>
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif


#include "stdafx.h"
#include "videodisplaymanager.h"
#include "SequencesPage.h"
#include "SwitchAgentCommunicator.h"
#include "DisplayItemManager.h"
#include "Sequence.h"
#include "VideoInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{
    IMPLEMENT_DYNCREATE(SequencesPage, CPropertyPage)

    SequencesPage::SequencesPage() : CPropertyPage(SequencesPage::IDD), m_isOffsetInitialised( false ), m_bottomOffset( 0 )
    {
        FUNCTION_ENTRY( "SequencesPage" );

        // {{AFX_DATA_INIT(SequencesPage)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    SequencesPage::~SequencesPage()
    {
        FUNCTION_ENTRY( "~SequencesPage" );
        FUNCTION_EXIT;
    }


    void SequencesPage::setOffset()
    {
        FUNCTION_ENTRY( "setOffset" );

        CRect thisRect, treeRect;
        GetWindowRect( thisRect );
        CWnd* dlgItem = GetDlgItem( IDC_SEQUENCETREE );
        dlgItem->GetWindowRect( treeRect );
        m_bottomOffset = ( thisRect.bottom - treeRect.bottom );

        m_isOffsetInitialised = true;

        FUNCTION_EXIT;
    }


    void SequencesPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CPropertyPage::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(SequencesPage)
        DDX_Control(pDX, IDC_SEQUENCETREE, m_sequenceTreeCtrl);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(SequencesPage, CPropertyPage)
        // {{AFX_MSG_MAP(SequencesPage)
        ON_WM_SHOWWINDOW()
        ON_WM_SIZE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_SEQUENCE_UPDATED, onSequenceUpdated)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // SequencesPage message handlers

    BOOL SequencesPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        setOffset();

        CPropertyPage::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SequencesPage::Init()//REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        // 
        // Insert all quads
        // 

        // load the cameras
    // mgr.SetStaticText(0, "Initializing the Sequences Page: inserting all sequences to GUI");

        std::map<std::string, Sequence*> sequences =
            DisplayItemManager::getInstance().getAllSequences();

        // build a vector
        std::vector<VideoInput*> inputs;
        for ( std::map<std::string, Sequence*>::iterator it = sequences.begin(); it != sequences.end(); it++ )
        {
            // insert the item
            inputs.push_back(it->second);
        }

        m_sequenceTreeCtrl.insertVideoSources(inputs);

        DisplayItemManager::getInstance().registerForSequenceChanges(this);

        FUNCTION_EXIT;
    }


    void SequencesPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForSequenceChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void SequencesPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void SequencesPage::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        FUNCTION_ENTRY( "OnShowWindow" );

        CDialog::OnShowWindow(bShow, nStatus);

        // TODO: Add your message handler code here
        if ( TRUE == bShow )
        {
            // ::SetFocus(AfxGetApp()->m_pMainWnd->GetSafeHwnd());
            m_sequenceTreeCtrl.SetFocus();
        }

        FUNCTION_EXIT;
    }


    BOOL SequencesPage::OnKillActive()
    {
        FUNCTION_ENTRY( "OnKillActive" );

        // TODO: Add your specialized code here and/or call the base class
        m_sequenceTreeCtrl.resetToolTip();

        FUNCTION_EXIT;
        return CPropertyPage::OnKillActive();
    }


    void SequencesPage::OnSize(UINT nType, int cx, int cy)
    {
        FUNCTION_ENTRY( "OnSize" );

        CPropertyPage::OnSize(nType, cx, cy);

        if ( m_isOffsetInitialised )
        {
            CRect rect;
            m_sequenceTreeCtrl.GetWindowRect( rect );
            ScreenToClient( rect );
            int diff = ( rect.bottom - ( cy - m_bottomOffset ) );
            m_sequenceTreeCtrl.SetWindowPos(NULL, 0, 0, rect.Width(), m_sequenceTreeCtrl.getSuitItemHeight( rect.Height() - diff ), // CL-21275
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        FUNCTION_EXIT;
    }


    LRESULT SequencesPage::onSequenceUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onSequenceUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Sequence update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Sequence update received for NULL display item");

        // update the list control
        m_sequenceTreeCtrl.videoInputUpdated(input);

        FUNCTION_EXIT;
        return 0;
    }

} // TA_IRS_App

