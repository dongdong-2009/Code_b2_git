/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSearchPage/src/ISearchPageHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Helper interface to allow CSearchPage to be abstracted away from RadioManager
  * (and be shared with other applications)
  *
  */

#pragma once

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/radioagentcorbadef_4669/IDL/src/IRadioDirectoryCorbaDef.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"

#include "core/naming/src/NamedObject.h"


namespace TA_IRS_Bus
{
    interface ISearchPageHelper
    {
        enum ESelectionType
        {
            USER,                   // Single user (within "Users" or "Dynamic Groups")
            GROUP,                  // Group (under "Groups")
            TRAIN,                  // Train (under "Trains")
            DYNAMIC_GROUP_ROOT      // Root node "Dynamic Groups"
        };

        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
                                        TA_Base_Bus::IRadioCorbaDef_ptr,
                                        TA_Base_Bus::IRadioCorbaDef_var> IRadio;

        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioDirectoryCorbaDef,
                                        TA_Base_Bus::IRadioDirectoryCorbaDef_ptr,
                                        TA_Base_Bus::IRadioDirectoryCorbaDef_var> IRadioDirectory;

        virtual IRadio& getRadioCorbaDef() = 0;

        /**
         * getRadioDirectoryCorbaDef
         *
         * @return radio directory CORBA reference
         *
         */
        virtual IRadioDirectory& getRadioDirectoryCorbaDef() = 0;

        /**
         * getTrainSelectorGUI
         *
         * @return reference to Train Selection component object
         *
         */
        virtual TA_IRS_Bus::MFCTrainSelectorGUI& getTrainSelectorGUI() = 0;
        virtual TA_IRS_Bus::TrainSelector* getTrainSelector() = 0;

        /**
         * isFunctionPermitted
         *
         * @return true if function is allowed
         *
         * @param globalRightIdentifier the identifier corresponding to the entry
         *      in global rights registry (identifying the operation to check privelege for)
         */
        virtual bool isFunctionPermitted(DWORD globalRightIdentifier) = 0;

        /**
         * SetCall
         *
         * This is called whenever the selection changes (the selected subscriber)
         *  - the selection can be from either the tree or the 'search matches' area
         *
         * @param tsiNumber, label, hotline the relevant parameters detailing the
         *      currently selected call
         * @param type defines type of user (or group) selected
         *
         */
        virtual void SetCall(CString& tsiNumber, CString& label, bool hotline, const ESelectionType& type) = 0;

        /**
         * addMonitors
         *
         * Instructs the client code that the operator is wanting to add users
         *  to the selected groups list
         *
         * @param member The members to add to the monitor list control.
         *              Each element defines an array of strings detailing the user
         *              Each user contains string data in the following order: ID, LOCATION, TSI
         *
         */
        virtual void addMonitors(std::vector<std::vector<CString> >* selectedSubscribers) = 0;

        /**
         * addDynamicGroupMembers
         *
         * Instructs the client code that the operator is wanting to add users
         *  to the selected groups list
         *
         * @param member The members to add to the dynamic group page.
         *              Each element defines an array of strings detailing the user
         *              Each user contains string data in the following order: ID, LOCATION, TSI
         *
         */
        virtual void addDynamicGroupMembers(std::vector<std::vector<CString> >* selectedSubscribers) = 0;


        /**
         * getAddButtonRightIdentifier
         *
         * @return the right ID that we should check before enabling the 'add' button
         */
        virtual DWORD getAddButtonRightIdentifier() = 0;

        /**
         * getEntityLocationKey
         *
         * @return location key, for usage with subscriber
         */
        virtual DWORD getEntityLocationKey() = 0;

        /**
         * shouldDisplayDynamicGroupNode
         *
         * @return true if the 'Dynamic Group' parent node should be displayed
         *              in the tree control, false to hide it
         */
        virtual bool shouldDisplayDynamicGroupNode() = 0;

        /**
         * selectedTrainDetailsUpdated
         *
         * Tells the GUI to update this item if it is in the dynamic group list
         * or if it is in the selected TSI list
         */
        virtual void selectedTrainDetailsUpdated( CString& oldTsiNumber,
                                                  CString& oldServiceNumber,
                                                  CString& oldLocation,
                                                  CString& newTsiNumber,
                                                  CString& newServiceNumber,
                                                  CString& newLocation ) = 0;
    };

} // end namespace TA_Bus

