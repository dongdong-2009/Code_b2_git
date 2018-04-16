/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/CamerasPage.cpp $
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
#include "CamerasPage.h"
#include "SwitchAgentCommunicator.h"
#include "DisplayItemManager.h"
#include "Camera.h"
#include "VideoInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

    IMPLEMENT_DYNCREATE(CamerasPage, CPropertyPage)

    CamerasPage::CamerasPage()
     : CPropertyPage(CamerasPage::IDD),
        m_isOffsetInitialised( false ),
        m_bottomOffset( 0 )
    {
        FUNCTION_ENTRY( "CamerasPage" );

        // {{AFX_DATA_INIT(CamerasPage)
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    CamerasPage::~CamerasPage()
    {
        FUNCTION_ENTRY( "~CamerasPage" );
        FUNCTION_EXIT;
    }


    void CamerasPage::setOffset()
    {
        FUNCTION_ENTRY( "setOffset" );

        CRect thisRect, treeRect;
        GetWindowRect( thisRect );
        CWnd* dlgItem = GetDlgItem( IDC_CAMERATREE );
        dlgItem->GetWindowRect( treeRect );
        m_bottomOffset = ( thisRect.bottom - treeRect.bottom );

        m_isOffsetInitialised = true;

        FUNCTION_EXIT;
    }


    void CamerasPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CPropertyPage::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(CamerasPage)
        DDX_Control(pDX, IDC_CAMERATREE, m_cameraTreeCtrl);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CamerasPage, CPropertyPage)
        // {{AFX_MSG_MAP(CamerasPage)
        ON_WM_SHOWWINDOW()
        ON_WM_SIZE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_CAMERA_UPDATED, onCameraUpdated)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CamerasPage message handlers

    BOOL CamerasPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        setOffset();

        CPropertyPage::OnInitDialog();
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CamerasPage::Init()//REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        // 
        // Insert all cameras
        // 
    // mgr.SetStaticText(0, "Initializing the Camera Page: inserting all cameras to GUI");
        // load the cameras
        std::map<std::string, Camera*> cameras =
            DisplayItemManager::getInstance().getAllCameras();

        // build a vector
        std::vector<VideoInput*> inputs;
        for ( std::map<std::string, Camera*>::iterator it = cameras.begin(); it != cameras.end(); it++ )
        {
            // insert the item
            inputs.push_back( it->second );
        }

        m_cameraTreeCtrl.insertVideoSources(inputs);

        DisplayItemManager::getInstance().registerForCameraChanges(this);

        FUNCTION_EXIT;
    }


    void CamerasPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForCameraChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void CamerasPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void CamerasPage::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        FUNCTION_ENTRY( "OnShowWindow" );

        CDialog::OnShowWindow(bShow, nStatus);

        if ( TRUE == bShow )
        {
            // ::SetFocus(AfxGetApp()->m_pMainWnd->GetSafeHwnd());
            m_cameraTreeCtrl.SetFocus();
        }

        FUNCTION_EXIT;
    }


    BOOL CamerasPage::OnKillActive()
    {
        FUNCTION_ENTRY( "OnKillActive" );

        m_cameraTreeCtrl.resetToolTip();

        FUNCTION_EXIT;
		return CPropertyPage::OnKillActive();
    }


    void CamerasPage::OnSize(UINT nType, int cx, int cy)
    {
        FUNCTION_ENTRY( "OnSize" );

      //  CPropertyPage::OnSize(nType, cx, cy);
		CPropertyPage::OnSize(nType,cx,cy);
        if ( m_isOffsetInitialised )
        {
            CRect rect;
           // m_cameraTreeCtrl.GetWindowRect( rect );
			GetWindowRect(rect);
            ScreenToClient( rect );
            int diff = ( rect.bottom - ( cy - m_bottomOffset ) );
            m_cameraTreeCtrl.SetWindowPos(NULL, 2, 2, rect.Width()-2, ( rect.Height()-2), // - diff ),
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        FUNCTION_EXIT;
    }


    LRESULT CamerasPage::onCameraUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onCameraUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Camera update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Camera update received for NULL display item");

        // update the list control
        m_cameraTreeCtrl.videoInputUpdated(input);

        FUNCTION_EXIT;
        return 0;
    }

} // TA_IRS_App



