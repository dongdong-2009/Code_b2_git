/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSearchPage/src/SearchPage.h $
  * @author Nick Jardine / Andy Parker
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides specialised search functionality for subscribers in radio directory
  *   
  */

#pragma once

#include <vector>

// This is to stop #include path compiler errors while compiling IDL..
#ifndef __CommonExceptionsCorbaDef_hh_EXTERNAL_GUARD__
#define __CommonExceptionsCorbaDef_hh_EXTERNAL_GUARD__
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#endif

#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h" //TD17304
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/radio/radiosearchpage/src/ISearchPageHelper.h"
#include "bus/radio/radiosearchpage/src/resource.h"
#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "core/data_access_interface/src/ILocation.h"
#include "ItemKeyToNameMap.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_Bus
{

    /////////////////////////////////////////////////////////////////////////////
    // CSearchPage dialog
    class CSearchPage : public CDialog, public TA_Base_Core::RunParamUser
    {
    protected:
        static ISearchPageHelper* s_dummyHelper;    // For default (dummy) constructor
        CSearchPage();
        DECLARE_DYNCREATE(CSearchPage)

    // Construction
    public:

        typedef TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification SubscriberNotification;

        CSearchPage(ISearchPageHelper& helper);
        ~CSearchPage();

        /**
         * Seperate the initialisation part from OnInitDialog() function.
         * This function must be called after Creating the object and before starting using it.
         */
        void Init();

        /**
         * [NOT THREADSAFE]
         *
         * removes the specified subscriber from the dynamic group
         * If not already in dynamic groups section, this call is ignored
         *
         * @pre function must be called from the main windows app thread
         */
        void removeDynamicGroupSubscriber(CString tsi);

        /**
         * [not threadsafe]
         *
         * Refreshes the dynamic group subscriber area of the Directory tree control
         * @param subscriberName, tsi, locationKey details of the subscriber
         *
         * @pre function must be called from the main windows app thread
         */
        void processDynamicGroupSubscriberUpdate(CString subscriberName,
                            CString tsi);


        /**
         * @return vector array of all the TSI's currently contained under the "Dynamic Group" node
         */
        std::vector<CString> getDynamicGroupTSIs();

        /**
         * @return appropriate location name as defined by the input key
         */
        CString getLocationNameFromKey(unsigned long locationKey);

        /*
         * onRunParamChange (interface implementation)
         *
         * @see RunParamUser::onRunParamChange for more details
         *
         */
        virtual void onRunParamChange(const std::string& name, const std::string& value);
        void onUpdateRights();

        struct TrainUpdate
        {
            unsigned char trainID;
            CString ITSI;
            CString name;
            unsigned long locationKey;
        };

        /**
         * If this TSI is a train, get the train ID it represents
         * otherwise return 0
         */
        unsigned char getTrainIdForTsi(const std::string& tsi);
        /**
         * Given a train ID, get the details for it.
         */
        TrainUpdate getDetailsForTrain(unsigned char trainId);

        /// Function for testing/debugging, so we can pump messages into subscriber
        TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>& DEBUG_getSubscriber() { return *m_messageSubscriber; }

        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetailsFromTSI(CString tsi);//limin, also made public

        void populateSearchListWithTSI(CString itemTSI, HTREEITEM parentItem, HTREEITEM selectedItem,bool bClearItems = true);//limin, made public
                
        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetailsFromTrainID(BYTE trainId);//limin, made public

    // Dialog Data
        //{{AFX_DATA(CSearchPage)
        enum { IDD = IDD_RADIO_FACILITYSEARCH };
        CButton    m_searchButton;
        CEdit    m_searchText;
        //CListCtrl    m_SearchList;
        TA_Base_Bus::AutoSortListCtrl m_SearchList;  //TD17304
        CTreeCtrl    m_DirectoryTree;
        //}}AFX_DATA


    // Overrides
        // ClassWizard generate virtual function overrides
        //{{AFX_VIRTUAL(CSearchPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
        //}}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        //{{AFX_MSG(CSearchPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnSubscribersearch();
        afx_msg void OnDisplayGroupDetails();
        afx_msg void OnOK();
        afx_msg void OnCancel();
        afx_msg void OnDestroy();
        afx_msg void OnChangeEditSearchText();

        afx_msg void OnAddToDynamicGroup();
        afx_msg void OnAddToMonitor();
        afx_msg void OnDblclkSearchList(NMHDR* pNMHDR, LRESULT* pResult);

        LRESULT OnSubscriberNotification(UINT wParam, LONG lParam);
        LRESULT OnSelectionChanged(UINT wParam, LONG lParam);

        //}}AFX_MSG
        afx_msg void OnInboundTrain(WPARAM);
        afx_msg void OnOutboundTrain(WPARAM);
        afx_msg void OnComboChanged(WPARAM);
        DECLARE_MESSAGE_MAP()

        /**
         * Intercept all messages to allow interception of train selection component messages
         *
         */
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    private:

        /// @return suitable train service number, derived from input train details
        CString getTrainServiceNumber(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& details);//limin, modify


        bool doesItemExistInSearchList(CString name);
        /**
         * Updates the enabled state of the search subscribers button
         * @param fullRefresh true to requery the callback object to update the
         *      state of the rights.  False to used the cached rights value for search text button.
         */
        void updateSearchTextButtonEnabledState(bool fullRefresh);

        /**
         * Methods for populating the seach list area with current selection details
         * TD14417 - add bool parameter to whether clear the searchlist or not
         */
        void populateSearchListWithGroup(CString groupTSI,bool bClearItems = true);
        void populateSearchListWithDynamicGroup();
        void populateSearchListWithStation(HTREEITEM &selectedItem, bool bClearItems = true);

        void traverseTree(HTREEITEM node, int count = 0);
        void populateSearchListWithNode(HTREEITEM node);

        /**
         * getSubscriberDetails
         *
         * @pre index >= 0 && index < m_SearchList.GetItemCount()
         * @return a set of strings defining the subscriber in the search list control
         *       at the specified index
         * @param index defines the index into the search CListCtrl object
         */
        std::vector<CString> getSubscriberDetails(int index);

        /**
         * processUpdate
         *
         * Processes a subscriber update notification
         * @param update - data defining the update
         */
        void processUpdate(const SubscriberNotification& update);

        /**
         * processTrainNotification
         *
         * The Search Page doesn't care whether the train is inbound or outbound - it cares whether it's been added or
         * removed.
         *
         * @param trainListUpdate The update to process
         */
        void processTrainNotification(TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate);

        /**
         * refreshLastUpdateField
         *
         * Called to force an update of the 'last updated' field on this dialog
         */
        void refreshLastUpdateField();

        /**
         * addNewSubscriber
         *
         * Adds in a new user or group subscriber to the tree control
         * @pre details represents a INDIVIDUAL_SUBSCRIBER or GROUP_SUBSCRIBER
         * @param details defines the subscriber to be added to the control
         */
        void addNewSubscriber(const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details);

        /**
         * updateSubscriber
         *
         * Updates an existing subscriber in the tree control
         * @pre details represents a INDIVIDUAL_SUBSCRIBER or GROUP_SUBSCRIBER
         * @param details defines the subscriber to be udpated
         */
        void updateSubscriber(const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details);

        /**
         * deleteSubscriber
         *
         * Deletes an existing subscriber frin the tree control
         * @pre details represents a INDIVIDUAL_SUBSCRIBER or GROUP_SUBSCRIBER
         * @param details defines the subscriber to be removed
         */
        void deleteSubscriber(const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details);

        /**
         * findUserHItemFromTSI
         *
         * @return the associated HITEM in the tree control that corresponds to the input TSI
         *          of a user or group subscriber or 0 if not found..
         */
        HTREEITEM findUserHItemFromTSI(CString tsi) const;

        /**
         * findDynamicGroupHItemFromTSI
         *
         * @return the associated HITEM in the tree control that corresponds to the input TSI
         *          of a dynamic group subscriber or 0 if not found..
         */
        HTREEITEM findDynamicGroupHItemFromTSI(CString tsi) const;


        /**
         * hasSelectedUsersLevelItem
         *
         * @return The tree control item that has been selected is the Users level item or a sub-level of
         *         the Users level.
         */
        bool hasSelectedUsersLevelItem();

        /**
         * hasSelectedGroupsLevelItem
         *
         * @return The tree control item that has been selected is the Groups level item
         */
        bool hasSelectedGroupsLevelItem();

        /**
         * hasSelectedTrainsLevelItem
         *
         * @return The tree control item that has been selected is the Trains level item
         */
        bool hasSelectedTrainsLevelItem();

        /**
         * update train details
         */
        void addTrain(TrainUpdate& update);
        void updateTrain(TrainUpdate& update);
        void deleteTrain(TrainUpdate& update);

        void initSortInfo(); //TD17304
        struct SubscriberDetails
        {
            CString subscriberName;
            CString tsi;
        };

        struct UserDetails : public SubscriberDetails
        {
            // If type changes, parent node changes (from user to group)
            TA_Base_Bus::IRadioCorbaDef::SubscriberType type;
            // Must store these additional details to see if location needs to change..
            // If location changes, parent node changes to proper location
            unsigned long locationKey;
        };

        // User ITSI Map is exclusively for "Group" and "Users" nodes (ie not Trains or "Dynamic Groups")
        typedef std::map<HTREEITEM, UserDetails> UserITSIMap;
        UserITSIMap m_userITSIs;

        typedef std::map<HTREEITEM, SubscriberDetails> DynamicGroupITSIMap;
        DynamicGroupITSIMap m_dynamicGroupITSIs;

        typedef std::map<unsigned char, TrainUpdate> TrainITSIMap;
        TrainITSIMap m_trainITSIs;

        static const int TYPE_TRAIN;
        static const int TYPE_INDIVIDUAL;
        static const int TYPE_GROUP;
        static const int TYPE_DYNAMIC_GROUP;

        /// Helper object to interface with main application
        ISearchPageHelper& m_callbackHelper;

        enum EColumnIds
        {
            COL_ID = 0,
            COL_TSI = 1,
            COL_LOCATION = 2,
            COL_MAX = 3
        };

        /// Subscriber for User/Group subscriber updates
        TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>* m_messageSubscriber;

        TA_Base_Bus::FixedHeaderCtrl m_fixedHeaderCtrl;

        HTREEITEM m_hItemUsers;
        HTREEITEM m_hTreeItemOcc;
        HTREEITEM m_hTreeItemDepot;
        HTREEITEM m_hTreeItemStation;

        HTREEITEM m_trainItem;
        HTREEITEM m_hGroupsItem;
        HTREEITEM m_hDynamicGroupsItem;

        TA_Base_Core::PrimitiveWrapper<long> m_occKey;
        TA_Base_Core::PrimitiveWrapper<long> m_depotKey;
        TA_Base_Core::PrimitiveWrapper<bool> m_searchButtonAccessRight;

        // This map only grows to add new locations, the tree listing of
        // locations never shrinks (if locations suddently have no child nodes)
        std::map<std::string, HTREEITEM> m_stationLocationMap;
        TA_Base_Bus::AutoSortListCtrl::SortInfo m_SortInfo; //TD17304
    };

} // end namespace TA_IRS_Bus
