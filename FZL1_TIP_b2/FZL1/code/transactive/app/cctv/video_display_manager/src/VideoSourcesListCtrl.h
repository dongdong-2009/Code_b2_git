/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoSourcesListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * This is a modified train list control for the cctv list which had to be different
  */

#if !defined(VideoSourcesListCtrl_H)
#define VideoSourcesListCtrl_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"


namespace TA_IRS_Bus
{
    namespace TrainInformationTypes
    {
        struct TrainDetails;
    }
}


namespace TA_IRS_App
{

        using TA_Base_Bus::ListCtrlSelNoFocus;

        using TA_Base_Bus::AutoSortListCtrl;
//         using TA_Base_Bus::AutoSortListCtrl::SortInfo;
		typedef TA_Base_Bus::AutoSortListCtrl::SortInfo SortInfo;

    //    class BvsStage;

        class VideoSourcesListCtrl : public ListCtrlSelNoFocus,
                                     public TA_Base_Bus::ISortingCallback
        {
        public:


            VideoSourcesListCtrl();
            virtual ~VideoSourcesListCtrl();

            void Init();

            // the standard columns for the list control
            enum TrainListColumns
            {
                PV_SERVICE = 0,
                LOCATION = 1,
                STAGE = 2,
                STATUS = 3
            };

            enum AtsValid
            {
                Unknown,
                Invalid,
                Valid
            };

            struct TrainDetails
            {
                unsigned short  trainID;
                unsigned long   currentLocation;
                unsigned long   currentCctvZone;
                std::string     serviceNumber;
                bool            isMute;
                AtsValid        atsValid;
                bool            isLocalisationValid;

                TrainDetails()
                {
                    trainID = 0;
                    currentLocation = 0;
                    currentCctvZone = 0;
                    serviceNumber = "_";
                    isMute = false;
                    atsValid = Unknown;
                    isLocalisationValid = false;
                }

                // simple copy
                TrainDetails(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& in)
                {
                    trainID = in.trainId;
                    currentLocation = in.currentLocation;
                    currentCctvZone = in.currentCctvZone;
                    serviceNumber = in.serviceNumber;
                    isMute = in.isMute;
                    atsValid = ( in.atsValid ? Valid :Invalid );
                    isLocalisationValid = in.isLocalisationValid;
                }

                // assignment operator
                TrainDetails& operator=(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& rhs)
                {
                    this->trainID = rhs.trainId;
                    this->currentLocation = rhs.currentLocation;
                    this->currentCctvZone = rhs.currentCctvZone;
                    this->serviceNumber = rhs.serviceNumber;
                    this->isMute = rhs.isMute;
                    atsValid = ( rhs.atsValid ? Valid : Invalid );
                    this->isLocalisationValid = rhs.isLocalisationValid;

                    return *this;
                }

            };


            void setDetailsPointer(CEdit* pDetailsEdit) {m_pDetailsEdit = pDetailsEdit;}


           /**
             * updateTrainList
             *
             * Updates this train list control - based on the input parameters
             *
             * @param action the operation to be applied to the list
             *
             * @param trainDetails the train details relevant to the operation defined
             *          by the action argument - if applicable
             *
             */
            void updateTrainList( const TA_IRS_Bus::ITrainSelectorGUI::EAction& action,
                                  const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);


            /**
              * stageUpdated
              *
              * When a BVS Stage is updated, refresh all trains in the
              * stage and re-paint the active/pre-conflict colours
              *
              * @param stage    The Stage that changed
              */
    //        void stageUpdated(BvsStage* stage);


            /**
             * getSelectedTrainId
             *
             * @return train ID of the selected train in the list (0 if none)
             */
            unsigned short getSelectedTrainId() const;


            /**
             * getSelectedTrainDetails
             *
             * @return the train details structure of the selected train in the list
             *         (NULL if none)
             */
            TrainDetails* getSelectedTrainDetails();


            /**
              * getTrainDetails
              *
              * @param trainId the train we want details for
              *
              * @return the train details structure if the train is in the list
              *         (NULL if none)
              */
            TrainDetails* getTrainDetails(unsigned short trainId);

            /**
             * sortCallback
             *
             * User defined sorting function
             *
             * @return positive value if item lParam2 before lParam1, negative if lParam1 should
             *      come first
             *
             * @param lParam1, lParam2 user defined values passed in from sort operation (app specific)
             * @param columnIndex the index of the column being sorted
             * @param currentSort defines the type of sorting specified (ie ascending or descending)
             */
            virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort);


            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(VideoSourcesListCtrl)
            protected:
            virtual void PreSubclassWindow();
            //}}AFX_VIRTUAL


        // Generated message map functions
        public:

            CImageList*  CreateDragImageEx(int nItem, LPPOINT lpPoint);

            //{{AFX_MSG(VideoSourcesListCtrl)
            afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
            afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
            afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
            afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
            afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
            afx_msg void OnSetFocus(CWnd* pOldWnd);
            afx_msg void OnKillFocus(CWnd* pNewWnd);
            afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
            afx_msg void onItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
            //}}AFX_MSG

            DECLARE_MESSAGE_MAP()


        protected:


            /**
             * findTrainIndex
             *
             * Finds the index of the specified train in this list
             *
             * @return the index of the train, or -1 if not found
             */
            int findTrainIndex(unsigned short trainId) const;


            /**
              * isSelectable
              *
              * Implementation of the parent class function
              *
              * @param rowNumber the row to test.
              *
              * @return whether to allow selection or not.
              */
            virtual bool isSelectable(int rowNumber);


        private:

            /**
             * deleteFromList
             *
             * Deletes the specified train from the list (based on its train ID)
             *
             * @param td the train we want to delete from list
             *
             *
             */
            void deleteFromList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td);


            /**
              * deleteAllTrains
              *
              * Delete all trains from the list
              */
            void deleteAllTrains();


            /**
             * addToList
             *
             * Adds the specified train to this list
             *
             * @param td the train we want to add to the list
             *
             */
            void addToList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td);


            /**
             * updateList
             *
             * Updates the specified train in the list (based on its train ID)
             *
             * @param td the train we want to update in the list
             *
             */
            void updateList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td);


            /**
              * trainIsInCurrentStage
              *
              * this will check if the train is in the current stage
              * if not it should not be in the list.
              *
              * @param td   the train to add/update
              *
              * @return false only if the train should not be displayed
              */
            bool trainIsInCurrentStage(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td);


            /**
             * getLocationString
             *
             * @return the string representation of what should be displayed in the location column of this list
             *
             */
            CString getLocationString(const TrainDetails& td) const;


            /**
             * getServiceString
             *
             * @return the string representation of what should be displayed in the service column of this list
             *
             */
            CString getPvServiceString(const TrainDetails& td) const;


            /**
             * getStageString
             *
             * @return the string representation of what should be displayed in the stage column of this list
             */
       //     CString getStageString(const TrainDetails& td) const;

            // TD16902
            /**
             * getStageNumber
             *
             * @return the string representation of what should be displayed in the detail edit
             */
            CString getStageNumber(const TrainDetails& td) const;
            // TD16902

            /**
             * getStatusString
             *
             * @return the string representation of what should be displayed in the status column of this list
             */
         //   CString getStatusString(const TrainDetails& td) const;


            /**
              * handleSelectionChanged
              *
              * Called to populate the details edit.
              */
            void handleSelectionChanged();


            // current sort information
            SortInfo m_sortInfo;

            // the custom header control
            TA_Base_Bus::FixedHeaderCtrl    m_fixedHeaderCtrl;


            CImageList m_imageList;
            CEdit* m_pDetailsEdit;

     //       BvsStage* m_localStage;

        };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} //TA_IRS_App

#endif // !defined(VideoSourcesListCtrl_H)
