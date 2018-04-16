/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/ScheduleDownloadPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the train time schedule download page.
  * It is on the train download tab.
  *
  */

#if !defined(AFX_SCHEDULEDOWNLOADPAGE_H__CDD0E251_DC95_45AA_9080_935CCC843A49__INCLUDED_)
#define AFX_SCHEDULEDOWNLOADPAGE_H__CDD0E251_DC95_45AA_9080_935CCC843A49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CurrentDownloadsListCtrl.h"
#include "DownloadTrainListCtrl.h"
#include "REBProgressManager.h"
#include "TrainDownloadManager.h"
#include "TrainTimeScheduleManager.h"
#include "TrainVersionManager.h"

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"


using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{

        class CScheduleDownloadPage : public CDialog,
                                      public TA_IRS_Bus::ICustomTrainDataProvider
        {

        public:

            CScheduleDownloadPage(CWnd* pParent = NULL);   // standard constructor

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
            //{{AFX_DATA(CScheduleDownloadPage)
            enum { IDD = IDD_SCHEDULE_DOWNLOAD };
            CButton    m_retryCheck;
            CEdit    m_versionProgress;
            CEdit    m_currentISCSVersion;
            ListCtrlSelNoFocus    m_scheduleList;
            CurrentDownloadsListCtrl m_downloadStatusList;
            DownloadTrainListCtrl    m_trainVersionList;
            //}}AFX_DATA


            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CScheduleDownloadPage)
            public:
            virtual BOOL PreTranslateMessage(MSG* pMsg);
            protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            //}}AFX_VIRTUAL

        // Implementation
        protected:

            // Generated message map functions
            //{{AFX_MSG(CScheduleDownloadPage)
            virtual BOOL OnInitDialog();
            afx_msg void OnAllTrains();
            afx_msg void OnClearSelection();
            afx_msg void OnClearCompleted();
            afx_msg void OnDownloadCurrent();
            afx_msg void OnUpgradeCurrent();
            afx_msg void OnSetCurrentSchedule();
            afx_msg void onSelchangeScheduleList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg LRESULT onUpdateCurrentTimeSchedule(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT onTimeScheduleChanged(WPARAM wParam, LPARAM lParam);
            afx_msg void OnDestroy();
            afx_msg void onSelectAllDownloads();
            afx_msg void onDeselectAlllDownloads();
            afx_msg void onStopAutoRetry();
            afx_msg void onItemchangedDownloadStatusList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onClickDownloadStatusList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onClickTrainList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onItemchangedTrainList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onClickScheduleList(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
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
              * loadTimeSchedules
              *
              * Loads the time schedule information from the database.
              *
              */
            void loadTimeSchedules();


            /**
              * updateCurrentVersion
              *
              * Gets the current time schedule version from the tis agent
              * and displays it in the current version box.
              *
              */
            void updateCurrentVersion();


            /**
              * scheduleSelectionChanged
              *
              * called to enable/disable the set current schedule button based
              * on what is selected, and operator rights.
              */
            void scheduleSelectionChanged();


            // the thread used to upgrade/download
            TrainDownloadManager* m_downloadManager;
            // the object that holds version data
            TrainVersionManager* m_versionManager;

            // the current time schedule in use
            unsigned short m_currentScheduleVersion;
            unsigned long m_currentScheduleKey;

            // the time schedule names and their pkeys in the database
            TrainTimeScheduleManager::TimeScheduleMap m_timeScheduleInformation;

            // access rights
            bool m_canSetCurrentSchedule;
            bool m_canDownload;
            bool m_canUpgrade;

            TrainTimeScheduleManager* m_trainTimeScheduleManager;
        };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(AFX_SCHEDULEDOWNLOADPAGE_H__CDD0E251_DC95_45AA_9080_935CCC843A49__INCLUDED_)
