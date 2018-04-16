/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/trains/TrainSelectionComponent/src/TrainListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2018/01/09 10:38:00 $
  * Last modified by: $Author: jiayi.zhang $
  * 
  * TrainListCtrl defines a list control that displays train (incoming/outgoing)
  * for use in transactive GUIs requiring selectable trains.
  *
  * This can be used on its own, or in conjunction with TrainSelectorGUI
  */

#if !defined(TrainListCtrl_H)
#define TrainListCtrl_H

#include "bus/trains/TrainSelectionComponent/src/StdAfx.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/ICustomTrainDataProvider.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainListCtrlEventListener.h"

#include <map>

namespace TA_IRS_Bus
{

    class TrainListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:

		TrainListCtrl(std::string name);

	    virtual ~TrainListCtrl();

        // the standard columns for the list control
        enum TrainListColumns
        {
            LOCATION = 0,
            PV_SERVICE = 1,
            TETRA_ID = 2
        };


		
        struct SortInformation
        {
            enum SortOrder
            {
                ASCENDING = 1,
				DESCENDING = ~ASCENDING
            };
			
            // will match a train list column, or a custom column id.
            int currentSortColumn;
            SortOrder currentSortOrder;
			
            // default sort
            SortInformation();
        };


        /** 
          * setCustomDataProvider
          *
          * If this list has custom columns, set the object that provides it.
          * This must be done before adding any custom columns.
          *
          * @param provider The custom data provider.
          *                 Cannot be null if there are custom columns.
          */
        void setCustomDataProvider(ICustomTrainDataProvider* provider);


        /** 
          * addCustomColumn
          *
          * Adds a custom column to the list control, with the given name and size.
          *
          * When the list needs to refresh the data in the custom column, it will
          * ask for the data by train id and column name.
          * setCustomDataProvider() above must be called before this.
          * This will result in an assertion if not followed.
          *
          * @param columnName   The name/title of the column (unique - protected by assertion)
          * @param columnSize   The column width
          *
          * @return the index of the new column
          */
        int addCustomColumn(const std::string& columnName, int columnSize);


        /** 
          * refreshCustomColumn
          *
          * Force a query for new custom data.
          * The custom data provider cannot be null if there are custom columns.
          * This will result in an assertion.
          *
          * @param trainId      The train Id to query,
          * @param columnName   The custom data column to request.
          */
        void refreshCustomColumn( CommonTypes::TrainIdType trainId, const std::string& columnName );


        /** 
          * setStandardColumnSizes
          *
          * Set the sizes of the standard list columns.
          * A size of 0 will hide the column.
          */
        void setStandardColumnSizes( int locationColumnSize,
                                     int pvServiceColumnSize,
                                     int tetraIdColumnSize = 0 );


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
        void updateTrainList( const ITrainSelectorGUI::EAction& action, 
							  const TrainInformationTypes::TrainDetails& trainDetails );


        /**
         * selectAll
         *
         * Selects all items in this list
         */
        void selectAll();


        /**
         * deselectAll
         *
         * Clears selection in this list control
         */
        void deselectAll();


        /**
         * setSelectedTrains
         *
         * Selects all the trains that match the input IDs - any items
         *  not matching one of selection IDs will be unselected
         *
         * @param trainIds array of train IDs requiring selection
         */
        void setSelectedTrains(const CommonTypes::TrainIdList& trainIds);


        /**
         * getSelectedTrainIds
         *
         * @return an array of the train IDs of all those trains selected in the list(s)
         */
		CommonTypes::TrainIdList getSelectedTrainIds() const;


        /**
         * getSelectedTrainData
         *
         * same as above, but return the train data rather than just the train id.
         *
         * @return an array of the train details structures of all those trains selected in list(s)
         */
		TrainInformationTypes::TrainDetailsList getSelectedTrainData() const;


    public:
        /* The following are called by the TrainSelectorGUI object */

        /** 
          * padList
          *
          * This pads the list out with some blank items to
          * allow the lists to act as one in depot mode.
          *
          * This should only be called by the TrainSelectorGUI
          */
        void padList();


        /** 
          * removePadding
          *
          * This removes any padding added previously
          *
          * This should only be called by the TrainSelectorGUI
          */
        void removePadding();


        /** 
          * setEventListener
          *
          * Sets the object that receives and handles some events
          *
          * @param eventListener
          */
        void setEventListener(ITrainListCtrlEventListener* eventListener);


        /** 
          * redirectedVScroll
          *
          * Called to allow vertical scrolling. (By the TrainSelectorGUI)
          *
          * @param nSBCode
          * @param nPos
          * @param pScrollBar
          */
        void redirectedVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


        /** 
          * redirectedKeyDown
          *
          * Called to allow vertical scrolling. (By the TrainSelectorGUI)
          *
          * @param nChar
          * @param nRepCnt
          * @param nFlags
          */
        void redirectedKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


        /** 
          * redirectedOnMouseWheel
          *
          * Called to allow vertical scrolling. (By the TrainSelectorGUI)
          *
          * @param nFlags
          * @param zDelta
          * @param pt
          */
        BOOL redirectedOnMouseWheel(UINT nFlags, short zDelta, CPoint pt);


        /** 
          * redirectedColumnclick
          *
          * Called to allow column sorting
          *
          * @param pNMHDR
          * @param pResult
          */
        void redirectedColumnclick(NMHDR* pNMHDR, LRESULT* pResult);


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(TrainListCtrl)
	    protected:
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL


    protected:


		// Generated message map functions
	    //{{AFX_MSG(TrainListCtrl)
        afx_msg BOOL onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL onClick(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        afx_msg LRESULT onHideScrollBars(UINT wParam, LONG lParam);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

		/**
		 * this is a copy of TrainDetails with custom data, so it can be set as item data for each list item.
		 * @version 1.0
		 * @created 18-Mar-2008 5:46:26 PM
		 */
        struct TrainDetailsItemData
        {
			
        public:
			
            TrainDetailsItemData();
			
			
            /**
			 * 
			 * @param in
			 */
            TrainDetailsItemData(const TrainInformationTypes::TrainDetails& in);
			
			
            /**
			 * 
			 * @param rhs
			 */
            void operator=(const TrainInformationTypes::TrainDetails& rhs);
			
			
            TrainInformationTypes::TrainDetails toBasicType() const;


            /**
			 * The physical train number
			 */
            CommonTypes::TrainIdType trainId;
			
			
            /**
			 * Whether the train is sending OA1/ATC tables over radio
			 */
            bool communicationValid;

            
            /**
             * This is where the train is currently sending OA1/ATC tables.
             * If the communication is not valid, this is the last place they were sent,
             * or 0 if they were never sent.
             */
            unsigned long communicatingLocation;
			
			
            /**
			 * The current primary ITSI of the train (the one currently communicating)
			 */
            std::string primaryTsi;
			
			
            /**
			 * The current secondary ITSI of the train (the one not currently communicating)
			 */
            std::string secondaryTsi;
			
			
            /**
			 * Whether the train is reported in ATS or not.
			 */
            bool atsValid;
			
			
            /**
			 * If the ATS is valid, is the localisation information held by ATS valid
			 */
            bool isLocalisationValid;
			
			
            /**
			 * The current location key of the train, or the last known location if the train is not valid.
			 */
            unsigned long currentLocation;


            /**
             * The CCTV zone of the train (not taking into acount pre conflict or conflict)
             */
            unsigned long currentCctvZone;
			
			
            /**
			 * Whether the train is mute or not (ATS flag)
			 */
            bool isMute;
			
			
            /**
			 * The service number of the train, or the last known service number if ats is not valid.
			 */
            std::string serviceNumber;
			
			
            /**
			 * The track the train is on (from ATS)
			 */
            TrainInformationTypes::EAtsTrack track;
			
			
            /**
			 * this is all the custom data for the item index -> value
			 */
            std::map< int, std::string > customDataValues;
			
        };


        /**
         * findTrainIndex
         *
         * Finds the index of the specified train in this list
         *
         * @return the index of the train, or -1 if not found
         */
        int findTrainIndex( CommonTypes::TrainIdType trainId ) const;


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

        
        /**
         * trainListSort
         *
         * Callback using by CListCtrl to sort the items in list
         *     See ListCtrl::SortItems for more details
         *
         * @param lParamSort a pointer to a TrainListCtrl
         *
         */
        static int CALLBACK trainListSort( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );


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
        void deleteFromList( const TrainInformationTypes::TrainDetails& td );


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
        void addToList( const TrainInformationTypes::TrainDetails& td );


        /**
         * updateList
         * 
         * Updates the specified train in the list (based on its train ID)
         *
         * @param td the train we want to update in the list
         *
         */
        void updateList(const TrainInformationTypes::TrainDetails& td);


        /**
         * getLocationString
         *
         * @return the string representation of what should be displayed in the location column of this list
         *
         */
        CString getLocationString(const TrainDetailsItemData& td) const;


        /**
         * getServiceString
         *
         * @return the string representation of what should be displayed in the service column of this list
         *
         */
        CString getPvServiceString(const TrainDetailsItemData& td) const;


        /**
         * getTetraIdString
         *
         * @return the string representation of what should be displayed in the Tetra ID column of this list
         *
         */
        CString getTetraIdString(const TrainDetailsItemData& td) const;


        /**
         * findSortedInsertionPoint
         *
         * Finds the insertion point required for the specified train to maintain a sorted list
         *
         * @param td the train we want to update in the list
         *
         * @return the index where we can insert this train into - and keep list sorted
         */
        int findSortedInsertionPoint(TrainDetailsItemData& td) const;


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


        // the current sorting
        SortInformation m_currentSortInformation;


        // the custom columns
        // mapped on name -> column index
        std::map<std::string, int> m_customColumns;


        // the object that provides the custom data for the custom columns
        ICustomTrainDataProvider* m_customDataProvider;


        // the custom header control
        TA_Base_Bus::FixedHeaderCtrl	m_fixedHeaderCtrl;


        // Handles events
        ITrainListCtrlEventListener* m_eventListener;


		// are there any blank items?
        bool m_paddedList;

		unsigned long m_locationKey;

		std::string m_name; //it is used to identify the list on log file.
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_Bus

#endif // !defined(TrainListCtrl_H)
