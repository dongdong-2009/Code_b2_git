/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioCallStackLibrary/src/RadioCallStackListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_)
#define AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioCallStackListCtrl.h : header file
//

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/IDL/src/IRadioCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"

/////////////////////////////////////////////////////////////////////////////
// CRadioCallStackListCtrl window

class CRadioCallStackListCtrl : 
    public TA_Base_Bus::ListCtrlSelNoFocus , 
    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
	public:

		// Types
		enum EStackType
		{
			IncomingStack,
			OthersStack,
			HeldStack
		};

		// Construction
		CRadioCallStackListCtrl(EStackType stackType);
		virtual ~CRadioCallStackListCtrl();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CRadioCallStackListCtrl)
		public:
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
		protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL
		afx_msg LRESULT onCallNotification(WPARAM wParam, LPARAM lParam);

	// Implementation
	public:
		void setRadioName(std::string radioName);
		void setCallTrainPermitted(bool canCallTrain);
		BOOL setColumnName(int nCol, char*  columnName);
		bool IsCallSelected();
		long getSelectedCallID();
        
        
        /** 
          * selectCall
          *
          * Select the call identified by the given call Id.
          * Clears any previous selection.
          *
          * @param callId   The call to select
          *
          * @return true if the call was present and is now selected.
          */
        bool selectCall(long callId);


        /** 
          * deselectAll
          *
          * Deselect any selected calls
          *
          */
        void deselectAll();


		/*
		* get a list of currently selected calls
		* supports single or multi select list controls.
		*/
		void getSelectedCallList(std::vector<long>&);

		// Generated message map functions
	protected:
		//{{AFX_MSG(CRadioCallStackListCtrl)
		//}}AFX_MSG

	private:

		void setupListControl();
		void startSubscription(void);
		void stopSubscription(void);
		void processUpdate(TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatus);
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

		bool loadCalls();
		void addCall(long callID);
		void removeCall(long callID);
		void updateCall(long callID,TA_Base_Bus::IRadioCorbaDef::ECallStatusType status, TA_Base_Bus::IRadioCorbaDef::ECallCategory category);
		const char* getStatusText(TA_Base_Bus::IRadioCorbaDef::ECallStatusType status, TA_Base_Bus::IRadioCorbaDef::ECallCategory category);

		TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
				TA_Base_Bus::IRadioCorbaDef_ptr,
				TA_Base_Bus::IRadioCorbaDef_var> m_radio;

		bool			m_bSubscribed;
		CImageList		m_ImageList;
		EStackType		m_stackType;
		unsigned long   m_radioEntityKey;
        unsigned long   m_radioEntityLocation;
		bool			m_isLoaded;
		bool            m_canCallTrain;
		unsigned long	m_consoleId;

		TA_Base_Core::ReEntrantThreadLockable  m_updateLock;

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_)
