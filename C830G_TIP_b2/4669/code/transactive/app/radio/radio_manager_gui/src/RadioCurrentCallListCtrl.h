/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioCurrentCallListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_RADIOCURRENTCALLLISTCTRL_H_INCLUDED)
#define AFX_RADIOCURRENTCALLLISTCTRL_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioCurrentCallListCtrl.h : header file
//

#include "bus\mfc_extensions\src\tree_list_control\TreeListCtrl.h"
#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include "CommsMessageCorbaDef.h"
#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioCorbaDef.h"
#include "core/naming/src/NamedObject.h"

#include <set>
#include <map>

/////////////////////////////////////////////////////////////////////////////
// RadioCurrentCallListCtrl window

class RadioCurrentCallListCtrl : public CTreeListCtrl , public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
	public:

        enum EColumnIndex
        {
            COL_TIME,
            COL_CALL,
            COL_NUMBER,
            COL_LOCATION,
            COL_STATUS,
            COL_INDEX_MAX
        };

        typedef std::set<long> SelectedCalls;

		// Construction
		RadioCurrentCallListCtrl();
		virtual ~RadioCurrentCallListCtrl();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(RadioCurrentCallListCtrl)
		public:
		protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL
		afx_msg LRESULT onCallNotification(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onPttNotification(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onInitNotification(WPARAM wParam, LPARAM lParam);

        virtual DWORD getForegroundColourForItem(int nItem);

	// Implementation
	public:
		void setRadioName(std::string radioName);
		void setSessionRef(long sessionRef);
		BOOL setColumnName(int nCol, char*  columnName);
		long getSelectedCallID();
        SelectedCalls getSelectedCallIDs();
		bool isSelectedCallHalfDuplex();

        // FORCE the subscriptions to be done once the train list
        // in the search page has been populated. This is after
        // the initial creation thread - so it posts a message to itself
        // effictively queueing up until after all the train updates are
        // processed
        void queueSubscriptions();


        /**
         * @return true if the call defined by the specified call ID is a half duplex call
         */
        bool isCallHalfDuplex(long callID);

		// Generated message map functions
	protected:
		//{{AFX_MSG(RadioCurrentCallListCtrl)
        afx_msg void OnDestroy();
		//}}AFX_MSG
        
        /// getRadioCorbaDef (simple accessor)
        TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
				TA_Base_Bus::IRadioCorbaDef_ptr,
				TA_Base_Bus::IRadioCorbaDef_var>& getRadioCorbaDef() { return m_radio; }

        /// Called whenever the list contents have been refreshed
        virtual void onListRefreshed() {}
		virtual void removeCall(long callID);

	private:
               
        struct CallData
        {
            CallData() : call(0) {}
            CallData(TA_Base_Bus::IRadioCorbaDef::CallDetailsType* callIn, std::string tsi) 
                : call(callIn), talkingTSI(tsi)     {}

            TA_Base_Bus::IRadioCorbaDef::CallDetailsType* call;
            std::string talkingTSI;
        };
        
        /// Refreshes the string information at the specified (virtual) row of this tree list
        /// for a child list item
        void refreshChildRow(int childVIndex, const TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant, const CallData& callInformation);
        /// Refreshes the string information at the specified (virtual) row of this
        /// tree list control (only used for conference calls)
        void refreshConferenceParentRow(int vIndex, const CallData& callInformation);

        /**
         * performs a full refresh of this list control (discarding all existing items)
         *  Saves the current selection details, in an attempt to retain the selection
         *  that existed prior to the refresh - also retains scroll position
         */
        void performListRefresh();
        
        /**
         * Inserts details of the specified call into this list control
         * @return the virtual index of this call into tree control (if it's a group
         *      call, the virtual index references the parent node)
         */
        int addCallToControl(const CallData& callInformation);

		void setupListControl();
		void startSubscription(void);
		void stopSubscription(void);
		void processUpdate(const TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatus);
		void processUpdate(const TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification& pttStatus);
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

		bool loadCalls();
        /**
         * @param isNew true if this call is being added to control (not being updated -
         *          note that an update involves deleting a call and re-adding it)
         * @param talkingTSI may be emptry string if unknown (in which case we wait for a
         *     PTT notification to come through
         */
		void addCall(long callID, bool isNew = true, std::string talkingTSI = "");
		void addCallGroupMembers(long callID);
		void updateCall(long callID);
		const char* getStatusText(TA_Base_Bus::IRadioCorbaDef::ECallStatusType status);

		TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
				TA_Base_Bus::IRadioCorbaDef_ptr,
				TA_Base_Bus::IRadioCorbaDef_var> m_radio;

		bool			m_bSubscribed;
		CImageList		m_ImageList;
		long			m_sessionRef;
		unsigned long   m_radioEntityKey;
        unsigned long   m_radioEntityLocation;
		bool            m_isLoaded;

		TA_Base_Core::ReEntrantThreadLockable  m_updateLock;

        // Stores cached versions of calls
        typedef std::map<long, CallData> CallMap;
        CallMap m_callMap;

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOCURRENTCALLLISTCTRL_H_INCLUDED)
