/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/QuadControllerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 11:31:40 $
  * Last modified by:  $Author: builder $
  *
  */
// QuadControllerDlg.cpp : implementation file
//

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "videodisplaymanager.h"
#include "SwitchAgentCommunicator.h"
#include "QuadControllerDlg.h"
#include "resource.h"
#include "DisplayItemManager.h"
#include "Quad.h"
#include "Camera.h"
#include "VideoInput.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    QuadControllerDlg::QuadControllerDlg(CWnd* pParent /*=NULL*/)
        : CDialog(QuadControllerDlg::IDD, pParent),
          m_mouseTracking(FALSE),
          m_hilitedControlID(0),
          m_prvHilitedControlID(0),
          m_dragAdjustPoint(0,0),
          m_contextActive(FALSE),
          m_dragImage(NULL),
          m_hilitingPerformed(TRUE),
          m_normalisationPerformed(TRUE),
          m_quad(NULL)
    {
        FUNCTION_ENTRY( "QuadControllerDlg" );

        // {{AFX_DATA_INIT(QuadControllerDlg)
        m_quadNameDesc = _T("");
        // }}AFX_DATA_INIT
        m_hIcon = AfxGetApp()->LoadIcon(IDI_QUAD);

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(QuadControllerDlg)
        DDX_Control(pDX, IDC_QC_APPLY_BUTTON, m_applyButton);
        DDX_Control(pDX, IDC_QC_CAMERATREE, m_cameraTreeCtrl);
        DDX_Text(pDX, IDC_SEQUENCENAMEDESC_EDIT, m_quadNameDesc);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(QuadControllerDlg, CDialog)
        // {{AFX_MSG_MAP(QuadControllerDlg)
        ON_WM_PAINT()
        ON_WM_LBUTTONDOWN()
        ON_WM_RBUTTONDOWN()
        ON_WM_MOUSEMOVE()
        ON_WM_LBUTTONUP()
        ON_WM_CONTEXTMENU()
        ON_BN_CLICKED(IDC_QC_APPLY_BUTTON, OnQcApplyButton)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_QUAD_UPDATED, onQuadUpdated)
        ON_MESSAGE(WM_CAMERA_UPDATED, onCameraUpdated)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // QuadControllerDlg message handlers

    int QuadControllerDlg::DoModal(Quad* quad)
    {
        FUNCTION_ENTRY( "DoModal" );

        TA_ASSERT(quad != NULL, "NULL quad object given to quad controller dialog");

        m_quad = quad;

        // show the dialog
        int result = CDialog::DoModal();

        // get rid of the local quad pointer
        m_quad = NULL;

        FUNCTION_EXIT;
        return result;
    }


    BOOL QuadControllerDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        TA_ASSERT(m_quad != NULL, "Cannot load a quad controller dialog with a NULL quad");

        // Set the icon for this dialog.
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon

        CString windowText("Quad Controller - ");
        windowText += m_quad->getName().c_str();
        SetWindowText(windowText);

        // 
        // Insert all cameras
        // 

        m_cameraTreeCtrl.setToQuadControllerTree();

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


        registerMonitors();
        registerLabels();


        loadExistingSegmentAssociations();

        m_applyButton.EnableWindow(FALSE);

        DisplayItemManager::getInstance().registerForCameraChanges(this);
        DisplayItemManager::getInstance().registerForQuadChanges(this);

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void QuadControllerDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForQuadChanges(this);
        DisplayItemManager::getInstance().deregisterForCameraChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void QuadControllerDlg::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void QuadControllerDlg::loadExistingSegmentAssociations()
    {
        FUNCTION_ENTRY( "loadExistingSegmentAssociations" );

        TA_ASSERT(m_quad != NULL, "Cannot load existing quad associations when m_quad is NULL");
        m_segmentAssociations = m_quad->getCameras();

        for (UINT i = 0; i < m_segmentAssociations.size(); i++)
        {
            CString text("");
            if (m_segmentAssociations[i] != NULL)
            {
                text = m_segmentAssociations[i]->getName().c_str();
            }
            m_labels[i]->SetText(text);
        }

        FUNCTION_EXIT;
    }


    /**
      * RegisterMonitors
      *
      * Registers all Vidiwall Monitors on the dialog.
      */
    void QuadControllerDlg::registerMonitors()
    {
        FUNCTION_ENTRY( "registerMonitors" );

        // Register all of the display devices.
        int my_first_monitor = IDC_QUADSEG1;
        int my_last_monitor = IDC_QUADSEG4;

        for( int i = my_first_monitor; i <= my_last_monitor; i++ )
        {
            boost::shared_ptr<CStatic> pStatic( new CStatic );

            m_monitors.push_back( pStatic );
            m_monitors[i - my_first_monitor]->SubclassDlgItem( i, this );
        }

        FUNCTION_EXIT;
    }


    /**
      * RegisterLabels
      *
      * Registers all Vidiwall Monitors on the dialog.
      */
    void QuadControllerDlg::registerLabels()
    {
        FUNCTION_ENTRY( "registerLabels" );

        // Register all of the display devices.
        // Register all of the display devices.
        int my_first_monitor = IDC_TEXT_QUADSEG_1;
        int my_last_monitor = IDC_TEXT_QUADSEG_4;

        for( int i = my_first_monitor; i <= my_last_monitor; i++ )
        {
            boost::shared_ptr<CLabel> pLabel( new CLabel );

            m_labels.push_back( pLabel );
            m_labels[i - my_first_monitor]->SubclassDlgItem( i, this );
            m_labels[i - my_first_monitor]->SetTransparent(TRUE);
            // m_labels[i - my_first_monitor]->SetText("test");
        }

        FUNCTION_EXIT;
    }


    BOOL QuadControllerDlg::DestroyWindow()
    {
        FUNCTION_ENTRY( "DestroyWindow" );

        if ( NULL != m_dragImage )
        {
            delete m_dragImage;
            m_dragImage = NULL;
        }

        FUNCTION_EXIT;
        return CDialog::DestroyWindow();
    }


    void QuadControllerDlg::OnPaint()
    {
        FUNCTION_ENTRY( "OnPaint" );

        CPaintDC dc(this); // device context for painting

        // Do not call CDialog::OnPaint() for painting messages

        // Invalidate both appropriate control labels.
        invalidateControlLabels();

        // Paint non-hilited control.
        if ( ( m_prvHilitedControlID != 0 ) && ( m_normalisationPerformed == FALSE ) )
        {
            paintStandardControl();
        }

        // Paint hilited control.
        if ( ( m_hilitedControlID != 0 ) && ( m_hilitingPerformed == FALSE ) )
        {
            paintHilitedControl();
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::updateDialog()
    {
        FUNCTION_ENTRY( "updateDialog" );

        SendMessage( WM_PAINT, NULL, NULL );

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::paintHilitedControl()
    {
        FUNCTION_ENTRY( "paintHilitedControl" );

        m_hilitingPerformed = TRUE;
        std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;

        for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
        {
            if( ( *theIterator )->GetDlgCtrlID() == m_hilitedControlID )
            {
                ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                    MAKEINTRESOURCE( IDB_QUADSEG_HILITE ) ) );

                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::paintStandardControl()
    {
        FUNCTION_ENTRY( "paintStandardControl" );

        m_normalisationPerformed = TRUE;
        std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;

        for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
        {
            if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedControlID )
            {
                ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                    MAKEINTRESOURCE( IDB_QUADSEG ) ) );

                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    BOOL QuadControllerDlg::isOverMonitor(CPoint& pt)
    {
        FUNCTION_ENTRY( "isOverMonitor" );

		std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;
        CRect wndRect;

        for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
        {
            ( *theIterator )->GetWindowRect( &wndRect );
            ScreenToClient( &wndRect );

            if( wndRect.PtInRect( pt ) )
            {
                if ( (* theIterator )->GetDlgCtrlID() != m_hilitedControlID )
                {
                    // m_prvHilitedMonitorID and m_hilitedMonitorID are reinitialized here.
                    m_prvHilitedControlID = m_hilitedControlID;
                    if ( 0 != m_prvHilitedControlID )
                    {
                        m_normalisationPerformed = FALSE;
                    }
                    m_hilitedControlID = (* theIterator )->GetDlgCtrlID();
                    m_hilitingPerformed = FALSE;

                    FUNCTION_EXIT;
                    return TRUE;
                }
                else if ( (* theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                {
                    // still over monitor
                    FUNCTION_EXIT;
                    return TRUE;
                }
            }
        }

        FUNCTION_EXIT;
        return FALSE;
    }


    void QuadControllerDlg::invalidateControlLabels()
    {
        FUNCTION_ENTRY( "invalidateControlLabels" );

        if ( ( m_normalisationPerformed && m_hilitingPerformed ) )//&&
            // (FALSE == m_mouseTracking ) )
        {
            // nothing to do
            FUNCTION_EXIT;
            return;
        }

        // Declare an iterator for the vector.
		std::vector< boost::shared_ptr< CLabel > >::iterator theIterator;

        // Iterate through the vector and invalidate both of the appropriate labels.
        int i = 0;
        int diff = (IDC_TEXT_QUADSEG_1 - IDC_QUADSEG1);
        for ( theIterator = m_labels.begin(); theIterator != m_labels.end(); theIterator++ )
        {
            if( m_hilitedControlID == ( * theIterator )->GetDlgCtrlID() - diff )
            {
                if ( ( FALSE == m_hilitingPerformed ) )//|| ( m_mouseTracking ) )
                {
                    ( *theIterator )->Invalidate();
                }
            }
            if( m_prvHilitedControlID  == ( * theIterator )->GetDlgCtrlID() - diff )
            {
                if ( ( FALSE == m_normalisationPerformed ) )//|| ( m_mouseTracking ) )
                {
                    ( *theIterator )->Invalidate();
                }
            }
            i++;
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonDown" );

        CDialog::OnLButtonDown( nFlags, point );

        CPoint nullPt( 0,0 );

        // point == nullPt if the user clicked in the Video Source region.
        if( point == nullPt )
        {
            SetCapture();
            m_mouseTracking = TRUE;
            CPoint cursorLocation;
            ::GetCursorPos(&cursorLocation);

            m_dragImage->BeginDrag(0, m_dragAdjustPoint);
            m_dragImage->DragEnter(NULL,cursorLocation);
        }
        // The user clicked outside the Video Sources region.
        else
        {
            // Reset the hilited and non-hilited monitor.
            if ( isOverMonitor(point) )
            {
                updateDialog();
                m_monitors[0]->SetFocus();

                FUNCTION_EXIT;
                return;
            }
            else
            {
                CPoint negPt( -1,-1 );
                if ( (m_hilitedControlID != 0) && (point == negPt) )
                {
                    m_prvHilitedControlID = m_hilitedControlID;
                    m_normalisationPerformed = FALSE;
                    m_hilitedControlID = 0;
                    updateDialog();
                }
            }
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnRButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnRButtonDown" );

        // Invoke the left button down functionality to ensure
        // that the hilited and non hilited monitors are updated.
        if (FALSE == m_contextActive)
        {
            LPARAM lParam = 0;
            lParam = ( ( point.x ) + (point.y << 16 ) );
            SendMessage( WM_LBUTTONDOWN, MK_RBUTTON, lParam );
        }
        CDialog::OnRButtonDown(nFlags, point);

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::setDragImage(CImageList* dragImage)
    {
        FUNCTION_ENTRY( "setDragImage" );

        if ( m_dragImage != NULL )
        {
            delete m_dragImage;
            m_dragImage = NULL;
        }
        m_dragImage = dragImage;

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnMouseMove(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnMouseMove" );

        short hilite = m_hilitedControlID;
        short prev = m_prvHilitedControlID;

        // If left mouse button is held down change to drag cursor
        // and keep resetting the current hilited monitor.
        if( m_mouseTracking )
        {
            SetCursor( AfxGetApp()->LoadCursor( IDC_DRAG_CURSOR ) );
            // Repaint the dialog as the hilited / non-hilited monitors change.
            if( FALSE == isOverMonitor( point ) )
            {
                m_prvHilitedControlID = m_hilitedControlID;
                m_hilitedControlID = 0;
            }

            if ( (hilite != m_hilitedControlID ) || (prev != m_prvHilitedControlID ) )
            {
                m_dragImage->DragLeave(NULL);
                m_dragImage->EndDrag();
                m_contextActive = TRUE;
                if (prev != m_prvHilitedControlID )
                {
                    m_normalisationPerformed = FALSE;
                }
                updateDialog();
            }
            else
            {
                ClientToScreen(&point);
                if ( TRUE == m_contextActive )
                {
                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                    m_dragImage->DragEnter(NULL,point);
                    m_contextActive = FALSE;
                }
                m_dragImage->DragMove(point);
                ScreenToClient(&point);
            }
        }
        CDialog::OnMouseMove(nFlags, point);

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnLButtonUp(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonUp" );

        CDialog::OnLButtonUp(nFlags, point);

        // Release mouse and load standard arrow cursor.
        if ( TRUE == m_mouseTracking )
        {
            ::ReleaseCapture();
            m_cameraTreeCtrl.endDragging();
            m_mouseTracking = FALSE;
            m_contextActive = FALSE;
            SetCursor( LoadCursor( NULL, IDC_ARROW ) );
            m_dragImage->DragLeave(this);
            m_dragImage->EndDrag();
            if ( NULL != m_dragImage )
            {
                delete m_dragImage;
                m_dragImage = NULL;
            }
            if ( isOverMonitor(point) )
            {
                unsigned int i = 0;
                for ( i; i < m_monitors.size(); i++ )
                {
                    if( m_monitors[i]->GetDlgCtrlID() == m_hilitedControlID )
                    {
                        // get the video source that was 'dropped'
                        VideoInput* source = m_cameraTreeCtrl.getSelectedInput();
                        Camera* camera = dynamic_cast<Camera*>(source);

                        // a camera was just dropped onto the monitor
                        if ( camera != NULL )
                        {
                            // update the label
                            m_labels[i]->SetText(camera->getName().c_str());

                            // update the local data
                            m_segmentAssociations[i] = camera;

                            m_monitors[0]->SetFocus();
                        }

                        adjustApplyButton();
                    }
                }
            }
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::adjustApplyButton()
    {
        FUNCTION_ENTRY( "adjustApplyButton" );

        TA_ASSERT(m_quad != NULL, "Cannot test against a NULL quad");
        std::vector<Camera*> cameras = m_quad->getCameras();

        for (int i = 0; i < 4; i++)
        {
            if (m_segmentAssociations[i] != cameras[i])
            {
                this->m_applyButton.EnableWindow(TRUE);

                FUNCTION_EXIT;
                return;
            }
        }

        m_applyButton.EnableWindow(FALSE);

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY( "OnContextMenu" );

        // if context menu currently displayed
        if ( (TRUE == m_contextActive) || (TRUE == m_mouseTracking) )
        {
            FUNCTION_EXIT;
            return;
        }

        ScreenToClient(&point);
        // Load the context menu.
        CMenu contextMenu;
        contextMenu.LoadMenu( IDR_QUADCONTEXT_MENU );
        CMenu* pContextMenu = contextMenu.GetSubMenu( 0 );

        // Only display context menu if the monitor is hilited and has an associated camera
        // or sequence.
        if( isOverMonitor( point ) )
        {
            updateDialog();

            // get the associated camera (NULL if none)
            Camera* associatedCamera = m_segmentAssociations[m_hilitedControlID - m_monitors[0]->GetDlgCtrlID()];

            if ( associatedCamera != NULL )
            {
                // Display the context menu.
                m_contextActive = TRUE;
                ClientToScreen(&point);
                int nCmd = ( int ) pContextMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                    point.x, point.y, AfxGetMainWnd() );
                // Process the appropriate context menu command.
                m_contextActive = FALSE;
                switch ( nCmd )
                {
                case ID_CLEAR_MONITOR:
                    {
                        unsigned int i = 0;
                        for ( i; i < m_monitors.size(); i++ )
                        {
                            if( m_monitors[i]->GetDlgCtrlID() == m_hilitedControlID )
                            {
                                m_labels[i]->SetText("");
                                m_segmentAssociations[i] = NULL;
                                adjustApplyButton();
                            }
                        }
                    }
                    break;
                default:
                    {
                        // do nothing
                    }
                    break;
                }
            }

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void QuadControllerDlg::OnQcApplyButton()
    {
        FUNCTION_ENTRY( "OnQcApplyButton" );

        TA_ASSERT(m_quad != NULL, "Cannot apply a NULL quad");

        try
        {
            TA_IRS_App::SwitchAgentCommunicator::getInstance().assignQuad(
                m_quad,
                m_segmentAssociations[0],
                m_segmentAssociations[1],
                m_segmentAssociations[2],
                m_segmentAssociations[3] );

            m_applyButton.EnableWindow(FALSE);
        }
        catch(TA_Base_Core::TransactiveException& te)
        {
            CString message = "Switching of quad inputs";
            CString errorNumber1 = "UE-090047";
            CString errorNumber2 = "UE-090048";
            CString errorNumber3 = "UE-020056";
            CString errorNumber4 = "UE-010021";

            TA_Base_Bus::TransActiveMessage userMsg;

            // TD16879
            if(te.what() == errorNumber1)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
            }
            else if(te.what() == errorNumber2)
            {
                userMsg << message ;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
            }
            else if(te.what() == errorNumber3)
            {
                userMsg << message ;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
            }
            else if(te.what() == errorNumber4)
            {
				std::string userName;
				std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
				TA_Base_Bus::AuthenticationLibrary authLibrary;
				TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
				std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
				userName = iOperator->getName();
				userMsg << userName;
	     		userMsg << message;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
            }
            else
            {
                userMsg << message;
                userMsg<< te.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
            }
            // TD16879
        }

        FUNCTION_EXIT;
    }


    BOOL QuadControllerDlg::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        // TODO: Add your specialized code here and/or call the base class
        switch ( pMsg->message )
        {
        case WM_SYSCOMMAND:
            {
                // otherwise wont respond to Alt/F4 during capture
                if ( pMsg->wParam == SC_CLOSE )
                {
                    ::ReleaseCapture();
                    if ( m_mouseTracking && (m_dragImage != NULL) )
                    {
                        m_dragImage->DragLeave(this);
                        m_dragImage->EndDrag();
                        delete m_dragImage;
                        m_dragImage = NULL;
                        SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                    }
                }
            }
            break;
        default:
            {
                // do nothing
            }
            break;
        }

        FUNCTION_EXIT;
        return CDialog::PreTranslateMessage(pMsg);
    }


    LRESULT QuadControllerDlg::onQuadUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onQuadUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Quad update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Quad update received for NULL display item");

        if (input == m_quad)
        {
            // the current quad was updated

            // it must be tested against the current state
            // to see if it was changed externally or not
            // (this could be the result of a save operation made here)
            bool same = true;

            // test the local cameras against the saved cameras
            std::vector<Camera*> cameras = m_quad->getCameras();
            for (int i = 0; i < 4; i++)
            {
                if (m_segmentAssociations[i] != cameras[i])
                {
                    same = false;
                    break;
                }
            }


            if ( !same )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_090070);

                // AfxMessageBox( "The existing inputs of the currently loaded Quad have been modified." );

                // reload the associations
                loadExistingSegmentAssociations();
                adjustApplyButton();
            }
        }

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT QuadControllerDlg::onCameraUpdated(WPARAM wParam, LPARAM lParam)
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
