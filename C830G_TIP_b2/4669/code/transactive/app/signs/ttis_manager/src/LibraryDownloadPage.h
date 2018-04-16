/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/LibraryDownloadPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the message library download page.
  * It is located on the train download page
  *
  */

#if !defined(AFX_LIBRARYDOWNLOADPAGE_H__6B1F502D_DBD0_461A_BD56_A8B75E6C58A5__INCLUDED_)
#define AFX_LIBRARYDOWNLOADPAGE_H__6B1F502D_DBD0_461A_BD56_A8B75E6C58A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "CurrentDownloadsListCtrl.h"
#include "DownloadTrainListCtrl.h"
#include "LibraryVersionListCtrl.h"
#include "TrainDownloadManager.h"
#include "TrainVersionManager.h"


namespace TA_IRS_App
{
    class TTISPredefinedMessages;
}

namespace TA_IRS_App
{
    class CLibraryDownloadPage : public CDialog,
                                 public TA_IRS_Bus::ICustomTrainDataProvider
    {
    // Construction
    public:
	    CLibraryDownloadPage(CWnd* pParent = NULL);   // standard constructor


		void Init();
        /** 
          * setDownloadManager
          *
          * Sets the thread used to process upgrade and download requests.
          * This must be called before the tab is used.
          *
          * @param downloadManager a reference to the thread, must be valid for the life of this object
          */
        void setDownloadManager(TrainDownloadManager& downloadManager);


        /** 
              * setVersionManager
              *
              * Sets the object that contains version information.
              *
              * @param versionManager a reference to the object, must be valid for the life of this object
              */
            void setVersionManager(TrainVersionManager& versionManager);


            /**
              * updateTrainList
              *
              * Updates the list of trains.
              *
              * @param action   (Add/update/delete)
              * @param trainDetails     The train information
              */
            void updateTrainList(TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                                 const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);


            /**
              * updateTrainVersion
              *
              * Updates versions for items within the train list.
              *
              * @param trainId The train to update
              */
            void updateTrainVersion(unsigned int trainId);


            /**
              * getCustomData
              *
              * Return the value that should populate 'columnName' for
              * the given train.
              *
              * @param trainId  The train the data is needed for
              * @param columnName   The column to populate.
              */
            virtual std::string getCustomData( unsigned int trainId,
                                               const std::string& columnName);


            /**
              * updateDownloadList
              *
              * When a train download starts or finishes, this is called.
              * This updates the status of existing downloads, or adds new ones, it is up to the opeator to clear the list.
              *
              * @param trainDownload The details of the train download that changed.
              */
            void updateDownloadList(const DownloadStatusDisplay& trainDownload);


            // Dialog Data
            //{{AFX_DATA(CLibraryDownloadPage)
            enum { IDD = IDD_LIBRARY_DOWNLOAD };
            CTabCtrl    m_stationTrainTab;
            LibraryVersionListCtrl    m_stationVersionList;
            CButton    m_retryCheck;
            CEdit    m_nextISCSVersion;
            CEdit    m_versionProgress;
            CEdit    m_currentISCSVersion;
            CurrentDownloadsListCtrl    m_downloadStatusList;
            DownloadTrainListCtrl    m_trainVersionList;
            //}}AFX_DATA


            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CLibraryDownloadPage)
            public:
            virtual BOOL PreTranslateMessage(MSG* pMsg);
            protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            //}}AFX_VIRTUAL

        // Implementation
        protected:

            // Generated message map functions
            //{{AFX_MSG(CLibraryDownloadPage)
            virtual BOOL OnInitDialog();
            afx_msg void OnAllTrains();
            afx_msg void OnClearSelection();
            afx_msg void OnClearCompleted();
            afx_msg void OnDestroy();
            afx_msg LRESULT OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnUpdateNextTTISVersion(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnUpdateLibrariesSynchronised(WPARAM wParam, LPARAM lParam);
            afx_msg void OnUpgradeIscs();
            afx_msg void OnDownloadNext();
            afx_msg void onUpgradeNext();
            afx_msg void onSelectAllDownloads();
            afx_msg void onDeselectAlllDownloads();
            afx_msg void onStopAutoRetry();
            afx_msg void onClickDownloadStatus(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onItemchangedDownloadStatus(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onClickTrainList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onItemchangedTrainList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void onSelchangeStationTrainTab(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
            DECLARE_MESSAGE_MAP()


            /**
             * OnOK
             *
             * This method has been implemented to hide accidental calls when
             * the Enter key is pressed. It can be overriden with new behaviour if required.
             */
            virtual afx_msg void OnOK();

            /**
             * OnCancel
             *
             * This method has been implemented to hide accidental calls when
             * the ESC key is pressed. It can be overriden with new behaviour if required.
             */
            virtual afx_msg void OnCancel();


        private:

            /**
              * updatePercentComplete
              *
              * Updates the % complete box.
              *
              */
            void updatePercentComplete();


            /**
              * enableUpgradeIscsButton
              *
              * Based on a number of factors, enable the Upgrade ISCS button
              * if it should be enabled. Disable it if not.
              *
              * enable the upgrade ISCS button if
              * - this is an OCC MFT
              * - the libraries are synchronised across the stations
              * - The library hasnt been upgraded yet
              */
            void enableUpgradeIscsButton();


            /**
              * setTrainMode
              *
              * Hides the station version list and shows the
              * train version list.
              */
            void setTrainMode();


            /**
              * setStationMode
              *
              * Hides the train version list and shows the
              * station version list.
              */
            void setStationMode();


            /**
              * enableTrainListButtons
              *
              * Enables buttons that depend on the train list changing
              */
            void enableTrainListButtons();


            /**
              * enableDownloadListButtons
              *
              * Enables buttons that depend on the current download list changing
              */
            void enableDownloadListButtons();


            // the status of the pre defined message library
            unsigned short m_currentLibraryVersion;
            unsigned short m_nextLibraryVersion;
            bool m_librariesAreSynchronised;

            // the thread used to upgrade/download
            TrainDownloadManager* m_downloadManager;

            // the object that holds version data
            TrainVersionManager* m_versionManager;

            // access rights
            bool m_canUpgradeISCS;
            bool m_canDownload;
            bool m_canUpgrade;

            TTISPredefinedMessages* m_ttisPredefinedMessages;
        };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(AFX_LIBRARYDOWNLOADPAGE_H__6B1F502D_DBD0_461A_BD56_A8B75E6C58A5__INCLUDED_)
