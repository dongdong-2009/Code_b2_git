//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingWindowThread.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2012/10/08 17:44:33 $
// Last modified by:  $Author: qi.huang $
//
// A self-deleting, user interface thread used as the base for the
// Plan Controller and Plan Editor threads.

#if !defined(AFX_FLOATINGWINDOWTHREAD_H__1ACD756F_141B_4450_B4ED_E5D935BAE818__INCLUDED_)
#define AFX_FLOATINGWINDOWTHREAD_H__1ACD756F_141B_4450_B4ED_E5D935BAE818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CFloatingWindowThread

    class CFloatingWindowThread : public CWinThread
    {
        DECLARE_DYNAMIC(CFloatingWindowThread)

        CFloatingWindowThread(TreeNodeId nodeId);
        virtual ~CFloatingWindowThread();

        BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

    // Operations
    public:
        void SetNodeId(TreeNodeId nodeId);
        TreeNodeId GetNodeId() const;
        void ActivateWindow();
        void Terminate();

    protected:
        virtual CDocTemplate *GetDocumentTemplate() const = 0;

	private:
		void AdjustPosition();
		void ShowMyWindow();

    // Attributes
    private:
        CDocManager m_docManager;
        TreeNodeId m_nodeId;
        CEvent m_initEvent;

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CFloatingWindowThread)
    public:
        virtual BOOL InitInstance();
        virtual int ExitInstance();
        virtual BOOL OnIdle(LONG lCount);
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CFloatingWindowThread)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_FLOATINGWINDOWTHREAD_H__1ACD756F_141B_4450_B4ED_E5D935BAE818__INCLUDED_)
