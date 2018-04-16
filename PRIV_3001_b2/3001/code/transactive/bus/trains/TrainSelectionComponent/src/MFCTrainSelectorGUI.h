/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is an extension to train selector.
  * The GUI doesnt have to implement ITrainSelectorGUI
  * This handles the update calls and posts messages to the specified GUI.
  *
  * Windows post message is used to deliver the updates to the GUI. The GUI is responsible
  * for deleting the given structure. A pointer to the data, stored on the heap is given as wParam.
  *
  * Simply create an MFCTrainSelectorGUI object, and pass it to the TrainSelector's constructor.
  * Handle the messages specified, and delete the given structure when done, failing to delete
  * the structure, or to handle the messages will result in memory leaks.
  *
  */

#if !defined(MFC_TRAIN_SELECTOR_H)
#define MFC_TRAIN_SELECTOR_H

#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"
#include "bus/trains/TrainSelectionComponent/src/TrainList.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>
#include <string>


namespace TA_IRS_Bus
{

    class MFCTrainSelectorGUI : public ITrainSelectorGUI
    {

    public:

        typedef struct
        {
			ITrainSelectorGUI::EAction action;
            TrainInformationTypes::TrainDetails trainDetails;
        } TrainListUpdate;


        typedef struct
        {
            std::vector<std::string> trainListNames;
        } SelectionListUpdate;

        /** 
          * MFCTrainSelectorGUI
          *
          * Constructor.
          * Be sure to actually handle the given messages, and delete structures passed in
          * as wParam when done handling them.
          *
          * @param callback The window to post the update messages to
          * @param updateInboundMessage The message number to send inbound updates as
          * @param updateOutboundMessage The message number to send outbound updates as
          * @param updateSelectionMessage The message number to send selection updates as
          *
          */
        MFCTrainSelectorGUI(CWnd* callback,
                            int updateInboundMessage,
                            int updateOutboundMessage,
                            int updateSelectionMessage);

        
        /** 
          * MFCTrainSelectorGUI
          *
          * Constructor.
          * Be sure to actually handle the given messages, and delete structures passed in
          * as wParam when done handling them.
          *
          * @param windows The list of windows to post the message to
          * @param updateInboundMessage The message number to send inbound updates as
          * @param updateOutboundMessage The message number to send outbound updates as
          * @param updateSelectionMessage The message number to send selection updates as
          *
          */
        MFCTrainSelectorGUI(std::vector<CWnd*> windows,
                            int updateInboundMessage,
                            int updateOutboundMessage,
                            int updateSelectionMessage);

        
        /** 
          * MFCTrainSelectorGUI
          *
          * Constructor.
          * Be sure to actually handle the given messages, and delete structures passed in
          * as wParam when done handling them.
          * Windows can be added via the addCallBackWindow() method.
          *
          * @param updateInboundMessage The message number to send inbound updates as
          * @param updateOutboundMessage The message number to send outbound updates as
          * @param updateSelectionMessage The message number to send selection updates as
          *
          */
        MFCTrainSelectorGUI(int updateInboundMessage,
                            int updateOutboundMessage,
                            int updateSelectionMessage);

        
        virtual ~MFCTrainSelectorGUI(){};

        
        /**
         * addCallbackWindow
         *
         * Adds an additional callback window to the list of windows to be notified when a train update
         * occurs.
         * NOTE: Additional windows are limited to listening to the same TYPE of messages as the other windows
         * (i.e. the station type is the same), and the message number will be the same across windows as well.
         *
         * @param callbackWindow The window to post the update message to
         */
        virtual void addCallbackWindow( CWnd* callbackWindow );


        /**
         * removeCallbackWindow
         *
         * Removes a callback window from the list of windows to be notified of train updates.
         *
         * @param callbackWindow The window to remove from the update list.
         */
        virtual void removeCallbackWindow( CWnd* callbackWindow );


        /**
         * Updates the list of inbound trains for this GUI
         * Posts the message and a pointer to a TrainListUpdate structure
         * is given as wParam. It needs to be cast to a TrainListUpdate*,
         * handled, and then deleted.
         *
         * @param action        the action to take (see EAction definition).
         * @param trainDetails  the new details of the train if this is an Add
         *                      or Update.
         * 
         */
        virtual void updateInboundList( EAction action,
										const TrainInformationTypes::TrainDetails& trainDetails );

        
        /**
         * Updates the list of outbound trains for this GUI
         * Posts the message and a pointer to a TrainListUpdate structure
         * is given as wParam. It needs to be cast to a TrainListUpdate*,
         * handled, and then deleted.
         *
         * @param action        the action to take (see EAction definition).
         * @param trainDetails  the new details of the train if this is an Add
         *                      or Update.
         * 
         */
        virtual void updateOutboundList( EAction action,
										 const TrainInformationTypes::TrainDetails& trainDetails );


		/**
		 * Updates the status of fallback mode
		 * 
		 * @param inFallback    Whether the local radio is in fallback mode or not
		 */
        virtual void updateFallbackStatus( bool inFallback ) { return; }


		/**
		* Gets the selected trains (from both lists)
		* 
		* @return A list of train details structures
		*/
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const;

        
        /**
         * Called when a selection group is added or deleted. The GUI should
         * update its selection combo with the list of train selection list
         * names.
         * Posts the message and a pointer to a SelectionListUpdate structure
         * is given as wParam. It needs to be cast to a SelectionListUpdate*,
         * handled, and then deleted.
         *
         * @param trainListNames  List of names of train selection lists that
         *                        are available.
         * 
         */
        virtual void updateSelectionCombo( const std::vector<std::string>& trainListNames );


		/**
		 * This is used by the train selector to determine if it needs to update radio groups or not.
		 * 
		 * @return true if radio group updates are needed
		 */
        virtual bool hasRadioGroupSupport() { return false; }


		/**
		* Called when a radio group is enabled or disabled
		* The GUI should update its selection combo with the list of radio group names.
		* 
		* @param trainListNames    List of names of train selection lists that are available.
		*/
        virtual void updateRadioGroupCombo( const std::vector<std::string>& trainListNames ){return;}

        /**
         * getInboundMessageId (simple accessor)
         */
        int getInboundMessageId() const { return m_updateInboundMessage; }

        /**
         * getOutboundMessageId (simple accessor)
         */
        int getOutboundMessageId() const { return m_updateOutboundMessage; }

        /**
         * getUpdateSelectionId (simple accessor)
         */
        int getUpdateSelectionId() const { return m_updateSelectionMessage; }

		
		/**
		 * isLocationInControl();
		 */
		bool isLocationInControl(unsigned long trainLocation);
		
    private:

        /** 
          * MFCTrainSelectorGUI
          *
          * default constructor
          *
          */
        MFCTrainSelectorGUI();


        /** 
          * postUpdates
          *
          * Post the inbound/outbound list update to all callback windows
          *
          * @param action the action (add/delete etc)
          * @param trainDetails the train details structure
          * @param messageNumber the message number to post,
          *        m_updateInboundMessage or m_updateOutboundMessage
          *
          */
        void postUpdates( EAction action,
						  const TrainInformationTypes::TrainDetails& trainDetails,
                          int messageNumber );

        /** 
          * postUpdate
          *
          * Post the inbound/outbound list update to one window
          *
          * @param action the action (add/delete etc)
          * @param trainDetails the train details structure
          * @param messageNumber the message number to post,
          *        m_updateInboundMessage or m_updateOutboundMessage
          * @param window the window to post to
          */
        void postUpdate( EAction action,
						 const TrainInformationTypes::TrainDetails& trainDetails,
                         int messageNumber,
                        CWnd* window) const;


        /** 
          * postUpdates
          *
          * Post the train selection list update to all callback windows.
          *
          * @param trainListNames a vector of selection names
          * @param messageNumber the message to post. m_updateSelectionMessage
          *
          */
        void postUpdates(const std::vector<std::string>& trainListNames,
                        int messageNumber);


        /** 
          * postUpdate
          *
          * Post the train selection list update to one window
          *
          * @param trainListNames a vector of selection names
          * @param messageNumber the message to post. m_updateSelectionMessage
          * @param window the window to post to
          *
          */
        void postUpdate(const std::vector<std::string>& trainListNames,
                        int messageNumber,
                        CWnd* window) const;


        /** 
          * synchroniseWindow
          *
          * After adding a window, it wont have the current list of
          * trains, or the current group selection list. This will bring it
          * up to speed with the rest of the callback windows.
          *
          * @param callbackWindow   the window to synchronise.
          */
        void synchroniseWindow( CWnd* callbackWindow );


        // the messages to send
        int m_updateInboundMessage;
        int m_updateOutboundMessage;
        int m_updateSelectionMessage;

        // the cache of trains to give to newly registered windows
        TrainList m_inboundList;
        TrainList m_outboundList;

        // the most recently received train selection list update
        // this is so when a window registers after updates have come through
        // it can receive the latest list
        std::vector<std::string> m_currentSelectionList;

		// the list of registered windows
        std::vector<CWnd*> m_callbackWindows;

        // the lock for the window list
        TA_Base_Core::NonReEntrantThreadLockable m_callbackListLock;

        // the lock for the selection list
        TA_Base_Core::NonReEntrantThreadLockable m_selectionListLock;

    };

} // TA_IRS_Bus

#endif // !defined(MFC_TRAIN_SELECTOR_H)
