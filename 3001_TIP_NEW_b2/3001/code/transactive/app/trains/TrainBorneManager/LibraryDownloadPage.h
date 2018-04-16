/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/LibraryDownloadPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
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
#include "core/data_access_interface/src/ILocation.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"


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
            CLibraryDownloadPage( CWnd* pParent = NULL ); // standard constructor


            void Init();

            /**
              * updateTrainList
              *
              * Updates the list of trains.
              *
              * @param action   (Add/update/delete)
              * @param trainDetails     The train information
              */
            void updateTrainList( TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                                  const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails );


            /**
              * updateTrainVersion
              *
              * Updates versions for items within the train list.
              *
              * @param trainId The train to update
              */
            void updateTrainVersion( unsigned int trainId );


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
                                               const std::string& columnName );

            afx_msg LRESULT OnUpdateDownloadList(WPARAM wParam, LPARAM lParam);
            /**
              * updateDownloadList
              *
              * When a train download starts or finishes, this is called.
              * This updates the status of existing downloads, or adds new ones, it is up to the opeator to clear the list.
              *
              * @param trainDownload The details of the train download that changed.
              */
            void updateDownloadList( const DownloadStatusDisplay& trainDownload );


            // Dialog Data
            //{{AFX_DATA(CLibraryDownloadPage)
            enum { IDD = IDD_LIBRARY_DOWNLOAD };
            CTabCtrl    m_stationTrainTab;
            LibraryVersionListCtrl    m_stationVersionList;
            //CButton    m_retryCheck;
            CEdit    m_nextISCSVersion;
            //CEdit    m_versionProgress;
            CEdit    m_currentISCSVersion;
            CurrentDownloadsListCtrl    m_downloadStatusList;
            DownloadTrainListCtrl    m_trainVersionList;
            //}}AFX_DATA


            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CLibraryDownloadPage)
        public:
            virtual BOOL PreTranslateMessage( MSG* pMsg );
        protected:
            virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
            //}}AFX_VIRTUAL

            // Implementation
        protected:

            // Generated message map functions
            //{{AFX_MSG(CLibraryDownloadPage)
            virtual BOOL OnInitDialog();
            afx_msg void OnSelectAllTrains();
			afx_msg void OnDeselectAllTrains();
            afx_msg void OnClearCompleted();
            afx_msg void OnDestroy();
            afx_msg void OnUpgradeIscs();
            afx_msg void OnDownloadNext();
            afx_msg void onUpgradeNext();
            afx_msg void onCancelDownload();

            afx_msg void onClickDownloadStatus( NMHDR* pNMHDR, LRESULT* pResult );
            afx_msg void onItemchangedDownloadStatus( NMHDR* pNMHDR, LRESULT* pResult );
            afx_msg void onClickTrainList( NMHDR* pNMHDR, LRESULT* pResult );
            afx_msg void onItemchangedTrainList( NMHDR* pNMHDR, LRESULT* pResult );
            afx_msg void onSelchangeStationTrainTab( NMHDR* pNMHDR, LRESULT* pResult );

			afx_msg void OnBnClickedHelp();
			afx_msg void OnBnClickedClose();
			//////////////////////////////////////////////////////////////////////////
			
            afx_msg LRESULT onRightsChanged( WPARAM wParam, LPARAM lParam );
            afx_msg LRESULT OnUpdateCurrentTTISVersion( WPARAM wParam, LPARAM lParam );
            afx_msg LRESULT OnUpdateNextTTISVersion( WPARAM wParam, LPARAM lParam );
            afx_msg LRESULT OnUpdateLibrariesSynchronised( WPARAM wParam, LPARAM lParam );

            afx_msg LRESULT OnUpdateInboundList(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnUpdateOutboundList(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnUpdateSelectionCombo(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnUpdateTrainVersion(WPARAM wParam, LPARAM lParam);
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
			inline bool isAtOccOrDpt()
			{
				return m_locationType == TA_Base_Core::ILocation::DPT || m_locationType == TA_Base_Core::ILocation::OCC;
			}
            /**
              * updatePercentComplete
              *
              * Updates the % complete box.
              *
              */
            //void updatePercentComplete();


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
              * enableCancelDownloadButton
              *
              * Enables buttons that depend on the current download list changing
              */
            void enableCancelDownloadButton();


            // the status of the pre defined message library
            unsigned long m_currentISCSLibraryVersion;
            unsigned long m_nextISCSLibraryVersion;
            bool m_librariesAreSynchronised;

            // the thread used to upgrade/download
            TrainDownloadManager* m_downloadManager;

            // the object that holds version data
            TrainVersionManager* m_versionManager;

            // train selection component
	        TA_IRS_Bus::MFCTrainSelectorGUI* m_mfcTrainSelector;
	        TA_IRS_Bus::TrainSelector* m_trainSelector;

            // access rights
            bool m_canUpgradeISCS;
            bool m_canDownload;
            bool m_canUpgrade;

            TTISPredefinedMessages* m_ttisPredefinedMessages;

			TA_Base_Core::ILocation::ELocationType m_locationType;

			unsigned long m_locationKey;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(AFX_LIBRARYDOWNLOADPAGE_H__6B1F502D_DBD0_461A_BD56_A8B75E6C58A5__INCLUDED_)
