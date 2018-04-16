/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $Date: 2008/11/28 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(ITrainSelectorGUI_H)
#define ITrainSelectorGUI_H

#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"


namespace TA_IRS_Bus
{

    /**
     * The interface that must be implemented by GUIs using the TrainSelectionComponent.
     * @version 1.0
     * @created 18-Mar-2008 5:46:19 PM
     */

    class ITrainSelectorGUI
    {

    public:

        /**
         * Actions that may be taken when the list of trains in the GUI changes.
         * @version 1.0
         * @created 18-Mar-2008 5:46:20 PM
         */
        typedef enum EAction
        {
        	/**
        	 * Add a new train to the list
        	 */
        	Add,
        	/**
        	 * Update a train that exists in the list
        	 */
        	Update,
        	/**
        	 * Delete a train from the list
        	 */
        	Delete,
        	/**
        	 * Delete all trains from the list
        	 */
        	ClearAll
        };


        ITrainSelectorGUI() {}


        virtual ~ITrainSelectorGUI() {}


        /**
         * Updates the list of inbound trains for this GUI
         * 
         * @param action    the action to take (see EAction definition).
         * @param trainDetails    the new details of the train if this is an Add or Update.
         */
        virtual void updateInboundList(EAction action, const TrainInformationTypes::TrainDetails& trainDetails) =0;


        /**
         * Updates the list of outbound trains for this GUI
         * 
         * @param action    the action to take (see EAction definition).
         * @param trainDetails    the new details of the train if this is an Add or Update.
         */
        virtual void updateOutboundList(EAction action, const TrainInformationTypes::TrainDetails& trainDetails) =0;


        /**
         * Called when a selection group is added or deleted.
         * The GUI should update its selection combo with the list of train selection list names.
         * 
         * @param trainListNames    List of names of train selection lists that are available.
         */
        virtual void updateSelectionCombo(const TrainInformationTypes::TrainSelectionList& trainListNames) =0;


        /**
         * Updates the status of fallback mode
         * 
         * @param inFallback    Whether the local radio is in fallback mode or not
         */
        virtual void updateFallbackStatus(bool inFallback) =0;


        /**
         * This is used by the train selector to determine if it needs to update radio groups or not.
         * 
         * @return true if radio group updates are needed
         */
        virtual bool hasRadioGroupSupport() =0;


        /**
         * Called when a radio group is enabled or disabled
         * The GUI should update its selection combo with the list of radio group names.
         * 
         * @param trainListNames    List of names of train selection lists that are available.
         */
        virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames) =0;


        /**
         * Gets the selected trains (from both lists)
         * 
         * @return A list of train details structures
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const =0;

    };

}

#endif // !defined(ITrainSelectorGUI_H)

