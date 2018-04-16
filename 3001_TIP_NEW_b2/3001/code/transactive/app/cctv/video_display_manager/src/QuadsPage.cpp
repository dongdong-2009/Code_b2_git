/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/QuadsPage.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * TODO: <description>
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "videodisplaymanager.h"
#include "QuadsPage.h"
#include "SwitchAgentCommunicator.h"
#include "DisplayItemManager.h"
#include "Quad.h"
#include "VideoInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

    IMPLEMENT_DYNCREATE(QuadsPage, CPropertyPage)

    QuadsPage::QuadsPage() : CPropertyPage(QuadsPage::IDD), m_isOffsetInitialised( false ), m_bottomOffset( 0 )
    {
        FUNCTION_ENTRY( "QuadsPage" );

        // {{AFX_DATA_INIT(QuadsPage)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    QuadsPage::~QuadsPage()
    {
        FUNCTION_ENTRY( "~QuadsPage" );
        FUNCTION_EXIT;
    }


    void QuadsPage::setOffset()
    {
        FUNCTION_ENTRY( "setOffset" );

        CRect thisRect, treeRect;
        GetWindowRect( thisRect );
        CWnd* dlgItem = GetDlgItem( IDC_QUADTREE );
        dlgItem->GetWindowRect( treeRect );
        m_bottomOffset = ( thisRect.bottom - treeRect.bottom );

        m_isOffsetInitialised = true;

        FUNCTION_EXIT;
    }


    void QuadsPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CPropertyPage::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(QuadsPage)
        DDX_Control(pDX, IDC_QUADTREE, m_quadTreeCtrl);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(QuadsPage, CPropertyPage)
        // {{AFX_MSG_MAP(QuadsPage)
        ON_WM_SHOWWINDOW()
        ON_WM_SIZE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_QUAD_UPDATED, onQuadUpdated)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // QuadsPage message handlers

    BOOL QuadsPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        setOffset();

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void QuadsPage::Init()//REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        // 
        // Insert all quads
        // 

        // load the cameras
    // mgr.SetStaticText(0, "Initializing the Quads Page: inserting all Quads to GUI");

        std::map<std::string, Quad*> quads =
            DisplayItemManager::getInstance().getAllQuads();

        // build a vector
        std::vector<VideoInput*> inputs;
        for ( std::map<std::string, Quad*>::iterator it = quads.begin(); it != quads.end(); it++ )
        {
            // insert the item
            inputs.push_back(it->second);
        }

        m_quadTreeCtrl.insertVideoSources(inputs);

        DisplayItemManager::getInstance().registerForQuadChanges(this);

        FUNCTION_EXIT;
    }


    void QuadsPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForQuadChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void QuadsPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void QuadsPage::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        FUNCTION_ENTRY( "OnShowWindow" );

        CDialog::OnShowWindow(bShow, nStatus);

        // TODO: Add your message handler code here
        if ( TRUE == bShow )
        {
            // ::SetFocus(AfxGetApp()->m_pMainWnd->GetSafeHwnd());
            m_quadTreeCtrl.SetFocus();
        }

        FUNCTION_EXIT;
    }


    BOOL QuadsPage::OnKillActive()
    {
        FUNCTION_ENTRY( "OnKillActive" );

        // TODO: Add your specialized code here and/or call the base class

        m_quadTreeCtrl.resetToolTip();

        FUNCTION_EXIT;
        return CPropertyPage::OnKillActive();
    }


    void QuadsPage::OnSize(UINT nType, int cx, int cy)
    {
        FUNCTION_ENTRY( "OnSize" );

        CPropertyPage::OnSize(nType, cx, cy);

        if ( m_isOffsetInitialised )
        {
            CRect rect;
            m_quadTreeCtrl.GetWindowRect( rect );
            ScreenToClient( rect );
            int diff = ( rect.bottom - ( cy - m_bottomOffset ) );
            m_quadTreeCtrl.SetWindowPos(NULL, 0, 0, rect.Width(), ( rect.Height() - diff ),
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        FUNCTION_EXIT;
    }


    LRESULT QuadsPage::onQuadUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onQuadUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Quad update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Quad update received for NULL display item");

        // update the list control
        m_quadTreeCtrl.videoInputUpdated(input);

        FUNCTION_EXIT;
        return 0;
    }


} // TA_IRS_App


