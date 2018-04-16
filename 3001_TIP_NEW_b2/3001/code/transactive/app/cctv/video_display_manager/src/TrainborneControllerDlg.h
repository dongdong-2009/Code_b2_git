/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/TrainborneControllerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_TRAINBORNECONTROLLERDLG_H__1D78792E_5D8D_4129_B34B_F30C65968400__INCLUDED_)
#define AFX_TRAINBORNECONTROLLERDLG_H__1D78792E_5D8D_4129_B34B_F30C65968400__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainborneControllerDlg.h : header file
//

#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/BvsStage.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Label.h"
#include "app/cctv/video_display_manager/src/VideoSourcesListCtrl.h"
#include "app/cctv/video_display_manager/src/transparentimage.h"

#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"


/////////////////////////////////////////////////////////////////////////////
// TrainborneControllerDlg dialog

namespace TA_IRS_App
{


        class TrainborneControllerDlg : public CDialog
        {
        // Construction
        public:
            TrainborneControllerDlg(CWnd* pParent = NULL);   // standard constructor

            int DoModal(TA_IRS_Bus::TrainCctvCorbaProxyPtr& trainCctvCorbaProxy, VideoSourcesListCtrl::TrainDetails* trainDetails);

            void setDragImage(CImageList* dragImage);

            void setDragAdjust(CPoint& dragAdjust) { m_dragAdjustPoint = dragAdjust; }

        // Dialog Data
            //{{AFX_DATA(TrainborneControllerDlg)
            enum { IDD = IDD_TRAINBORNECONTROLLER };
            CButton    m_sequenceRadioButton;
            CButton    m_quadRadioButton;
            CButton    m_singleRadioButton;
            CComboBox    m_sequenceComboBox;
            CButton    m_applyButton;
            CString    m_trainName;
            //}}AFX_DATA


        // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(TrainborneControllerDlg)
            protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            //}}AFX_VIRTUAL

        // Implementation
        protected:

            // Generated message map functions
            //{{AFX_MSG(TrainborneControllerDlg)
            virtual BOOL OnInitDialog();
			afx_msg void OnSingleradioX(void);
            afx_msg void OnSingleradio(bool doResetCam = true);
            afx_msg void OnSequenceradio();
			afx_msg void OnQuadradioX(void);
            afx_msg void OnQuadradio(bool doResetCam = true);
            afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            afx_msg void OnPaint();
            afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
            afx_msg void OnDestroy();
            afx_msg void OnSetFocus(CWnd* pOldWnd);
            afx_msg void OnSelchangeSequencecombo();
            afx_msg void OnTcApplyButton();
            afx_msg LRESULT onStageUpdated(WPARAM wParam, LPARAM lParam);
            //}}AFX_MSG
            DECLARE_MESSAGE_MAP()

        private:
        // Helper Functions
			//bohong++
			void Singleradio(bool doResetCam = true);
			void Quadradio(bool doResetCam = true);

            /**
            * UpdateDialog
            *
            * Sends a WM_PAINT message to the Vidiwall dialog.
            */
            void updateDialog();

            /**
            * RegisterMonitors
            *
            * Registers all Vidiwall Monitors on the dialog.
            */
            void registerMonitors();

            /**
            * RegisterMonitors
            *
            * Registers all Vidiwall Monitors on the dialog.
            */
            void registerTrainCams();


            /**
            * RegisterRecordingDevices
            *
            * Registers all Vidiwall RecordingDevices on the dialog.
            */
            void registerLabels();

          /**
            * InvalidateControlLabels
            *
            * Invalidates the control labels associated with
            * the previous and currently highlighted monitors.
            */
            void invalidateControlLabels();

            /**
            * IsOverMonitor
            *
            * TRUE if the mouse is over a monitor.
            */
            BOOL isOverMonitor( CPoint& pt, int first = 0, int last = 5 );

            /**
            * IsOverTrainCam
            *
            * TRUE if the mouse is over a monitor.
            */
            BOOL isOverTrainCam( CPoint& pt );

            /**
            * PaintStandardControl
            *
            * Paints the non-hilited bitmap in the control that lost focus.
            */

            void paintStandardControl();

          /**
            * PaintHilitedControl
            *
            * Paints the hilited bitmap in the control that lost focus.
            */

            void paintHilitedControl();

            /**
            * PaintStandardControl
            *
            * Paints the non-hilited bitmap in the control that lost focus.
            */

            void paintStandardCamControl();

          /**
            * PaintHilitedControl
            *
            * Paints the hilited bitmap in the control that lost focus.
            */

            void paintHilitedCamControl();

            unsigned short getCamIndex( const CString& camText );

            CImageList* CreateDragImage();

            void updateState();


        // State Members
            HICON m_hIcon;                                // Dialog icon.
            BOOL m_mouseTracking;                        // TRUE if mouse tracking & dragging.
            std::vector<  boost::shared_ptr< CTransparentImage > > m_monitors;
            std::vector<  boost::shared_ptr< CTransparentImage > > m_trainCameras;
            std::vector<  boost::shared_ptr< CLabel > > m_labels;
            std::vector< CString > m_camTextLabels;
            short m_hilitedControlID;                    // Resource ID of hilited control.
            short m_hilitedCamControlID;
            BOOL m_hilitingPerformed;
            BOOL m_camHilitingPerformed;
            short m_prvHilitedControlID;                // Resource ID of previous hilited control.
            short m_prvHilitedCamControlID;
            BOOL m_normalisationPerformed;
            BOOL m_camNormalisationPerformed;
            BOOL m_contextActive;
            BOOL m_firstSelection;
            CImageList* m_dragImage;
            CPoint m_dragAdjustPoint;
            bool m_singleEnabled;
            bool m_quadEnabled;

            void enableSingle();
            void enableQuad();
            void disableSingle();
            void disableQuad();

            VideoSourcesListCtrl::TrainDetails* m_trainDetails;
            BvsStage* m_bvsStage;

            TA_IRS_Bus::TrainCctvCorbaProxyPtr m_trainCctvCorbaProxy;
        };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} //TA_IRS_App

#endif // !defined(AFX_TRAINBORNECONTROLLERDLG_H__1D78792E_5D8D_4129_B34B_F30C65968400__INCLUDED_)
