/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISPredefinedMessages.h $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/08/22 09:25:06 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is used to load the current STIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  * If a list of messages is kept, the GUI keeping the messages should
  * register itself with this object to be notified when the pre-defined messages
  * are re-loaded. A WM_UPDATE_CURRENT_STIS_VERSION will be sent.
  *
  * This will also receive notification of changes in the next message library version.
  * Windows interested in this will receive a WM_UPDATE_NEXT_STIS_VERSION message.
  *
  * This will also receive notification of changes to message library synchronisation.
  * Windows interested in this will receive a WM_UPDATE_LIBRARY_SYNCHRONISED message.
  */

#if !defined(STISPREDEFINED_MESSAGES_H)
#define STISPREDEFINED_MESSAGES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

namespace TA_Base_Bus
{
    class ScadaProxyFactory;
}


namespace TA_IRS_App
{
    class STISPredefinedMessages : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
        
    public:

        /**
         * ~STISPredefinedMessages
         *
         * Standard destructor.
         */

        virtual ~STISPredefinedMessages();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      STISPredefinedMessages&
         *              A reference to an instance of a STISPredefinedMessages object.
         */
		static STISPredefinedMessages* getInstance();

	    /**
	      * removeInstance
	      * 
	      * Terminates singleton instance of factory, releasing all resources 
          * associated with it.
	      */
        static void removeInstance();

        /** 
          * registerCurrentVersionUser
          *
          * When the pre-defined message library is updated, window
          * will be given a WM_UPDATE_PREDEFINEDLIST message. It should re-load its lists.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerCurrentVersionUser(CWnd* window);


        /** 
          * registerNextVersionUser
          *
          * When the next version of the pre-defined message library is updated, window
          * will be given a WM_UPDATE_NEXT_STIS_VERSION message.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerNextVersionUser(CWnd* window);

        /** 
          * registerLibrarySynchronisedUser
          *
          * When the next pre-defined message library version is synchronised, window
          * will be given a WM_UPDATE_LIBRARY_SYNCHRONISED message.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerLibrarySynchronisedUser(CWnd* window);


        /** 
          * deregisterCurrentVersionUser
          *
          * window is no longer interested in being notified of pre-defined message library changes.
          *
          * @param window       The window to be removed
          */
        void deregisterCurrentVersionUser(CWnd* window);


        /** 
          * deregisterNextVersionUser
          *
          * window is no longer interested in being notified of next message library changes.
          *
          * @param window       The window to be removed
          */
        void deregisterNextVersionUser(CWnd* window);

        /** 
          * deregisterLibrarySynchronisedUser
          *
          * window is no longer interested in being notified of library synchronisation changes.
          *
          * @param window       The window to be removed
          */
        void deregisterLibrarySynchronisedUser(CWnd* window);


        /** 
          * getCurrentMessageLibraryVersion
          *
          * This returns the current version of the pre-defined message library
          *
          * @return current version of the pre-defined message library
          */
        inline unsigned long getCurrentMessageLibraryVersion()
        {
            return m_currentSTISLibraryVersion;
        }


         /** 
          * getNextMessageLibraryVersion
          *
          * This returns the current version of the pre-defined message library
          *
          * @return current version of the pre-defined message library
          */
        inline unsigned long getNextMessageLibraryVersion()
        {
            return m_nextSTISLibraryVersion;
        }

        /** 
          * getMessageLibrarySynchronised
          *
          * This returns whether the message libraries are synchronised
          *
          * @return whether the message libraries are synchronised
          */
        inline bool getMessageLibrarySynchronised()
        {
            return m_librariesAreSynchronised;
        }


        /** 
          * getNormalMessages
          *
          * Get a vector of each normal priority
          * pre-defined message
          *
          *
          * @return A vector of messages (still owned by this singleton)
          */
        inline std::vector<TA_Base_Core::PredefinedMessage> getNormalMessages()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);

            return m_normalSTISPredefinedMessages;
        }
        
        /** 
          * getEmergencyMessages
          *
          * Get a vector of each emergency priority
          * pre-defined message
          *
          *
          * @return A vector of messages (still owned by this singleton)
          */
        inline std::vector<TA_Base_Core::PredefinedMessage> getEmergencyMessages()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);

            return m_emergencySTISPredefinedMessages;
        }


        /** 
          * getNormalMessageById
          *
          * Given the id of a normal priority message, retrieve the message
          * corresponding to the id.
          *
          * @param messageId    The message Id to search for.
          *
          * @return     A pointer to the matching message, NULL if not found.
          */
        const TA_Base_Core::PredefinedMessage* getNormalMessageById(int messageId);


        /** 
          * getEmergencyMessageById
          *
          * Given the id of an emergency priority message, retrieve the message
          * corresponding to the id.
          *
          * @param messageId    The message Id to search for.
          *
          * @return     A pointer to the matching message, NULL if not found.
          */
        const TA_Base_Core::PredefinedMessage* getEmergencyMessageById(int messageId);

		inline TA_Base_Core::DisplayAttributes getDefaultStisAttributes()
		{
			TA_Base_Core::ThreadGuard guard(m_messageLock);
			return m_defaultStisAttributes;
		}

        /** 
          * getDefaultLedDisplayAttributes
          *
          * Gets the default ad-hoc attributes set in the predefined message library.
          *
          * @return The structure containing the default adhoc attributes.
          */
        inline TA_Base_Core::LEDAttributes getDefaultLedDisplayAttributes()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);
            return m_defaultLedAttributes;
        }


		inline TA_Base_Core::LCDAttributesSTIS getDefaultLcdDisplayAttributes()
		{
			TA_Base_Core::ThreadGuard guard(m_messageLock);
			return m_defaultLcdAttributes;
		}

        /** 
          * processEntityUpdateEvent
          *
          * implemented from IEntityUpdateEventProcessor.
          * called when a datapoint is updated.
          *
          * @param entityKey
          * @param updateType
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);


		// Build the library initial value and register library change
		void init();

    private:
        
        /**
         * STISPredefinedMessages
         *
         * Private constructors.
         */
        STISPredefinedMessages();
        STISPredefinedMessages& operator=(const STISPredefinedMessages &); // TD11310 ~ edited
        STISPredefinedMessages( const STISPredefinedMessages& ); // TD11310 ~ edited

        static const std::string NEXT_LIBRARY_VERSION_DP_NAME;
        static const std::string CURRENT_LIBRARY_VERSION_DP_NAME;
        static const std::string LIBRARY_SYNCHRONISED_DP_NAME;


        /**
          * registerForLibraryChanges
          *
          * registers for datapoint state changes for the library version.
          *
          */
        void registerForLibraryChanges();


        /** 
          * loadSTISPredefinedMessages
          *
          * load all pre-defined messages from the database
          *
          */
        void loadSTISPredefinedMessages();


        /** 
          * notifyWindowsOfCurrentVersionChange
          *
          * Sends an update message to all interested windows
          * telling them the current library version has changed.
          *
          */
        void notifyWindowsOfCurrentVersionChange();


        /** 
          * notifyWindowsOfNextVersionChange
          *
          * Sends an update message to all interested windows
          * telling them the next library version has changed.
          *
          */
        void notifyWindowsOfNextVersionChange();

        /** 
          * notifyWindowsOfCurrentOCCVersionChange
          *
          * Sends an update message to all interested windows
          * telling them the current library version has changed.
          *
          */
        void notifyWindowsOfCurrentOCCVersionChange();


        /** 
          * notifyWindowsOfNextOCCVersionChange
          *
          * Sends an update message to all interested windows
          * telling them the next library version has changed.
          *
          */
        void notifyWindowsOfNextOCCVersionChange();

        /** 
          * notifyWindowsOfLibrarySynchronisationChange
          *
          * Sends an update message to all interested windows
          * telling them the library synchronisation status has changed.
          *
          */
        void notifyWindowsOfLibrarySynchronisationChange();
        
        /** 
          * postMessageToWindows
          *
          * post the given message to the given vector of windows
          *
          * @param windows  A vector of windows to post the message to
          * @param message  The message number to post
          *
          */
        void postMessageToWindows(const std::vector<CWnd*>& windows, int message);       

        // class instance
        static STISPredefinedMessages* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        // The current and next pre-defined message library version at this location
        unsigned long m_currentSTISLibraryVersion;
        unsigned long m_nextSTISLibraryVersion;		

        bool m_librariesAreSynchronised; // according to the OCC that is

        // the name of the current and next library datapoints at this location
        TA_Base_Bus::DataPointProxySmartPtr m_currentLibraryDp;
        TA_Base_Bus::DataPointProxySmartPtr m_nextLibraryDp;
        TA_Base_Bus::DataPointProxySmartPtr m_librarySynchronisedDp;
        
        // the windows that must be notified of updates to the loaded library
        std::vector<CWnd*> m_currentWindowsToNotify;
        // the windows that must be notified of updates to the next library version
        std::vector<CWnd*> m_nextWindowsToNotify;
        // the windows that must be notified of updates to the library synchronisation
        std::vector<CWnd*> m_syncWindowsToNotify;

        // a vector of normal priority pre-defined messages
        std::vector<TA_Base_Core::PredefinedMessage> m_normalSTISPredefinedMessages;
        
        // a vector of emergency priority pre-defined messages
        std::vector<TA_Base_Core::PredefinedMessage> m_emergencySTISPredefinedMessages;

        // a map of normal priority pre-defined message id's
        // to the message index
        std::map<int, unsigned int> m_normalMessageIndexFromId;

        // a map of emergency priority pre-defined message id's
        // to the message index
        std::map<int, unsigned int> m_emergencyMessageIndexFromId;


        // the default STIS adhoc attributes for the current message library
		TA_Base_Core::DisplayAttributes m_defaultStisAttributes;
        TA_Base_Core::LEDAttributes m_defaultLedAttributes;
		TA_Base_Core::LCDAttributesSTIS m_defaultLcdAttributes;



        // threadlock for registration/deregistration
        TA_Base_Core::ReEntrantThreadLockable m_windowListLock;

        // threadlock for message lists and maps
        TA_Base_Core::ReEntrantThreadLockable m_messageLock;

		std::string m_locationName;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory; // TD11310 ~ added

        static int m_referenceCount;   // TD11310 ~ added
    };

} // end namespace TA_IRS_App

#endif 
