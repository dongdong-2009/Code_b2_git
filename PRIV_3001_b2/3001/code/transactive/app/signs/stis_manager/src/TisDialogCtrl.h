#if !defined(TISDIALOGCTRL_H)
#define TISDIALOGCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>

namespace TA_IRS_App
{

    class CTisDialogCtrl : public CTabCtrl
    {
            // Construction
        public:
            CTisDialogCtrl();

            // Attributes
        public:
            CDialog*  getActiveDialog()
            {
                return m_activeDialog;
            }

            // Operations
        public:

            int SetCurSel( int nItem );

            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CTisDialogCtrl)
        protected:
            virtual void PreSubclassWindow();
            //}}AFX_VIRTUAL

            // Implementation
        public:
            virtual ~CTisDialogCtrl();

            // Generated message map functions
        protected:

            void AddPage( CDialog&  dlg,UINT TemplateId, CString& label );
            void ResizeDialogs();
		    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
            //{{AFX_MSG(CTisDialogCtrl)
            afx_msg BOOL OnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
            afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
            afx_msg void OnDestroy();
            afx_msg void OnMove( int x, int y );
            afx_msg void OnPaint();
            afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
            //}}AFX_MSG

        private:

            void CreateDialogs();
            void DisplayDialog( UINT tabPage );

            struct DialogInfo
            {
                CDialog*   pDlg;
                int        nTemplateID;
                CString    label;
            };

            std::vector<DialogInfo> m_dlgInfo;
            CDialog*    m_activeDialog;

            DECLARE_MESSAGE_MAP()


    };


///////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//}}AFX_INSERT_LOCATION

//#undef  AFX_DATA
//#define AFX_DATA

}
#endif // !defined(MFC_EXTENSION_TAB_DIALOG_CTRL_H)