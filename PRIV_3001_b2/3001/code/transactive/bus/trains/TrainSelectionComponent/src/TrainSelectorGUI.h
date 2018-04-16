/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This handles the enabling/disabling of GUI items for train selection.
  * It also handles interaction between the components - so each gui can have
  * a common feel/operation, but the layout can be different.
  *
  */

#if !defined(TRAINSELECTORGUI_H)
#define TRAINSELECTORGUI_H

#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"
#include "bus/trains/TrainSelectionComponent/src/TrainListCtrl.h"
#include "bus/trains/TrainSelectionComponent/src/TrainGroupComboBox.h"
#include "bus/trains/TrainSelectionComponent/src/TrainStaticGroupComboBox.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectionListener.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainListCtrlEventListener.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainGroupComboEventListener.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainStaticGroupComboEventListener.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include <vector>
#include <string>


namespace TA_IRS_Bus
{
    class TrainSelectorGUI : public ITrainSelectorGUI,
                             public ITrainListCtrlEventListener,
							 public ITrainStaticGroupComboEventListener,
                             public ITrainGroupComboEventListener,
                             public ILocationCacheObserver
    {

    public:


        /** 
          * TrainSelectorGUI
          *
          * Constructor that takes gui elements
          * If any gui elements are null, they will be ignored.
          * GUI elements that cannot be null are passed as references.
          *
          * @param inboundList      The inbound selection list
          * @param outboundList     The outbound selection list (if null, the inbound list will hold all trains)
          * @param allInbound       The button that selects all inbound trains
          * @param allOutbound      The button that selects all outbound trains
          * @param allTrains        The button that selects all trains
          * @param clearAllTrains   The button that deselects all trains
          * @param inboundLabel     The label above the inbound list
          * @param outboundLabel    The label above the outbound list
          * @param trainGroupCombo  The train group combo box
          * @param saveGroup        The save train group button
          * @param deleteGroup      The delete train group button
		  * @param trainStaticGroupCombo    This is the combo box for selecting train static talkgroups
          * @param selectionListener The object that should be notified when the train selection is changed
          */
        TrainSelectorGUI(TrainListCtrl& inboundList,
                         TrainListCtrl* outboundList,
                         CButton* allInbound,
                         CButton* allOutbound,
                         CButton* allTrains,
                         CButton* clearAllTrains,
                         CStatic* inboundLabel,
                         CStatic* outboundLabel,
                         TrainGroupComboBox* trainGroupCombo,
                         CButton* saveGroup,
                         CButton* deleteGroup,
						 TrainStaticGroupComboBox* trainStaticGroupCombo = NULL,
                         ITrainSelectionListener* selectionListener = NULL);
        

        ~TrainSelectorGUI();


        /** 
          * setTrainSelector
          *
          * Sets the train selector to be used for loading, saving, and deleting groups.
          * This is kind of a special case, because the train selector can throw exceptions
          * from its constructor, so if the train agent isnt around, the train selector
          * cant be constructed. This allows this object to be created without a train selector.
          *
          * @param theTrainSelector A pointer to the train selector.
          *                         If null, groups will not be enabled.
          */
        void setTrainSelector(TrainSelector* theTrainSelector);


        /** 
          * setDepotMode
          *
          * The Depot tab has been selected.
          * Hide the inbound+outbound labels and buttons.
          */
        void setDepotMode();


        /** 
          * setMainlineMode
          *
          * The mainline tab has been selected.
          * Show the inbound+outbound labels and buttons.
          */
        void setMainlineMode();


        /** 
          * setRights
          *
          * Sets whether the current operator has rights to access trains and train groups.
          * False will disable the items. True will enable the items if they should
          * be enabled based on GUI logic.
          * If the first flag is false, that will imply the rest are also false.
          * If the first train group flag is false, that implies that the other two are also false.
          *
          * @param canSelectTrains      Whether the operator can select trains at all
          * @param canUseTrainGroups    Whether the operator can use groups at all
          * @param canSaveTrainGroups   Whether the operator can save train groups
          * @param canDeleteTrainGroups Whether the operator can delete train groups
          *
          */
        void setRights( bool canSelectTrains,
                        bool canUseTrainGroups,
                        bool canSaveTrainGroups,
                        bool canDeleteTrainGroups);


        /** 
          * getSelectedTrains
          *
          * Gets the selected trains (from both lists)
          *
          * @return A vector of train IDs
          */
        CommonTypes::TrainIdList getSelectedTrains() const;


        /** 
          * getSelectedTrainData
          *
          * Gets the selected trains (from both lists)
          *
          * @return A vector of train details structures
          */
        virtual TrainInformationTypes::TrainDetailsList getSelectedTrainData() const;


        /* Button Press Callbacks - The GUI must pass these on */

        /** 
          * selectAllInbound
          *
          * The all inbound button has been pressed.
          * Select all inbound trains.
          */
        void selectAllInbound();


        /** 
          * selectAllOutbound
          *
          * The all outbound button has been pressed.
          * Select all outbound trains.
          */
	    void selectAllOutbound();


        /** 
          * selectAllTrains
          *
          * The select all button was pressed.
          */
	    void selectAllTrains();


        /** 
          * clearSelection
          *
          * The clear all button was pressed.
          */
	    void clearSelection();


        /** 
          * saveTrainSelection
          *
          * Saves the selected trains as the group in the combo box.
          */
        void saveTrainSelection();


        /** 
          * deleteTrainSelection
          *
          * Deletes the selected train group in the combo box.
          */
        void deleteTrainSelection();


        /* ITrainSelectorGUI implementation 
           Keep threading in mind, it might be a good idea to use a post message,
           Or the MFC train selector, and just pass the messages on. */

        /**
         * Updates the list of inbound trains for this GUI
         *
         * @param action        the action to take (see EAction definition).
         * @param trainDetails  the new details of the train if this is an Add
         *                      or Update.
         */
        virtual void updateInboundList( EAction action,
										const TrainInformationTypes::TrainDetails& trainDetails );

        
        /**
         * Updates the list of outbound trains for this GUI
         *
         * @param action        the action to take (see EAction definition).
         * @param trainDetails  the new details of the train if this is an Add
         *                      or Update.
         */
        virtual void updateOutboundList( EAction action,
										 const TrainInformationTypes::TrainDetails& trainDetails);

		/**
		 * Updates the status of fal back mode
		 * 
		 * @param inFallback    Whether the local radio is in fallback mode or not
		 */
        void updateFallbackStatus(bool inFallback);


        /**
         * Called when a selection group is added or deleted. The GUI should
         * update its selection combo with the list of train selection list
         * names.
         *
         * @param trainListNames  List of names of train selection lists that
         *                        are available.
         */
        virtual void updateSelectionCombo( const TrainInformationTypes::TrainSelectionList& trainListNames );


		/**
		 * This is used by the train selector to determine if it needs to update radio groups or not.
		 * 
		 * @return true if radio group updates are needed
		 */
        virtual bool hasRadioGroupSupport();


		/**
		 * Called when a radio group is enabled or disabled
		 * The GUI should update its selection combo with the list of radio group names.
		 * 
		 * @param trainListNames    List of names of train selection lists that are available.
		 */
        virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames);


        /**
         * This is the master enable/disable of the radio group combo
         * If the application is in a state where the radio group combo is not applicable
         * it can stop it from becoming enabled
         */
        void enableRadioGroupCombo( bool enable );


        /**
          * Called by a list when it receives an OnVScroll.
          * The list should not call CListCtrl::OnVScroll, that will be done
          * via the redirected scroll method.
          *
          * @param list     The list this comes from
          * @param nSBCode  from MFC
          * @param nPos     from MFC
          * @param *pScrollBar  from MFC
          */
        virtual void listScrolledV( TrainListCtrl* list,
                                    UINT nSBCode,
                                    UINT nPos,
                                    CScrollBar *pScrollBar );


        /** 
          * onMouseWheel
          *
          * Called when a mouse wheel event is received by a train list
          *
          * @param list The list this comes from
          * @param nFlags
          * @param zDelta
          * @param pt
          */
        virtual BOOL onMouseWheel( TrainListCtrl* list,
                                   UINT nFlags,
                                   short zDelta,
                                   CPoint pt );


        /** 
          * onKeyDown
          *
          * Manages scrolling by keypress
          *
          * @param list     The list this comes from
          * @param nChar
          * @param nRepCnt
          * @param 
          */
        virtual void onKeyDown( TrainListCtrl* list,
                                UINT nChar,
                                UINT nRepCnt,
                                UINT nFlags );


        /** 
          * onColumnclick
          *
          * Called when the column of a list is clicked
          *
          * @param list     The list this comes from
          * @param pNMHDR
          * @param pResult
          */
        virtual void onColumnclick( TrainListCtrl* list,
                                    NMHDR* pNMHDR,
                                    LRESULT* pResult );


        /** 
          * onClick
          *
          * Called when a list is clicked
          *
          * @param list
          * @param pNMHDR
          * @param pResult
          */
        virtual void onClick( TrainListCtrl* list,
                              NMHDR* pNMHDR,
                              LRESULT* pResult );


        /** 
          * onItemchanged
          *
          * Called when a list item is changed
          *
          * @param list
          * @param pNMHDR
          * @param pResult
          */
        virtual void onItemchanged( TrainListCtrl* list,
                                    NMHDR* pNMHDR,
                                    LRESULT* pResult );

 
		/**
		 * Called when a train list is trying to determine whether a train should be enabled for selection or not.
		 * 
		 * @param list    The list this comes from
		 * @param trainId    The train
		 * @param trainLocation    The train location
		 * @param rowNumber    the row of the list control
         */
        virtual bool onIsSelectable( TrainListCtrl* list,
                                     CommonTypes::TrainIdType trainId,
                                     unsigned long trainLocation, 
                                     int rowNumber,
									 unsigned long trainCommunicationLocation);


        /** 
          * selectionChangedGroupCombo
          *
          * Called by the train group combo when its selection
          * is changed.
          */
        virtual void selectionChangedGroupCombo();


        /** 
          * editChangedGroupCombo
          *
          * Called by the train group combo when its edit box
          * is changed.
          */
        virtual void editChangedGroupCombo();


		/**
		 * Called by the static group combo when its edit box is changed.
		 */
        virtual void editChangedStaticGroupCombo();
		
		
        /**
		 * Called by the static group combo when its selection is changed.
		 */
        virtual void selectionChangedStaticGroupCombo();


    private:

        TrainSelectorGUI(){}


        /** 
          * showInboundOutboundControls
          *
          * Shows or hides inbound and outbound buttons and labels.
          * So these can be shown in mainline mode, and hidded in depot mode.
          *
          * @param show Whether to show the controls or not
          */
        void showInboundOutboundControls(bool show);


        /** 
          * enableButtons
          *
          * Based on the state of the lists, enable or disable select/deselect
          * buttons.
          */
        void enableButtons();


        /** 
          * enableGroupControls
          *
          * Based on the state of the lists, and the rights of the operator,
          * enable or disable train group controls.
          */
        void enableGroupControls();


        /** 
          * notifySelectionChanged
          *
          * Optionally tell the interested object if a train is selected or not.
          */
        void notifySelectionChanged();


        /** 
          * clearData
          *
          * Clear all trains and groups from lists.
          */
        void clearData();


        /** 
          * positionListItems
          *
          * Makes sure the top of the second list is the
          * bottom of the first list.
          *
          * @param referenceList The list that shouldnt scroll.
          */
        void positionListItems(TrainListCtrl* referenceList);

        /** 
          * onLocationCacheChange
          *
          * Called by LocationCache
          */
        virtual void onLocationCacheChange();

    private:

        // The current mode of the page
        // InService or Depot (Ignore station - same as InService as far as this gui is concerned)
        LocationCache::EStationType m_currentMode;

        // The train selector to use for saving, deleting and retreiving selection lists.
        TrainSelector* m_trainSelector;

        // The train lists
        TrainListCtrl* m_inboundList;
        TrainListCtrl* m_outboundList;

        // the buttons that control selection of the train lists
        CButton* m_allInbound;
        CButton* m_allOutbound;
        CButton* m_allTrains;
        CButton* m_clearAllTrains;

        // the labels for the inbound and outbound lists
        CStatic* m_inboundLabel;
        CStatic* m_outboundLabel;

        // The train selection group combo box and associated buttons
        TrainGroupComboBox* m_trainGroupCombo;
        CButton* m_saveGroup;
        CButton* m_deleteGroup;

        // optionally tell this object when the selection changes.
        ITrainSelectionListener* m_selectionListener;

        // whether the train lists should be enabled
        bool m_trainListsEnabled;
        // Whether the combo box, save, and delete for train groups should be enabled
        // This is rights based. - The actual state will depend on other things as well
        bool m_trainGroupsEnabled;
        bool m_trainGroupSaveEnabled;
        bool m_trainGroupDeleteEnabled;

        // this is just to make it more efficient
        // the gui will only be notified of selection changes
        // if the selection is different from what it last sent
        bool m_trainsAreSelected;

		TrainStaticGroupComboBox* m_trainStaticGroupCombo;
        
        // whether the application will allow the static group combo to be enabled or not
        bool m_enableStaticGroupCombo;
		
		
        /**
		* Whether in radio fallback or not
		*/
        bool m_isInRadioFallback;

    };

} // TA_IRS_Bus

#endif // !defined(TRAINSELECTORGUI_H)

