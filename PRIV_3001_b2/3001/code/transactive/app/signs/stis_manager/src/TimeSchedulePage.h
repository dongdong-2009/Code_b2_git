
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/stis_manager/src/LibraryVersionPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
  *
  * This page shows the library versions at each location.
  */

#if !defined(TIME_SCHEDULE_PAGE_H)
#define TIME_SCHEDULE_PAGE_H

#include "Resource.h"
#include "ValidatingEdit.h"
#include "TimeScheduleTimeControl.h"
#include "DisplayAttributesManager.h"
#include "RangeNumberEdit.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleAccessFactory.h"
#include <string>
#include <vector>

#pragma once

namespace TA_IRS_App
{
    class TimeSchedulePage: public CDialog
    {
    public:
	    TimeSchedulePage(CWnd* pParent = NULL);
        void Init();

        // Dialog Data
	    //{{AFX_DATA(LibraryVersionPage)

    protected:

	    enum { IDD = IDD_TIME_SCHEDULE_PAGE };
		//controlled by TimeScheduleTimeControl
        CComboBox m_priorityCombo;
        CallbackDateTimeCtrl m_startDate;
        CallbackDateTimeCtrl m_startTime;
        CallbackDateTimeCtrl m_endDate;
	    CallbackDateTimeCtrl m_endTime;
	    CRangeNumberEdit m_repeatInterval;
		CSpinButtonCtrl m_repeatIntervalSpin;

		//controlled by DisplayAttributesManager
        CComboBox    m_lcdFontColour;
        CComboBox       m_lcdFontSize;
		CStatic       m_displayTimeLabel;
        CDateTimeCtrl m_displayTime;
        CComboBox     m_ledFontSize;
        CComboBox     m_ledFontColour;
		CComboBox     m_scrollSpeed;
        CComboBox     m_justification;
        CallBackComboBox     m_displayMode;
		CStatic       m_justificationLabel;
		CStatic       m_scrollSpeedLabel;

		//controlled by self
		CListCtrl m_messageList;
		ValidatingEdit m_textEdit;
		CButton m_saveBtn;
		CButton m_editBtn;
		CButton m_downLoadBtn;

		afx_msg LRESULT OnTimeScheduleDataCompleteLoad(WPARAM, LPARAM);
		afx_msg void OnEdit();
		afx_msg void OnSave();
		afx_msg void OnDownLoad();
		//called when m_messageList selection changed
		afx_msg void OnMessageSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnKillFocusRepeatInterval();
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL PreTranslateMessage(MSG* pMsg);

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(LibraryVersionPage)
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

	    // Generated message map functions
	    //{{AFX_MSG(LibraryVersionPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDestroy();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
		TimeScheduleTimeControl *m_timeAttributesControl;
		DisplayAttributesManager *m_displayAttributesControl;
		std::vector<TA_IRS_Core::StisTimeScheduleData*> m_timeScheduleMessags;
		//invalidate one message item, make it reload from database
		void invalidateMessageItem(unsigned int itemIndex);
		void loadAllMessages();
		void initMessageList();
		void resetAllcontrolValue(TA_IRS_Core::StisTimeScheduleData* data);
		void setAllDefaultValue();
		void resetItemImage(int nItem);
		bool saveAllControlValue(TA_IRS_Core::StisTimeScheduleData* data);
		//used by message list ctrl
		int m_displayOrder[2];
		CImageList m_imageList;

	    static const int MAX_FREE_TEXT_CHARACTERS;
	    static const CString VALID_CHARACTERS; 
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_)
