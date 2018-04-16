/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/LibraryVersionPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This page shows the library versions at each location.
  */

#if !defined(AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_)
#define AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_

#include "Resource.h"
#include "LibraryVersionListCtrl.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/signs/tis_agent_access/src/ILibraryVersionListener.h"
#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Core
{
	class ILocation;
}

namespace TA_IRS_App
{
    class STISPredefinedMessages;
	class LibraryVersionPage : public CDialog, public TA_IRS_Bus::ILibraryVersionListener
    {
    public:
	    LibraryVersionPage(CWnd* pParent = NULL);
		~LibraryVersionPage();

        void Init();

        // Dialog Data
	    //{{AFX_DATA(LibraryVersionPage)
	    enum { IDD = IDD_VERSION_PAGE };
	    LibraryVersionListCtrl	m_subHeader;
	    LibraryVersionListCtrl	m_mainHeader;
	    CButton	m_upgradeISCS;
	    LibraryVersionListCtrl	m_stationVersionList;
    	//}}AFX_DATA
		virtual void libraryVersionUpdate( TA_IRS_Bus::LibraryVersionUpdateInfo libraryVersion );


    protected:

        // the columns for the list control
        enum VersionListColumns
        {
            Station = 0,
            CurrentISCS = 1,
            CurrentSTIS = 2,
            NextISCS = 3,
            NextSTIS = 4,
            Spare = 5
        };


        virtual afx_msg void OnOK();
        virtual afx_msg void OnCancel();

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(LibraryVersionPage)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

	    // Generated message map functions
	    //{{AFX_MSG(LibraryVersionPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDestroy();
        afx_msg void OnUpgradeIscs();
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnNextLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnCurrentLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnOCCLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnLibraryVersionUpdate(WPARAM updateInfo, LPARAM nouse);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:


		void initRightsCheck();
        void setupLists();
		int initLocationItems(TA_Base_Core::ILocation* location, int insertIndex);
		void populatePageData();


        /** 
          * enableUpgradeIscsButton
          *
          * Based on a number of factors, enable the Upgrade ISCS button
          * if it should be enabled. Disable it if not.
          *
          * enable the upgrade ISCS button if
          * - the libraries are synchronised across the stations
          * - The library hasnt been upgraded yet
          */
        void enableUpgradeIscsButton();

        // access rights
        bool m_canUpgradeISCS;

        // the status of the pre defined message library
        unsigned long m_currentLibraryVersion;
        unsigned long m_nextLibraryVersion;
        bool m_librariesAreSynchronised;

		// Lock the map of datapoint values
		TA_Base_Core::ReEntrantThreadLockable m_versionInfoLock;

		std::string m_locationName;

        STISPredefinedMessages* m_stisPredefinedMessages;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_)
