/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This page shows the library versions at each location.
  */

#if !defined(AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_)
#define AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_

#include "Resource.h"
#include "LibraryVersionListCtrl.h"
#include "LibraryVersionMonitor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"


#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class REBProgressManager;

namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310 ~ added fwd decl.

    class LibraryVersionPage : public CDialog
    {
    public:
	    LibraryVersionPage(CWnd* pParent = NULL);

		~LibraryVersionPage();

		//haipeng added
        void Init();
		//haipeng added

        // Dialog Data
	    //{{AFX_DATA(LibraryVersionPage)
	    enum { IDD = IDD_VERSION_PAGE };
	    LibraryVersionListCtrl	m_subHeader;
	    LibraryVersionListCtrl	m_mainHeader;
	    CButton	m_upgradeISCS;
	    LibraryVersionListCtrl	m_stationVersionList;
	    CEdit	m_nextISCSVersion;
	    CEdit	m_currentISCSVersion;
    	//}}AFX_DATA


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
		afx_msg LRESULT OnLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnNextLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnCurrentLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnOCCLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:


        /** 
          * setupLists
          *
          * Sets up the list controls
          */
        void setupLists();

        /** 
          * populateLists
          *
          * Sets up the list controls
          */
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

		unsigned long getEntityKeyFromName( const std::string& entityName );


        // access rights
        bool m_canUpgradeISCS;

        // the status of the pre defined message library
        unsigned short m_currentLibraryVersion;
        unsigned short m_nextLibraryVersion;
        bool m_librariesAreSynchronised;

		// Map of library version datapoint entity key to current value
		std::map<unsigned long, unsigned short> m_libraryVersionInfo;

		// Entity keys of datapoints at each location
		std::map<unsigned long, LocationInfo> m_locationInfo;

		// Lock the map of datapoint values
		TA_Base_Core::ReEntrantThreadLockable m_versionInfoLock;

		std::string m_locationName;

        // TD11310 ~ added
        LibraryVersionMonitor* m_libraryVersionMonitor;
        STISPredefinedMessages* m_stisPredefinedMessages;
        //////////////////////////////////////////////////////////////////////////
        // =========
        typedef struct
        {
            std::string locationName;
            unsigned long locationkey;
            unsigned long currentVersionKey;
            unsigned long nextVersionkey;
        } LocationVerionInfo ;
        typedef std::vector<LocationVerionInfo> stationLibraryVersionList;
        void getLibraryVersion(const std::string& TTISStationLibraryVersionName, stationLibraryVersionList& libraryVersionList);
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_LIBRARYVERSIONPAGE_H__D9BF008C_18EC_4E07_9CC7_A7CEC21E65F0__INCLUDED_)
