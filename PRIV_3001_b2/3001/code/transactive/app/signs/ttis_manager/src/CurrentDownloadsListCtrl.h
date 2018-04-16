/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/CurrentDownloadsListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  *
  * CurrentDownloadsListCtrl defines a list control that displays
  * train downloads.
  */

#if !defined(AFX_CurrentDownloadsListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
#define AFX_CurrentDownloadsListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_

#include "app/signs/ttis_manager/src/AbstractTrainDownloads.h"

#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

#include <vector>

//#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainInformationCorbaDef.h"//limin


namespace TA_IRS_App
{

    class CurrentDownloadsListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:

        // the columns for the list control
        enum DownloadListColumns
        {
            TRAIN = 0,
            TYPE = 1,
            STATUS = 2
        };

        CurrentDownloadsListCtrl();
        virtual ~CurrentDownloadsListCtrl();

        void Init();

        /**
         * updateDownloadList
         *
         * Updates this list control - based on the input parameters
         *
         * @param trainDownload the train download information
         * @param serviceNo     The current service number for the train.
         */
        void updateDownloadList(const DownloadStatusDisplay& trainDownload,
                                const std::string& serviceNo);


        /**
          * updateTrainDetails
          *
          * This list shows the PV/Service number.
          * This is basically just in case the service number changes.
          *
          * @param trainDetails The details for the given train.
          */
            void updateTrainDetails(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);


        /**
         * trainListSort
         *
         * Callback using by CListCtrl to sort the items in list
         *     See ListCtrl::SortItems for more details
         *
         * @param lParamSort a pointer to a CurrentDownloadsListCtrl
         *
         */
        static int CALLBACK trainListSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


        /**
         * clearCompleted
         *
         * Clears all completed downloads from the list
         *
         */
        void clearCompleted();


        /**
          * completedDownloadsInList
          *
          * Checks if there is at least one completed download in the list.
          */
        bool completedDownloadsInList();


        /**
         * getSelectedDownloads
         *
         * Gets the selected download/upgrades so that they can be cancelled.
         * Only auto retrying downloads are selectable.
         *
         * @return a vector of selected train downloads.
         */
        std::vector<DownloadStatusDisplay> getSelectedDownloads() const;


        /**
         * deselectAll
         *
         * Clears selection in this list control
         */
        void deselectAll();


        /**
         * selectAll
         *
         * Selects all (selectable) items in this list
         */
        void selectAll();

        /*
        // xufeng++ 2006/11/10 TDTD17538
         *
         * findTrainIndex
         *
         * Finds the index of the specified train in this list
         *
         * @return the index of the train, or -1 if not found

        int findTrainIndex(const DownloadStatusDisplay& trainDownload) const;

        DownloadStatusDisplay* getDownloadStatus(const DownloadStatusDisplay& trainDownload);
        // ++xufeng 2006/11/10 TDTD17538
        */
    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CurrentDownloadsListCtrl)
        protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:

        // {{AFX_MSG(CurrentDownloadsListCtrl)
        afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

        virtual bool isSelectable(int rowNumber);

    private:

        struct SortInformation
        {

            enum SortOrder
            {
                ASCENDING = 1,
                DESCENDING = ~ASCENDING
            };

            DownloadListColumns currentSortColumn;
            SortOrder currentSortOrder;

            SortInformation()
                : currentSortColumn(TRAIN),
                currentSortOrder(ASCENDING)
            {}

        };
        /**
         * findTrainIndex
         *
         * Finds the index of the specified train in this list
         *
         * @return the index of the train, or -1 if not found
         */
        int findTrainIndex(const DownloadStatusDisplay& trainDownload) const;

        /**
         * findSortedInsertionPoint
         *
         * Finds the insertion point required for the specified train to maintain a sorted list
         *
         * @param trainDownload the train download we want to update in the list
         *
         * @return the index where we can insert this train into - and keep list sorted alphabetically
         */
        int findSortedInsertionPoint(const DownloadStatusDisplay& trainDownload) const;


        /**
          * getTrainIdString
          *
          * convert the train id to a string.
          *
          * @param trainDownload The train download status structure.
          * @param serviceNo     The train service number.
          *
          * @return the train id as a string
          */
        CString getTrainIdString(const DownloadStatusDisplay& trainDownload,
                                 const std::string& serviceNo) const;


        /**
          * getTypeString
          *
          * convert the transaction type to a string (download or upgrade)
          *
          * @param trainDownload The train download status structure.
          *
          * @return the transaction type as a string
          */
        CString getTypeString(const DownloadStatusDisplay& trainDownload) const;


        /**
          * removeSortIndicator
          *
          * Removes any sort indicator from the column
          */
        void removeSortIndicator(int column);


        /**
          * addSortIndicator
          *
          * Adds the appropriate indicator to the given column
          */
        void addSortIndicator(int column, SortInformation::SortOrder indicatorType);


        /**
          * getCurrentSortInformation
          *
          * Holds the current sort column and order.
          * Used by the static sort function to get the sort
          * information from the given list.
          *
          * @return The SortInformation structure.
          */
        inline SortInformation getCurrentSortInformation()
        {
            return m_currentSortInformation;
        }

        SortInformation m_currentSortInformation;

        // custom header control
        TA_Base_Bus::FixedHeaderCtrl    m_fixedHeaderCtrl;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(AFX_CurrentDownloadsListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
