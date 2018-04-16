/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/QuadControllerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_QUADCONTROLLERDLG_H__CFC414B2_9766_4D7C_8B55_1DE4DCAF05AC__INCLUDED_)
#define AFX_QUADCONTROLLERDLG_H__CFC414B2_9766_4D7C_8B55_1DE4DCAF05AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuadControllerDlg.h : header file
//

#include "VideoSourcesTreeCtrl.h"
#include <vector>

class CLabel;
namespace TA_IRS_App
{
    // forward declaration
   // class Quad;
	class Monitor;
    class QuadControllerDlg : public CDialog
    {
    // Construction
    public:
        QuadControllerDlg(CWnd* pParent = NULL);   // standard constructor

        int DoModal(Monitor* monitor);//(Quad* quad);

        void setDragImage(CImageList* dragImage);

        void setDragAdjust(CPoint& dragAdjust) { m_dragAdjustPoint = dragAdjust; }

    // Dialog Data
        // {{AFX_DATA(QuadControllerDlg)
        enum { IDD = IDD_QUADCONTROLLER_DIALOG };
        CButton    m_applyButton;
        VideoSourcesTreeCtrl    m_cameraTreeCtrl;
        CString    m_quadNameDesc;
        // }}AFX_DATA

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(QuadControllerDlg)
        public:
        virtual BOOL DestroyWindow();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(QuadControllerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnOK();
        afx_msg void OnPaint();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
        afx_msg void OnQcApplyButton();
        afx_msg void OnDestroy();
        afx_msg LRESULT onCameraUpdated(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onQuadUpdated(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
    // Helper Functions
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
        BOOL isOverMonitor( CPoint& pt );

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


        void adjustApplyButton();

        void loadExistingSegmentAssociations();

    // State Members
        HICON m_hIcon;                                // Dialog icon.
        BOOL m_mouseTracking;                        // TRUE if mouse tracking & dragging.
        std::vector<  boost::shared_ptr< CStatic > > m_monitors;
        std::vector<  boost::shared_ptr< CLabel > > m_labels;

        short m_hilitedControlID;                    // Resource ID of hilited control.
        BOOL m_hilitingPerformed;
        short m_prvHilitedControlID;                // Resource ID of previous hilited control.
        BOOL m_normalisationPerformed;
        BOOL m_contextActive;
        CImageList* m_dragImage;
        CPoint m_dragAdjustPoint;

        // the current quad being edited - only valid while the dialog is being displayed
       // Quad* m_quad;
		Monitor* m_monitor;
        // the current set of camera assignments - to locally modify
        std::vector<Camera*> m_segmentAssociations;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_QUADCONTROLLERDLG_H__CFC414B2_9766_4D7C_8B55_1DE4DCAF05AC__INCLUDED_)
