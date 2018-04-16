#pragma once
#include "afxwin.h"
#include "IPTZCommandListener.h"
#include <boost/shared_ptr.hpp>

namespace TA_IRS_App
{

    class CPTZButton : public CButton
    {
            //DECLARE_DYNAMIC(CPTZButton)

        public:
            CPTZButton();
            virtual ~CPTZButton();

            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CCoolButton)
        public:
            //virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        protected:
            //virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

            virtual bool handleButtonUp () = 0;

            virtual bool handleButtonDown() = 0;

        public:

			void setCommandListener ( IPTZCommandListener* listener )
            {
                m_commandListener = listener;
            }

            void setIcons ( UINT m_iconMouseUp );




        protected:

            //{{AFX_MSG(CCoolButton)
            //  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            //  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

            afx_msg void OnLButtonUp ( UINT nFlags, CPoint point );
            afx_msg void OnLButtonDown ( UINT nFlags, CPoint point );



			IPTZCommandListener* m_commandListener;

            DECLARE_MESSAGE_MAP()

        private:
            HICON m_iconUp;
            HICON m_iconDown;

            HICON CreateDarkerIcon ( HICON hIcon );
            COLORREF DarkenColor ( COLORREF crColor, double dFactor );




    };
}


