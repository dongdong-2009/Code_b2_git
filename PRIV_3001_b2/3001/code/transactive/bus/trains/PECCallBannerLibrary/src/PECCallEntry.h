/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $2008-01-09$
  * Last modified by:  $LiangHua Yuan$
  *
  */
#if !defined(PECCallEntry_H)
#define PECCallEntry_H


#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/trains/TrainCorbaProxy/src/TrainPecCorbaProxy.h"

#include <string>


// forward declarations
namespace TA_IRS_Bus
{
    class PECCallInitialiser;
    class PECCallFactory;
    class PECDialog;
	//class PECCallCCTV;
}


namespace TA_IRS_Bus
{
    
    /**
     * @author Robert van Hugten
     * @version 1.0
     * @created 21-Dec-2007 3:49:54 PM
     */

    class PECCallEntry : public AbstractCallEntry
    {

    public:

        /**
         * The type of call entry this is, ie PEC
         */
        static const std::string CALL_ENTRY_TYPE;


        /**
         * Creates a new call entry.
         *
         * @param id    the id of this PEC call entry.
         * @param point    the PEC point identifying the PEC from which the message was
         * received.
         * @param factory    the call factory that created this object.
         * @param pecCorbaProxy    The object used to communicate with the train agent
         */
        PECCallEntry( unsigned long id,
                      const PecTypes::PecUpdate& point,
                      PECCallFactory& factory,
                      TrainPecCorbaProxyPtr pecCorbaProxy );


        /**
         * The destructor.
         */
        virtual ~PECCallEntry();


        /**
         * Gets the type of call entry that this object represents, as a string.
         *
         * @return this call type as a string
         */
        static std::string getStaticType();


        /**
         * Gets the type of call entry that this object represents, as a string.
         *
         * @return this call type as a string
         */
        virtual std::string getType() const;


        /**
         * Returns the call category for th call banner filter buttons.
         *
         * @return the call category which is just PEC.
         */
        virtual std::string getCategory() const
        {
            // PECs have their own category called PEC
            return CALL_ENTRY_TYPE;
        }


        /**
         * Return the call severity for the banner sounds
         * PECs have a constant severity of 1
         *
         * @return the severity of a PEC call
         */
        virtual int getSeverity() const
        {
            // PEC severity is always 1
            return 1;
        }

        /**
         * @return true if this is an emergency call
         */
        virtual bool isEmergency() const;

        /**
         * Get the PEC source string to go into the banner source field
         *
         * @return the PEC source string suitable for the banner
         */
        std::string getPecSourceString() const;


        /**
         * Get the PEC information string to go into the banner information field
         *
         * @return the PEC information string suitable for the banner
         */
        std::string getPecInformationString() const;


        /**
         * Gets the status of this PEC for the banner status field
         *
         * @return the PEC status string suitable for the banner
         */
        virtual std::string getStatusStr() const;


        /**
         * Gets the location of this PEC for the banner location field
         *
         * @return the PEC location string suitable for the banner
         */
        virtual std::string getLocationStr() const;


        /**
         * Gets the creation time of this PEC for the banner time field
         *
         * @return the PEC creation time
         */
        virtual time_t getCreateTime() const;


        /**
         * Gets the PEC details for the PEC dialog.
         *
         * @return the string to display in the PEC field of the PEC dialog
         */
        virtual std::string getPECDetailString() const;


        /**
         * Gets the location details for the PEC dialog.
         *
         * @return the string to display in the location field of the PEC dialog
         */
        virtual std::string getLocationDetailString() const;


        /**
         * Gets the train details for the PEC dialog.
         *
         * @return the string to display in the train field of the PEC dialog
         */
        virtual std::string getTrainDetailString() const;


        /**
         * Checks if CCTV is available at this location.
         * If false, cctv will not be activated for a PEC, and the CCTV button on the PEC banner will be greyed out.
         *
         * @return true if CCTV should be activated, false otherwise
         */
        bool isCctvAvailable();


        /**
         * Get the train this PEC is for
         *
         * @return the train ID
         */
        CommonTypes::TrainIdType getTrainId();

		/**
         * Get the train Primary Tsi
         *
         * @return the primaryTsi
         */
		unsigned long getPrimaryTsi();

		/**
         * Get the train Secondary Tsi
         *
         * @return the secondaryTsi
         */
		unsigned long getSecondaryTsi();


        /**
         * Get the operator session associated with this PEC, if any
         *
         * @return the operator who has answered this PEC, or "" otherwise
         */
        std::string getSessionId();


        /**
         * Get the source associated with this PEC
         *
         * @return whether this PEC is answered by driver, operator or none
         */
        PecTypes::EPecSource getPecSource();


        /**
         * This method is called to retrieve a list of menu items to be displayed in a context menu
         *
         * @return a list of strings, each string representing a menu entry
         */
        virtual std::vector<std::string> getContextMenuItems();


        /**
         * This method is called when a ContextMenu item was selected.
         *
         * @param command    The menu item that was selected
         */
        virtual void onContextMenuCommand( std::string command );


        /**
         * The train details for this PEC have changed. The banner and possibly the PEC
         * dialog need to be updated.
         */
        void trainDetailsChanged();


        /**
         * Updates this PEC with new status information
         *
         * @param update    The new PEC details
         */
        void processUpdate( const PecTypes::PecUpdate& update );


        /**
         * This is called when the call for the PEC in progress has been dropped.
         * It simply notifies the operator so they can act on the situation.
         */
        void processCallLost();


        /**
         * This is called when the call for the PEC in progress has been reset.
         * Will notify the operator if this entry has an active PEC dialog, otherwise it
         * will do nothing and allow this call entry to be removed.
         */
        void processReset();


        /**
         * This is called when the call for the PEC in progress has been reset due to MPU changeover
         * Will notify the operator if this entry has an active PEC dialog, otherwise it will do nothing and allow this call entry to be removed.
         */
        void processMpuChangeover();


        /**
         * Called by the dialog when the CCTV button is pressed.
         */
        virtual void dialogCctvButtonPressed();


        /**
         * Called by the dialog when the reset button is pressed.
         */
        virtual void dialogResetButtonPressed();


        /**
         * Called by the dialog when the reset all button is pressed.
         */
        virtual void dialogResetAllButtonPressed();

		virtual void dialogHoldButtonPressed();


        /**
         * This method is called when the call entry is activated,
         * for example when the user double-clicks on it.
         */
        virtual void onActivate();


        /**
         * This is the actual answer method called by the PecCallInitialiser.
         * It will answer the PEC and activate CCTV.
         */
        void threadedAnswer();

// 		/**
//          * This is the actual answer method called by the PecCallInitialiser.
//          * It willactivate CCTV.
//          */
//         void threadedCCTV();

		
		/**
		*	This method is called when user right click on a selected call item
		*	and select a Reset All PEC
		*	This will reset all PEC call that has the same train id
		*	Note the "Reset All" will only show if the call is valid
		**/
		virtual void onForceReset ();

		
		/**
		*	Evaluate the Train source 
		*	to determine if the train is coupled or not
		*/
		bool isCoupledTrain ();

		
		/**
		*	This method is called when user right click on a selected call item
		*	that is invalid and select Clear
		*	This will reset selected invalid call
		*	Note the "Clear" option will only show if the status is invalid
		**/
		virtual void onClearCall  ();
		
		virtual void onCallTermination ();

		virtual void onCallSetupFailed ();

    private:

        /**
         * Private constructors
         */
        PECCallEntry();
        PECCallEntry( const PECCallEntry& thePECCallEntry );


        /**
         * Internal helper function to display details of caught exceptions
         * (from the catch macro)
         *
         * @param prefixMessage    The operation the failed. Eg "Error answering PEC"
         * @param errorDetails    The description of the error. Eg "Train time out"
         */
        void showExceptionCatchDetails( const std::string& prefixMessage, const std::string& errorDetails );


        /**
         * Shows a message box indicating an error has occurred. The dialog must exist
         * before this method is called or it will assert.
         *
         * @param errorMessage The description of the error. Eg "Train time out"
         */
        void showErrorMessage( const std::string& errorMessage );


        /**
         * This will activate CCTV for this PEC
         * It will handle prompting for location for delocalised trains, and prompting for
         * deactivating a train in conflict.
         */
        void activateCctv();


        /**
         * The context menu item for reset
         */
        static const std::string RESET_MENU_ITEM;


        /**
         * The context menu item for reset all
         */
        static const std::string RESET_ALL_MENU_ITEM;


        /**
         * The context menu item for clear
         */
        static const std::string CLEAR_MENU_ITEM;


        /**
         * The context menu item for clear all
         */
        static const std::string CLEAR_ALL_MENU_ITEM;


        /**
         * PEC Status string
         */
        static const std::string STATUS_DRIVER;


        /**
         * PEC Status string
         */
        static const std::string STATUS_TRAIN_BUSY;


        /**
         * PEC Status string
         */
        static const std::string STATUS_IN_PROGRESS;


        /**
         * PEC Status string
         */
        static const std::string STATUS_ANSWERED;


        /**
         * PEC Status string
         */
        static const std::string STATUS_NEW;


        /**
         * PEC Status string
         */
        static const std::string STATUS_INVALID;


        /**
         * PEC Status string
         */
        static const std::string STATUS_UNKNOWN;


        /**
         * String to use for unknown train location
         */
        static const std::string UNKNOWN_LOCATION_NAME;


        /**
         * Message when an in progress PEC has been taken over by the driver
         */
        static const std::string ANSWERED_BY_DRIVER_MESSAGE;


        /**
         * Error message prefix for errors answering calls
         */
        static const std::string ANSWER_ERROR_PREFIX;


        /**
         * Error message prefix for errors activating cctv
         */
        static const std::string CCTV_ERROR_PREFIX;


        /**
         * Error message prefix for errors deactivating cctv in conflict scenarios
         */
        static const std::string DEACTIVATE_OWNING_TRAIN_ERROR_PREFIX;


        /**
         * Error message prefix for errors resetting pec
         */
        static const std::string RESET_ERROR_PREFIX;


        /**
         * Error message prefix for errors resetting all pec
         */
        static const std::string RESET_ALL_ERROR_PREFIX;


        /**
         * The dialog that is opened when the onActivate method is called. There is only
         * one copy for all PEC call entries.
         */
        static PECDialog* s_dialog;


        /**
         * this prevents object deletion while a reset operation is in progress ie a reset
         * notification is received before the reset method has finished This is also used
         * to prevent a "This call has been reset" message if its this user resetting the
         * call
         */
        TA_Base_Core::ReEntrantThreadLockable m_resetLock;


        /**
         * The PEC point that is the source of the call
         */
        PecTypes::PecUpdate m_pecDetails;


        /**
         * A reference to the call factory that created this call entry.
         */
        PECCallFactory& m_pecCallFactory;


        /**
         * A pointer to the train pec corba proxy interface
         */
        TrainPecCorbaProxyPtr m_trainPecCorbaProxy;


        /**
         * The thread that will be used to answer the PEC
         */
        PECCallInitialiser* m_pecCallInitialiser;

// 		/**
//          * The thread that will use for activating the CCTV
//          */
//         PECCallCCTV* m_pecCallCCTV;

		static const std::string TRAINLOC_INNER_PREFIX;
		static const std::string TRAINLOC_OUTER_PREFIX;


	public:
		virtual bool isCallInvalid ();

		virtual bool isCallActive ();

		void callCannotSetupExcepProc ();

	private:

		bool m_bIsActive;
		bool m_bIsCallTerminated;
		bool m_bOnGoingChangeOverProc;
    };

}

#endif // !defined(PECCallEntry_H)
