/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/TrainborneControllerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/22 10:52:21 $
  * Last modified by:  $Author: builder $
  *
  */
// TrainborneControllerDlg.cpp : implementation file
//

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"

#include "TrainborneControllerDlg.h"
#include "videodisplaymanager.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TrainborneControllerDlg dialog


namespace TA_IRS_App
{


        TrainborneControllerDlg::TrainborneControllerDlg(CWnd* pParent /*=NULL*/)
            : CDialog(TrainborneControllerDlg::IDD, pParent), m_mouseTracking(FALSE),
                m_hilitedControlID(0), m_prvHilitedControlID(0), m_dragAdjustPoint(0,0), m_contextActive(FALSE),
                m_dragImage(NULL), m_hilitingPerformed(TRUE), m_normalisationPerformed(TRUE),
                m_hilitedCamControlID(0), m_prvHilitedCamControlID(0), m_camHilitingPerformed(TRUE), m_camNormalisationPerformed(TRUE),
                m_firstSelection(TRUE), m_singleEnabled(false), m_quadEnabled(false)
        {
            FUNCTION_ENTRY( "TrainborneControllerDlg" );

            //{{AFX_DATA_INIT(TrainborneControllerDlg)
            m_trainName = _T("");
            //}}AFX_DATA_INIT
            m_hIcon = AfxGetApp()->LoadIcon(IDI_TRAIN);

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::DoDataExchange(CDataExchange* pDX)
        {
            FUNCTION_ENTRY( "DoDataExchange" );

            CDialog::DoDataExchange(pDX);
            //{{AFX_DATA_MAP(TrainborneControllerDlg)
            DDX_Control(pDX, IDC_SEQUENCERADIO, m_sequenceRadioButton);
            DDX_Control(pDX, IDC_QUADRADIO, m_quadRadioButton);
            DDX_Control(pDX, IDC_SINGLERADIO, m_singleRadioButton);
            DDX_Control(pDX, IDC_SEQUENCECOMBO, m_sequenceComboBox);
            DDX_Control(pDX, IDC_TC_APPLY_BUTTON, m_applyButton);
            DDX_Text(pDX, IDC_TC_TRAIN_LABEL, m_trainName);
            //}}AFX_DATA_MAP

            FUNCTION_EXIT;
        }


        int TrainborneControllerDlg::DoModal(TA_IRS_Bus::TrainCctvCorbaProxyPtr& trainCctvCorbaProxy, VideoSourcesListCtrl::TrainDetails* trainDetails)
        {
            FUNCTION_ENTRY( "DoModal" );

            // TODO: Add your specialized code here and/or call the base class

            m_trainCctvCorbaProxy = trainCctvCorbaProxy;

            if(trainDetails != NULL)
            {
                m_trainDetails = trainDetails;

                CString trainIDStr = m_trainDetails->serviceNumber.c_str();
                trainIDStr = "Train" + trainIDStr;

                m_trainName = trainIDStr;

                BvsStage* stage = DisplayItemManager::getInstance().getBvsStageForLocation(m_trainDetails->currentLocation);

                if(stage!=NULL)
                {
                    m_bvsStage = stage;

                    FUNCTION_EXIT;
                    return CDialog::DoModal();
                }
            }

            FUNCTION_EXIT;
            return 0;
        }


        BEGIN_MESSAGE_MAP(TrainborneControllerDlg, CDialog)
            //{{AFX_MSG_MAP(TrainborneControllerDlg)
            ON_BN_CLICKED(IDC_SINGLERADIO, OnSingleradio)
            ON_BN_CLICKED(IDC_SEQUENCERADIO, OnSequenceradio)
            ON_BN_CLICKED(IDC_QUADRADIO, OnQuadradio)
            ON_WM_LBUTTONDOWN()
            ON_WM_RBUTTONDOWN()
            ON_WM_MOUSEMOVE()
            ON_WM_DESTROY()
            ON_WM_PAINT()
            ON_WM_LBUTTONUP()
            ON_WM_SETFOCUS()
            ON_CBN_SELCHANGE(IDC_SEQUENCECOMBO, OnSelchangeSequencecombo)
            ON_BN_CLICKED(IDC_TC_APPLY_BUTTON, OnTcApplyButton)
            ON_MESSAGE(WM_STAGE_UPDATED, onStageUpdated)
            //}}AFX_MSG_MAP
        END_MESSAGE_MAP()

        /////////////////////////////////////////////////////////////////////////////
        // TrainborneControllerDlg message handlers

        BOOL TrainborneControllerDlg::OnInitDialog()
        {
            FUNCTION_ENTRY( "OnInitDialog" );

            CDialog::OnInitDialog();

            DisplayItemManager::getInstance().registerForStageChanges(this);

            // Set the icon for this dialog.
            SetIcon(m_hIcon, TRUE);            // Set big icon
            SetIcon(m_hIcon, FALSE);        // Set small icon

            // TODO: Add extra initialization
            SetWindowText("Trainborne CCTV Controller - " + m_trainName);

            registerMonitors();

            registerTrainCams();

            registerLabels();

            m_sequenceComboBox.InsertString(0,"1 - Trainborne Sequence 1");
            m_sequenceComboBox.InsertString(1,"2 - Trainborne Sequence 2");
            m_sequenceComboBox.InsertString(2,"3 - Trainborne Sequence 3");
            m_sequenceComboBox.InsertString(3,"4 - Trainborne Sequence 4");
            m_sequenceComboBox.InsertString(4,"5 - Trainborne Sequence 5");
            m_sequenceComboBox.InsertString(5,"6 - Trainborne Sequence 6");
            // Find the longest string in the combo box.
            CString str;
            CSize   sz;
            int     dx=0;
            CDC*    pDC = m_sequenceComboBox.GetDC();
            for (int i=0;i < m_sequenceComboBox.GetCount();i++)
            {
               m_sequenceComboBox.GetLBText( i, str );
               sz = pDC->GetTextExtent(str);
               if (sz.cx > dx)
               {
                   dx = sz.cx;
               }

            }
            m_sequenceComboBox.ReleaseDC(pDC);

            // Adjust the width for the vertical scroll bar and the left and right border.
            dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

            // Set the width of the list box so that every item is completely visible.
            m_sequenceComboBox.SetDroppedWidth(dx);


            m_applyButton.EnableWindow(FALSE);

            m_sequenceComboBox.EnableWindow(FALSE);


            //we need to iniatialise this window to what is currently displaying on
            //the train

            switch(m_bvsStage->getActiveVideoSource())
            {
                case BvsStage::Single:
                    m_singleRadioButton.SetCheck(1);
                    Singleradio(true);
                    m_labels[0]->SetText(m_camTextLabels[m_bvsStage->getAssignment1()]);

                    break;
                case BvsStage::Quad:
                    m_quadRadioButton.SetCheck(1);
                    Quadradio(true);
                    m_labels[1]->SetText(m_camTextLabels[m_bvsStage->getAssignment1()]);
                    m_labels[2]->SetText(m_camTextLabels[m_bvsStage->getAssignment2()]);
                    m_labels[3]->SetText(m_camTextLabels[m_bvsStage->getAssignment3()]);
                    m_labels[4]->SetText(m_camTextLabels[m_bvsStage->getAssignment4()]);
                    break;
                case BvsStage::Sequence:
                    m_sequenceRadioButton.SetCheck(1);
                    OnSequenceradio();
                    m_sequenceComboBox.SetCurSel(m_bvsStage->getAssignment1() - 1);
                    break;
                case BvsStage::None:
                    break;
            }

            FUNCTION_EXIT;
            return TRUE;  // return TRUE unless you set the focus to a control
                          // EXCEPTION: OCX Property Pages should return FALSE
        }


        void TrainborneControllerDlg::OnDestroy()
        {
            FUNCTION_ENTRY( "OnDestroy" );

            DisplayItemManager::getInstance().deregisterForStageChanges(this);
            CDialog::OnDestroy();

            FUNCTION_EXIT;
        }


        /**
          * RegisterMonitors
          *
          * Registers all Vidiwall Monitors on the dialog.
          */
        void TrainborneControllerDlg::registerMonitors()
        {
            FUNCTION_ENTRY( "registerMonitors" );

            // Register all of the display devices.
            int my_first_monitor = IDC_TRAINSINGLEMON;
            int my_last_monitor = IDC_TRAINQUADMON4;

            for( int i = my_first_monitor; i <= my_last_monitor; i++ )
            {
				boost::shared_ptr<CTransparentImage> pTransparentImage( new CTransparentImage );

                m_monitors.push_back( pTransparentImage );
                m_monitors[i - my_first_monitor]->SubclassDlgItem( i, this );
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::registerLabels()
        {
            FUNCTION_ENTRY( "registerLabels" );

            // Register all of the display devices.
            int my_first_monitor = IDC_SINGLEMON_TEXT;
            int my_last_monitor = IDC_QUAD4_TEXT;

            for( int i = my_first_monitor; i <= my_last_monitor; i++ )
            {
				boost::shared_ptr<CLabel> pLabel( new CLabel );

                m_labels.push_back( pLabel );
                m_labels[i - my_first_monitor]->SubclassDlgItem( i, this );
                m_labels[i - my_first_monitor]->SetTransparent(TRUE);
            }

            FUNCTION_EXIT;
        }


        /**
          * RegisterMonitors
          *
          * Registers all Vidiwall Monitors on the dialog.
          */
        void TrainborneControllerDlg::registerTrainCams()
        {
            FUNCTION_ENTRY( "registerTrainCams" );

            // no camera
            m_camTextLabels.push_back("");

            // Register all of the display devices.
            int my_first_trainCam = IDC_TRAINCAM1;
            int my_last_trainCam = IDC_TRAINCAM8;

            for( int i = my_first_trainCam; i <= my_last_trainCam; i++ )
            {
				boost::shared_ptr<CTransparentImage> pTransparentImage( new CTransparentImage );
                CString cam;
                cam.Format("Cam %d",(i - my_first_trainCam + 1));
                m_trainCameras.push_back( pTransparentImage );
                m_camTextLabels.push_back(cam);
                m_trainCameras[i - my_first_trainCam]->SubclassDlgItem( i, this );
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::updateDialog()
        {
            FUNCTION_ENTRY( "updateDialog" );

            SendMessage( WM_PAINT, NULL, NULL );

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnSingleradio()
        {
            FUNCTION_ENTRY( "OnSingleradio" );
			bool doResetCam = true;

            // TODO: Add your control notification handler code here
            if(m_singleRadioButton.GetCheck() == 1)
            {
                m_singleEnabled = true;
                m_quadEnabled = false;

                if ( (doResetCam == false) || (m_sequenceRadioButton.GetCheck() == 1) || (m_quadRadioButton.GetCheck() == 1) || (m_firstSelection == TRUE))
                {
                    m_firstSelection = FALSE;
                    m_sequenceRadioButton.SetCheck(0);
                    m_quadRadioButton.SetCheck(0);
                    m_applyButton.EnableWindow(FALSE);
                    if ( doResetCam )
                    {
                        m_hilitedControlID = 0;
                        m_prvHilitedCamControlID = m_hilitedCamControlID;
                        m_hilitedCamControlID = 0;
                        if (m_prvHilitedCamControlID != 0)
                        {
                            m_camNormalisationPerformed = FALSE;
                        }
                    }
                    // Create clearing bitmap
                    // Create bitmap
                    CClientDC    dc (this);
                    CDC         memDC;
                    if(!memDC.CreateCompatibleDC(&dc))
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                    CBitmap bitmap;
                    BITMAP bm;
                    bitmap.LoadBitmap(IDB_QUADVMON_DISABLED);
                    bitmap.GetBitmap(&bm);
                    CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );

                    m_sequenceComboBox.EnableWindow(FALSE);
					int i = 1;
                    for (; i < 5; i++)
                    {
                        m_labels[i]->SetText("");
                        m_monitors[i]->SetBitmap( bitmap );
                    }
                    updateDialog();

                    for (i = 1; i < 5; i++)
                    {
                        m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_QUADVMON_DISABLED ) ) );
                    }
                    if ( doResetCam )
                    {
                        m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                    MAKEINTRESOURCE( IDB_SINGLEVMON_FORTRAIN ) ) );
                    }
                    updateDialog();
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnSequenceradio()
        {
            FUNCTION_ENTRY( "OnSequenceradio" );

            // TODO: Add your control notification handler code here
            if(m_sequenceRadioButton.GetCheck() == 1)
            {
                this->m_singleEnabled = false;
                this->m_quadEnabled = false;

                if ( (m_quadRadioButton.GetCheck() == 1) || (m_singleRadioButton.GetCheck() == 1)  || (m_firstSelection == TRUE))
                {
                    m_firstSelection = FALSE;
                    m_singleRadioButton.SetCheck(0);
                    m_quadRadioButton.SetCheck(0);
                    if (this->m_sequenceComboBox.GetCurSel() != -1)
                    {
                        m_applyButton.EnableWindow(TRUE);
                    }

                    else
                    {
                        m_applyButton.EnableWindow(FALSE);
                    }

                    m_hilitedControlID = 0;

                    m_prvHilitedCamControlID = m_hilitedCamControlID;
                    m_hilitedCamControlID = 0;
                    if (m_prvHilitedCamControlID != 0)
                    {
                        m_camNormalisationPerformed = FALSE;
                    }
                    // Create clearing bitmap
                    // Create bitmap
                    CClientDC    dc (this);
                    CDC         memDC;
                    if(!memDC.CreateCompatibleDC(&dc))
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                    CBitmap bitmapSing, bitmap;
                    BITMAP bm;
                    bitmapSing.LoadBitmap(IDB_SINGLEVMON_DISABLED);
                    bitmap.LoadBitmap(IDB_QUADVMON_DISABLED);
                    bitmapSing.GetBitmap(&bm);
                    CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmapSing );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );
                    bitmap.GetBitmap(&bm);
                    pOldMemDCBitmap = memDC.SelectObject( &bitmap );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );
                    m_sequenceComboBox.EnableWindow(TRUE);
                    m_labels[0]->SetText("");
                    m_monitors[0]->SetBitmap( bitmapSing );
					int i = 1;
                    for (; i < 5; i++)
                    {
                        m_labels[i]->SetText("");
                        m_monitors[i]->SetBitmap( bitmap );
                    }
                    updateDialog();
                    m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_SINGLEVMON_DISABLED ) ) );
                    for (i = 1; i < 5; i++)
                    {
                        m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_QUADVMON_DISABLED ) ) );
                    }
                    updateDialog();
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnQuadradio()
        {
            FUNCTION_ENTRY( "OnQuadradio" );
			bool doResetCam = true;

            if(m_quadRadioButton.GetCheck() == 1)
            {
                // TODO: Add your control notification handler code here
                m_singleEnabled = false;
                m_quadEnabled = true;
                if ( (doResetCam == false) || (m_sequenceRadioButton.GetCheck() == 1) || (m_singleRadioButton.GetCheck() == 1) || (m_firstSelection == TRUE))
                {
                    m_firstSelection = FALSE;
                    m_singleRadioButton.SetCheck(0);
                    m_sequenceRadioButton.SetCheck(0);
                    m_applyButton.EnableWindow(FALSE);
                    if ( doResetCam )
                    {
                        m_hilitedControlID = 0;
                        m_prvHilitedCamControlID = m_hilitedCamControlID;
                        m_hilitedCamControlID = 0;
                        if (m_prvHilitedCamControlID != 0)
                        {
                            m_camNormalisationPerformed = FALSE;
                        }
                    }
                    // Create clearing bitmap
                    // Create bitmap
                    CClientDC    dc (this);
                    CDC         memDC;
                    if(!memDC.CreateCompatibleDC(&dc))
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                    CBitmap bitmap;
                    BITMAP bm;
                    bitmap.LoadBitmap(IDB_SINGLEVMON_DISABLED);
                    bitmap.GetBitmap(&bm);
                    CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );
                    m_sequenceComboBox.EnableWindow(FALSE);
                    m_labels[0]->SetText("");
                    m_monitors[0]->SetBitmap( bitmap );
                    updateDialog();
                    m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_SINGLEVMON_DISABLED ) ) );
                    if ( doResetCam )
                    {
                        for (int i = 1; i < 5; i++)
                        {
                            m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                    MAKEINTRESOURCE( IDB_QUADVMON ) ) );
                        }
                    }
                    updateDialog();
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnLButtonDown(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnLButtonDown" );

            // TODO: Add your message handler code here and/or call default

            CDialog::OnLButtonDown(nFlags, point);

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
                int first, last;
                if ( 1 == m_singleRadioButton.GetCheck() )
                {
                    first = 0;
                    last = 1;
                }
                else if ( 1 == m_quadRadioButton.GetCheck() )
                {
                    first = 1;
                    last = 5;
                }
                else
                {
                    first = last = 0;
                }
                // Reset the hilited and non-hilited monitor.
                if ( isOverMonitor(point, first, last) )
                {
                    m_prvHilitedCamControlID = m_hilitedCamControlID;
                    m_hilitedCamControlID = 0;
                    if (m_prvHilitedCamControlID != 0)
                    {
                        m_camNormalisationPerformed = FALSE;
                    }
                    updateDialog();

                    FUNCTION_EXIT;
                    return;
                }
                else if ( isOverTrainCam(point) )
                {
                    m_hilitedControlID = 0;
                    updateDialog();
                    SetCapture();
                    m_mouseTracking = TRUE;
                    setDragImage(CreateDragImage());
                    CRect wndRect;
                    m_trainCameras[m_hilitedCamControlID-IDC_TRAINCAM1]->GetWindowRect( &wndRect );
                    ScreenToClient( &wndRect );
                    setDragAdjust(CPoint((point.x - wndRect.left),(point.y - wndRect.top)));
                    CPoint cursorLocation;
                    ::GetCursorPos(&cursorLocation);
                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                    m_dragImage->DragEnter(NULL,cursorLocation);

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


        void TrainborneControllerDlg::OnRButtonDown(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnRButtonDown" );

            // TODO: Add your message handler code here and/or call default

            CDialog::OnRButtonDown(nFlags, point);

            FUNCTION_EXIT;
        }


        BOOL TrainborneControllerDlg::isOverMonitor(CPoint& pt, int first, int last)
        {
            FUNCTION_ENTRY( "isOverMonitor" );

            CRect wndRect;
            for ( int i = first; i < last; i++ )
            {
                m_monitors[i]->GetWindowRect( &wndRect );
                ScreenToClient( &wndRect );

                if( wndRect.PtInRect( pt ) )
                {
                    if ( m_monitors[i]->GetDlgCtrlID() != m_hilitedControlID )
                    {
                        if (m_monitors[i]->GetDlgCtrlID() < IDC_TRAINQUADMON1)
                        {
                            if ( false == m_singleEnabled )
                            {
                                if (m_dragImage != NULL)
                                {
                                    m_dragImage->DragLeave(NULL);
                                    m_dragImage->EndDrag();
                                }
                                enableSingle();
                                if (m_dragImage != NULL)
                                {
                                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                                    m_dragImage->DragEnter(NULL,pt);
                                }
                            }
                        }
                        else if (m_monitors[i]->GetDlgCtrlID() >= IDC_TRAINQUADMON1)
                        {
                            if ( false == m_quadEnabled )
                            {
                                if (m_dragImage != NULL)
                                {
                                    m_dragImage->DragLeave(NULL);
                                    m_dragImage->EndDrag();
                                }
                                enableQuad();
                                if (m_dragImage != NULL)
                                {
                                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                                    m_dragImage->DragEnter(NULL,pt);
                                }
                            }
                        }
                        // m_prvHilitedMonitorID and m_hilitedMonitorID are reinitialized here.
                        if ((m_quadEnabled == false) && (m_hilitedControlID >= IDC_TRAINQUADMON1) )
                        {
                            m_prvHilitedControlID = 0;

                        }
                        else if ((m_singleEnabled == false) && (m_hilitedControlID < IDC_TRAINQUADMON1) )
                        {
                            m_prvHilitedControlID = 0;
                        }
                        else
                        {
                            m_prvHilitedControlID = m_hilitedControlID;
                        }

                        if ( 0 != m_prvHilitedControlID )
                        {
                            m_normalisationPerformed = FALSE;
                        }
                        m_hilitedControlID = m_monitors[i]->GetDlgCtrlID();
                        m_hilitingPerformed = FALSE;

                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    else if ( m_monitors[i]->GetDlgCtrlID() == m_hilitedControlID )
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


        BOOL TrainborneControllerDlg::isOverTrainCam(CPoint& pt)
        {
            FUNCTION_ENTRY( "isOverTrainCam" );

            CRect wndRect;
            for ( unsigned int i = 0; i < m_trainCameras.size(); i++ )
            {
                m_trainCameras[i]->GetWindowRect( &wndRect );
                ScreenToClient( &wndRect );

                if( wndRect.PtInRect( pt ) )
                {
                    if ( m_trainCameras[i]->GetDlgCtrlID() != m_hilitedCamControlID )
                    {
                        if ((m_singleRadioButton.GetCheck() == 1) && (m_hilitedControlID >= IDC_TRAINQUADMON1) )
                        {
                            m_prvHilitedControlID = 0;
                        }
                        else if ((m_quadRadioButton.GetCheck() == 1) && (m_hilitedControlID < IDC_TRAINQUADMON1) )
                        {
                            m_prvHilitedControlID = 0;
                        }
                        else
                        {
                            m_prvHilitedControlID = m_hilitedControlID;
                        }
                        if ( 0 != m_prvHilitedControlID )
                        {
                            m_normalisationPerformed = FALSE;
                        }
                        m_prvHilitedCamControlID = m_hilitedCamControlID;
                        m_hilitedCamControlID = m_trainCameras[i]->GetDlgCtrlID();
                        m_camHilitingPerformed = FALSE;
                        if ( m_prvHilitedCamControlID != 0 )
                        {
                            m_camNormalisationPerformed = FALSE;
                        }

                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    else if ( m_trainCameras[i]->GetDlgCtrlID() == m_hilitedCamControlID )
                    {
                        // still over traincam
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                }
            }

            FUNCTION_EXIT;
            return FALSE;
        }


        void TrainborneControllerDlg::OnMouseMove(UINT nFlags, CPoint point)
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
                BOOL overMonitor = isOverMonitor( point );
                if( FALSE == overMonitor )
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

                if ( FALSE == overMonitor )
                {
                    if ( m_singleEnabled && ( m_singleRadioButton.GetCheck() == 0 ) )
                    {
                        disableSingle();
                    }
                    if ( m_quadEnabled && ( m_quadRadioButton.GetCheck() == 0 ) )
                    {
                        disableQuad();
                    }
                }
            }
            CDialog::OnMouseMove(nFlags, point);

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::invalidateControlLabels()
        {
            FUNCTION_ENTRY( "invalidateControlLabels" );

            if ( ( m_normalisationPerformed && m_hilitingPerformed ) )//&&
                //(FALSE == m_mouseTracking ) )
            {
                //nothing to do
                FUNCTION_EXIT;
                return;
            }

            // Declare an iterator for the vector.
			std::vector< boost::shared_ptr< CLabel > >::iterator theIterator;

            // Iterate through the vector and invalidate both of the appropriate labels.
            int i = 0;
            int diff = (IDC_SINGLEMON_TEXT - IDC_TRAINSINGLEMON);
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


        void TrainborneControllerDlg::paintHilitedControl()
        {
            FUNCTION_ENTRY( "paintHilitedControl" );

            m_hilitingPerformed = TRUE;
			std::vector< boost::shared_ptr< CTransparentImage > >::iterator theIterator;

            for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
            {
                if( ( *theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                {
                    if (m_hilitedControlID == IDC_TRAINSINGLEMON)
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_SINGLEVMON_FORTRAIN_HILITE ) ) );
                    }
                    else
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_QUADVMON_HILITE ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::paintStandardControl()
        {
            FUNCTION_ENTRY( "paintStandardControl" );

            m_normalisationPerformed = TRUE;
            std::vector< boost::shared_ptr< CTransparentImage > >::iterator theIterator;

            for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
            {
                if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedControlID )
                {
                    if (m_prvHilitedControlID == IDC_TRAINSINGLEMON)
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_SINGLEVMON_FORTRAIN ) ) );
                    }
                    else
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_QUADVMON ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::paintHilitedCamControl()
        {
            FUNCTION_ENTRY( "paintHilitedCamControl" );

            m_camHilitingPerformed = TRUE;
            std::vector< boost::shared_ptr< CTransparentImage > >::iterator theIterator;

            for ( theIterator = m_trainCameras.begin(); theIterator != m_trainCameras.end(); theIterator++ )
            {
                if( ( *theIterator )->GetDlgCtrlID() == m_hilitedCamControlID )
                {
                    if ((m_hilitedCamControlID%2) == 0)
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_TRAINCAMLEFT_HILITE ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                    else
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_TRAINCAMRIGHT_HILITE ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::paintStandardCamControl()
        {
            FUNCTION_ENTRY( "paintStandardCamControl" );

            m_camNormalisationPerformed = TRUE;
            std::vector< boost::shared_ptr< CTransparentImage > >::iterator theIterator;

            for ( theIterator = m_trainCameras.begin(); theIterator != m_trainCameras.end(); theIterator++ )
            {
                if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedCamControlID )
                {
                    if ((m_prvHilitedCamControlID % 2) == 0)
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_TRAINCAMLEFT ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                    else
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_TRAINCAMRIGHT ) ) );

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnPaint()
        {
            FUNCTION_ENTRY( "OnPaint" );

            CPaintDC dc(this); // device context for painting

            // TODO: Add your message handler code here

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

            // Paint non-hilited control.
            if ( ( m_prvHilitedCamControlID != 0 ) && ( m_camNormalisationPerformed == FALSE ) )
            {
                paintStandardCamControl();
            }

            // Paint hilited control.
            if ( ( m_hilitedCamControlID != 0 ) && ( m_camHilitingPerformed == FALSE ) )
            {
                paintHilitedCamControl();
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnLButtonUp(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnLButtonUp" );

            CDialog::OnLButtonUp(nFlags, point);

            // Release mouse and load standard arrow cursor.
            if ( TRUE == m_mouseTracking )
            {
                ::ReleaseCapture();
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
                            if ( m_hilitedControlID < IDC_TRAINQUADMON1 )
                            {
                                m_singleRadioButton.SetCheck(1);
                                Singleradio(false);
                            }
                            else
                            {
                                m_quadRadioButton.SetCheck(1);
                                Quadradio(false);
                            }

                            m_labels[i]->SetText(m_camTextLabels[m_hilitedCamControlID - IDC_TRAINCAM1 + 1]);
                            m_applyButton.EnableWindow(TRUE);
                        }
                    }
                    m_prvHilitedCamControlID = m_hilitedCamControlID;
                    m_hilitedCamControlID = 0;
                    if (m_prvHilitedCamControlID != 0)
                    {
                        m_camNormalisationPerformed = FALSE;
                        updateDialog();
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::setDragImage(CImageList* dragImage)
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


        CImageList* TrainborneControllerDlg::CreateDragImage()
        {
            FUNCTION_ENTRY( "CreateDragImage" );

            BITMAP bm;
            CBitmap bitmap;
            if (m_hilitedCamControlID % 2 == 0)
            {
                bitmap.LoadBitmap(IDB_TRAINCAMLEFT);
            }
            else
            {
                bitmap.LoadBitmap(IDB_TRAINCAMRIGHT);
            }
            bitmap.GetBitmap(&bm);
            // Create imagelist
            CImageList* pImageList = new CImageList;
            pImageList->Create(bm.bmWidth, bm.bmHeight,
                ILC_COLOR | ILC_MASK, 0, 1);
            pImageList->Add(&bitmap, RGB(255, 0, 255)); // mask
            FUNCTION_EXIT;
            return pImageList;
        }


        void TrainborneControllerDlg::OnSelchangeSequencecombo()
        {
            FUNCTION_ENTRY( "OnSelchangeSequencecombo" );

            // TODO: Add your control notification handler code here
            m_applyButton.EnableWindow(TRUE);

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::OnTcApplyButton()
        {
            FUNCTION_ENTRY( "OnTcApplyButton" );

            if ( 1 == m_singleRadioButton.GetCheck() )
            {
                CString camText1("");
                m_labels[ 0 ]->CLabel::GetWindowText(camText1);

                try
                {
                    m_trainCctvCorbaProxy->setSingleScreen( (m_trainDetails->trainID%0x0100),
                                                            static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(getCamIndex(camText1)%0x0100),
                                                            TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                }
                catch( TA_Base_Core::TransactiveException& e)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << e.what();
                    UINT selectedButton = userMsg.showMsgBox( IDS_UE_090084 );
                }
            }
            else if ( 1 == m_quadRadioButton.GetCheck() )
            {
                CString camText1("");
                CString camText2("");
                CString camText3("");
                CString camText4("");
                m_labels[ 1 ]->CLabel::GetWindowText(camText1);
                m_labels[ 2 ]->CLabel::GetWindowText(camText2);
                m_labels[ 3 ]->CLabel::GetWindowText(camText3);
                m_labels[ 4 ]->CLabel::GetWindowText(camText4);

                unsigned short topLeft = (camText1.IsEmpty()) ? 0 : getCamIndex(camText1);
                unsigned short topRight= (camText2.IsEmpty()) ? 0 : getCamIndex(camText2);
                unsigned short bottomLeft = (camText3.IsEmpty()) ? 0 : getCamIndex(camText3);
                unsigned short bottomRight = (camText4.IsEmpty()) ? 0 : getCamIndex(camText4);

                try
                {
                    m_trainCctvCorbaProxy->setQuadScreen( (m_trainDetails->trainID%0x0100),
                                                          static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(topLeft%0x0100),
                                                          static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(topRight%0x0100),
                                                          static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(bottomLeft%0x0100),
                                                          static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(bottomRight%0x0100),
                                                          TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                }
                catch( TA_Base_Core::TransactiveException& e)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << e.what();
                    UINT selectedButton = userMsg.showMsgBox( IDS_UE_090085 );
                }
            }
            else
            {
                unsigned short sequenceNum = static_cast<unsigned short>(m_sequenceComboBox.GetCurSel());
                sequenceNum++;
                try
                {
                    m_trainCctvCorbaProxy->setSequence( (m_trainDetails->trainID%0x0100),
                                                        static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber>(sequenceNum%0x0100),
                                                        TA_IRS_Bus::ProtocolCctvTypes::SequenceForward,
                                                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                }
                catch( TA_Base_Core::TransactiveException& e)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << e.what();
                    UINT selectedButton = userMsg.showMsgBox( IDS_UE_090086 );
                }
            }

            m_applyButton.EnableWindow(FALSE);

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::enableSingle()
        {
            FUNCTION_ENTRY( "enableSingle" );

            m_singleEnabled = true;
            m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                    MAKEINTRESOURCE( IDB_SINGLEVMON_FORTRAIN ) ) );
            updateDialog();

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::enableQuad()
        {
            FUNCTION_ENTRY( "enableQuad" );

            m_quadEnabled = true;
            for (int i = 1; i < 5; i++)
            {
                m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                        MAKEINTRESOURCE( IDB_QUADVMON ) ) );
            }
            updateDialog();

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::disableSingle()
        {
            FUNCTION_ENTRY( "disableSingle" );

            m_singleEnabled = false;
            // Create clearing bitmap
            // Create bitmap
            CClientDC    dc (this);
            CDC         memDC;
            if(!memDC.CreateCompatibleDC(&dc))
            {
                FUNCTION_EXIT;
                return;
            }

            CBitmap bitmap;
            BITMAP bm;
            bitmap.LoadBitmap(IDB_SINGLEVMON_DISABLED);
            bitmap.GetBitmap(&bm);
            CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
            memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
            memDC.SelectObject( pOldMemDCBitmap );
            m_sequenceComboBox.EnableWindow(FALSE);
            m_labels[0]->SetText("");
            m_monitors[0]->SetBitmap( bitmap );
            updateDialog();
            m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                        MAKEINTRESOURCE( IDB_SINGLEVMON_DISABLED ) ) );
            updateDialog();

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::disableQuad()
        {
            FUNCTION_ENTRY( "disableQuad" );

            m_quadEnabled = false;
            // Create clearing bitmap
            // Create bitmap
            CClientDC    dc (this);
            CDC         memDC;
            if(!memDC.CreateCompatibleDC(&dc))
            {
                FUNCTION_EXIT;
                return;
            }

            CBitmap bitmap;
            BITMAP bm;
            bitmap.LoadBitmap(IDB_QUADVMON_DISABLED);
            bitmap.GetBitmap(&bm);
            CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
            memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
            memDC.SelectObject( pOldMemDCBitmap );
			int i = 1;
            for (; i < 5; i++)
            {
                m_labels[i]->SetText("");
                m_monitors[i]->SetBitmap( bitmap );
            }
            updateDialog();
            for (i = 1; i < 5; i++)
            {
                m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                        MAKEINTRESOURCE( IDB_QUADVMON_DISABLED ) ) );
            }
            updateDialog();

            FUNCTION_EXIT;
        }


        unsigned short TrainborneControllerDlg::getCamIndex( const CString& camText )
        {
            FUNCTION_ENTRY( "getCamIndex" );

            unsigned short i(0);
            for ( ; i < m_camTextLabels.size(); i++ )
            {
                if ( camText == m_camTextLabels[ i ] )
                {
                    FUNCTION_EXIT;
                    return i;
                }
            }

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT TrainborneControllerDlg::onStageUpdated(WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "onStageUpdated" );

            // lParam holds the DisplayItem* that was updated
            DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
            TA_ASSERT(item != NULL, "Sequence update received for NULL display item");
            VideoInput* input = dynamic_cast<VideoInput*>(item);
            TA_ASSERT(input != NULL, "Sequence update received for NULL display item");

            VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
            TA_ASSERT(application != NULL, "Application is NULL");

            if (input == m_bvsStage)
            {
                updateState();
            }

            FUNCTION_EXIT;
            return 0;
        }


        void TrainborneControllerDlg::OnSetFocus(CWnd* pOldWnd)
        {
            FUNCTION_ENTRY( "OnSetFocus" );

            CDialog::OnSetFocus(pOldWnd);
            updateState();

            FUNCTION_EXIT;
        }


        void TrainborneControllerDlg::updateState()
        {
            FUNCTION_ENTRY( "updateState" );

            // the current stage was updated
            m_applyButton.EnableWindow(FALSE);

            //we need to iniatialise this window to what is currently displaying on
            //the train

            switch(m_bvsStage->getActiveVideoSource())
            {
                case BvsStage::Single:
                    disableQuad();
                    enableSingle();
                    m_sequenceComboBox.EnableWindow(FALSE);
                    m_quadRadioButton.SetCheck(0);
                    m_singleRadioButton.SetCheck(1);
                    m_sequenceRadioButton.SetCheck(0);
                    Singleradio(false);
                    m_labels[0]->SetText(m_camTextLabels[m_bvsStage->getAssignment1()]);

                    break;
                case BvsStage::Quad:
                    enableQuad();
                    disableSingle();
                    m_sequenceComboBox.EnableWindow(FALSE);
                    m_quadRadioButton.SetCheck(1);
                    m_singleRadioButton.SetCheck(0);
                    m_sequenceRadioButton.SetCheck(0);
                    Quadradio(false);
                    m_labels[1]->SetText(m_camTextLabels[m_bvsStage->getAssignment1()]);
                    m_labels[2]->SetText(m_camTextLabels[m_bvsStage->getAssignment2()]);
                    m_labels[3]->SetText(m_camTextLabels[m_bvsStage->getAssignment3()]);
                    m_labels[4]->SetText(m_camTextLabels[m_bvsStage->getAssignment4()]);
                    break;
                case BvsStage::Sequence:
                    disableQuad();
                    disableSingle();
                    m_sequenceComboBox.EnableWindow(TRUE);
                    m_quadRadioButton.SetCheck(0);
                    m_singleRadioButton.SetCheck(0);
                    m_sequenceRadioButton.SetCheck(1);
                    OnSequenceradio();
                    m_sequenceComboBox.SetCurSel(m_bvsStage->getAssignment1() - 1);
                    break;
                case BvsStage::None:
                    disableQuad();
                    disableSingle();
                    m_quadRadioButton.SetCheck(0);
                    m_singleRadioButton.SetCheck(0);
                    m_sequenceRadioButton.SetCheck(0);
                    m_sequenceComboBox.EnableWindow(FALSE);
                    break;
            }
            m_applyButton.EnableWindow(FALSE);

            updateDialog();

            FUNCTION_EXIT;
        }

		
		//bohong++
        void TrainborneControllerDlg::Singleradio(bool doResetCam)
        {
            FUNCTION_ENTRY( "Singleradio" );

            // TODO: Add your control notification handler code here
            if(m_singleRadioButton.GetCheck() == 1)
            {
                m_singleEnabled = true;
                m_quadEnabled = false;

                if ( (doResetCam == false) || (m_sequenceRadioButton.GetCheck() == 1) || (m_quadRadioButton.GetCheck() == 1) || (m_firstSelection == TRUE))
                {
                    m_firstSelection = FALSE;
                    m_sequenceRadioButton.SetCheck(0);
                    m_quadRadioButton.SetCheck(0);
                    m_applyButton.EnableWindow(FALSE);
                    if ( doResetCam )
                    {
                        m_hilitedControlID = 0;
                        m_prvHilitedCamControlID = m_hilitedCamControlID;
                        m_hilitedCamControlID = 0;
                        if (m_prvHilitedCamControlID != 0)
                        {
                            m_camNormalisationPerformed = FALSE;
                        }
                    }
                    // Create clearing bitmap
                    // Create bitmap
                    CClientDC    dc (this);
                    CDC         memDC;
                    if(!memDC.CreateCompatibleDC(&dc))
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                    CBitmap bitmap;
                    BITMAP bm;
                    bitmap.LoadBitmap(IDB_QUADVMON_DISABLED);
                    bitmap.GetBitmap(&bm);
                    CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );

                    m_sequenceComboBox.EnableWindow(FALSE);
					int i = 1;
                    for (; i < 5; i++)
                    {
                        m_labels[i]->SetText("");
                        m_monitors[i]->SetBitmap( bitmap );
                    }
                    updateDialog();

                    for (i = 1; i < 5; i++)
                    {
                        m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_QUADVMON_DISABLED ) ) );
                    }
                    if ( doResetCam )
                    {
                        m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                    MAKEINTRESOURCE( IDB_SINGLEVMON_FORTRAIN ) ) );
                    }
                    updateDialog();
                }
            }

            FUNCTION_EXIT;
        }

		
		//bohong++
        void TrainborneControllerDlg::Quadradio(bool doResetCam)
        {
            FUNCTION_ENTRY( "Quadradio" );

            if(m_quadRadioButton.GetCheck() == 1)
            {
                // TODO: Add your control notification handler code here
                m_singleEnabled = false;
                m_quadEnabled = true;
                if ( (doResetCam == false) || (m_sequenceRadioButton.GetCheck() == 1) || (m_singleRadioButton.GetCheck() == 1) || (m_firstSelection == TRUE))
                {
                    m_firstSelection = FALSE;
                    m_singleRadioButton.SetCheck(0);
                    m_sequenceRadioButton.SetCheck(0);
                    m_applyButton.EnableWindow(FALSE);
                    if ( doResetCam )
                    {
                        m_hilitedControlID = 0;
                        m_prvHilitedCamControlID = m_hilitedCamControlID;
                        m_hilitedCamControlID = 0;
                        if (m_prvHilitedCamControlID != 0)
                        {
                            m_camNormalisationPerformed = FALSE;
                        }
                    }
                    // Create clearing bitmap
                    // Create bitmap
                    CClientDC    dc (this);
                    CDC         memDC;
                    if(!memDC.CreateCompatibleDC(&dc))
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                    CBitmap bitmap;
                    BITMAP bm;
                    bitmap.LoadBitmap(IDB_SINGLEVMON_DISABLED);
                    bitmap.GetBitmap(&bm);
                    CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
                    memDC.FillSolidRect(0,0, bm.bmWidth, bm.bmHeight, ::GetSysColor(COLOR_3DFACE));
                    memDC.SelectObject( pOldMemDCBitmap );
                    m_sequenceComboBox.EnableWindow(FALSE);
                    m_labels[0]->SetText("");
                    m_monitors[0]->SetBitmap( bitmap );
                    updateDialog();
                    m_monitors[0]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_SINGLEVMON_DISABLED ) ) );
                    if ( doResetCam )
                    {
                        for (int i = 1; i < 5; i++)
                        {
                            m_monitors[i]->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                    MAKEINTRESOURCE( IDB_QUADVMON ) ) );
                        }
                    }
                    updateDialog();
                }
            }

            FUNCTION_EXIT;
        }


} // TA_IRS_App
