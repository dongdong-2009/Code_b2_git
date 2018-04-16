#pragma once
#include <afxwin.h>
#include <boost/function.hpp>
#include "app/cctv/video_display_manager/src/Camera.h"

namespace TA_IRS_App
{
	class Monitor;
    class CPTZButton : public CButton
    {
            //DECLARE_DYNAMIC(CPTZButton)

        public:
            CPTZButton();
            virtual ~CPTZButton();
            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CCoolButton)
			static void setPtzRequestInterval(int ptzRequestInterval);
            void setIcons ( UINT m_iconMouseUp );
			void setBtnDownActionHandler(boost::function<void (Camera*)> actionHandler);
			void setBtnUpActionHandler(boost::function<void (Camera*)> actionHandler);
			static void setActiveMonitor(Monitor* monitor);
			static Monitor* getActiveMonitor();
        protected:
            //{{AFX_MSG(CCoolButton)
            //  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            //  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
            afx_msg void OnLButtonUp ( UINT nFlags, CPoint point );
            afx_msg void OnLButtonDown ( UINT nFlags, CPoint point );
			afx_msg void OnTimer(UINT_PTR nIDEvent);
            DECLARE_MESSAGE_MAP()
        private:
			static int s_ptzRequestInterval; // Time unit is ms
			static Monitor* m_activeMonitor;
            HICON m_iconUp;
            HICON m_iconDown;
            HICON CreateDarkerIcon ( HICON hIcon );
            COLORREF DarkenColor ( COLORREF crColor, double dFactor );
			boost::function<void (Camera*)> m_btnDownActionHandler;
			boost::function<void (Camera*)> m_btnUpActionHandler;
    };
}


