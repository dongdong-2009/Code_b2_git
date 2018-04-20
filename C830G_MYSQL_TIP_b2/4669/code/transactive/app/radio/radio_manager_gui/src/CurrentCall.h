/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/CurrentCall.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CurrentCall.h: interface for the CCurrentCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURRENTCALL_H__ED650A05_656B_11D7_B165_0050BAB1D931__INCLUDED_)
#define AFX_CURRENTCALL_H__ED650A05_656B_11D7_B165_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioCurrentCallListCtrl.h"

#include <set>

class CCurrentCall  : public RadioCurrentCallListCtrl
{
	public:
    
		CCurrentCall();
		virtual ~CCurrentCall();
	
		static CCurrentCall* GetInstance(void);

		bool IsCallSelected();

		bool IsCallDisplayed(void);  // Indicates if a selected call is displayed.
		bool IsHalfDuplexCallSelected();

		CString GetID();
		CString GetTetraID();
		CString GetLocation();


		bool IsOnHold(void);
    
        /**
         * @return the destination TETRA ID of the specified call
         */
        CString GetDestinationTetraID(int callID);
                        
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurrentCall)
	protected:
	virtual void	PreSubclassWindow();
    BOOL onItemchangingCurrentCallList(NMHDR* pNMHDR, LRESULT* pResult);
    BOOL onItemDeleted(NMHDR* pNMHDR, LRESULT* pResult);
    LRESULT         onRefreshRequest(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCurrentCall)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

        virtual void onListRefreshed();
        virtual void Collapse(int nPos, int nItem);
	    virtual void removeCall(long callID);

	private:

        void    refreshSelectionConstraints();
		CString GetControlText(UINT CtrlID);
		void    SetListHeadings();
        void    onItemDeselected(int iItem);

	private:
        
		UINT							m_currentCallID;
		bool							m_callDisplayed;
//		CRadioSession::ERadioCallType	m_currentCallType;

		bool							m_headingsDisplayed;
		CImageList						m_il;

        bool                            m_internalStateChange;

        std::set<unsigned long>     m_callGroupsSelected;
        std::set<unsigned long>     m_prevCallGroupsSelected;
};

#endif // !defined(AFX_CURRENTCALL_H__ED650A05_656B_11D7_B165_0050BAB1D931__INCLUDED_)
