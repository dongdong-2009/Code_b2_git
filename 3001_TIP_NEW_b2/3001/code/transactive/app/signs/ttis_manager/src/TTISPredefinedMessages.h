/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISPredefinedMessages.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to load the current TTIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  * If a list of messages is kept, the GUI keeping the messages should
  * register itself with this object to be notified when the pre-defined messages
  * are re-loaded. A WM_UPDATE_CURRENT_TTIS_VERSION will be sent.
  *
  * This will also receive notification of changes in the next message library version.
  * Windows interested in this will receive a WM_UPDATE_NEXT_TTIS_VERSION message.
  *
  * This will also receive notification of changes to message library synchronisation.
  * Windows interested in this will receive a WM_UPDATE_LIBRARY_SYNCHRONISED message.
  */

#if !defined(TTISPREDEFINED_MESSAGES_H)
#define TTISPREDEFINED_MESSAGES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

// forward declarations
namespace TA_Base_Bus
{
    class ScadaProxyFactory;
}

namespace TA_IRS_App
{
    class TTISPredefinedMessages : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
        
    public:

        /**
         * ~TTISPredefinedMessages
         *
         * Standard destructor.
         */

        virtual ~TTISPredefinedMessages();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TTISPredefinedMessages&
         *              A reference to an instance of a TTISPredefinedMessages object.
         */
		static TTISPredefinedMessages& getInstance();


        /**
         * removeInstance
         *
         * cleans up the singleton
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
          * will be given a WM_UPDATE_NEXT_TTIS_VERSION message.
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
        inline unsigned short getCurrentMessageLibraryVersion()
        {
            return m_currentTTISLibraryVersion;
        }


         /** 
          * getNextMessageLibraryVersion
          *
          * This returns the current version of the pre-defined message library
          *
          * @return current version of the pre-defined message library
          */
        inline unsigned short getNextMessageLibraryVersion()
        {
            return m_nextTTISLibraryVersion;
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
          * Get the description/name of each normal priority
          * pre-defined message
          *
          *
          * @return A vector of message names
          */
        inline std::vector<TA_Base_Core::PredefinedMessage> getNormalMessages()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);

            return m_normalTTISPredefinedMessages;
        }
        
        /** 
          * getEmergencyMessages
          *
          * Get the description/name of each emergency priority
          * pre-defined message
          *
          *
          * @return A vector of message names
          */
        inline std::vector<TA_Base_Core::PredefinedMessage> getEmergencyMessages()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);

            return m_emergencyTTISPredefinedMessages;
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


        /** 
          * getDefaultDisplayAttributes
          *
          * Gets the default ad-hoc attributes set in the predefined message library.
          *
          * @return The structure containing the default adhoc attributes.
          */
        inline TA_Base_Core::TTISLedAttributes getDefaultDisplayAttributes()
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);
            return m_defaultAttributes;
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

        
    private:
        
        /**
         * TTISPredefinedMessages
         *
         * Private constructors.
         */
        TTISPredefinedMessages();
        TTISPredefinedMessages& operator=(const TTISPredefinedMessages &);
        TTISPredefinedMessages( const TTISPredefinedMessages& );

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
          * loadTTISPredefinedMessages
          *
          * load all pre-defined messages from the database
          *
          */
        void loadTTISPredefinedMessages();


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
        static TTISPredefinedMessages* m_theClassInstance;

        // reference count
        static unsigned int m_refCounter;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        

        // The current and next pre-defined message library version
        unsigned short m_currentTTISLibraryVersion;
        unsigned short m_nextTTISLibraryVersion;
        bool m_librariesAreSynchronised;

        // the name of the current and next library datapoints
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
        std::vector<TA_Base_Core::PredefinedMessage> m_normalTTISPredefinedMessages;
        
        // a vector of emergency priority pre-defined messages
        std::vector<TA_Base_Core::PredefinedMessage> m_emergencyTTISPredefinedMessages;

        // a map of normal priority pre-defined message id's
        // to the message index
        std::map<int, unsigned int> m_normalMessageIndexFromId;

        // a map of emergency priority pre-defined message id's
        // to the message index
        std::map<int, unsigned int> m_emergencyMessageIndexFromId;


        // the default ttis adhoc attributes for the current message library
        TA_Base_Core::TTISLedAttributes m_defaultAttributes;


        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory;
    };

} // end namespace TA_IRS_App

#endif 
