/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/PredefinedMessageHelper.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Keeps a map of message Ids to the message text
  * for displaying purposes only.
  *
  * If there are any erros during loading,
  * they are logged, and the appropriate message sent to the operator.
  */

#if !defined(TTISPREDEFINED_MESSAGES_H)
#define TTISPREDEFINED_MESSAGES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"


#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
}

namespace TA_IRS_App
{
    class PredefinedMessageHelper : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
        
    public:

        /**
         * ~PredefinedMessageHelper
         *
         * Standard destructor.
         */

        virtual ~PredefinedMessageHelper() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      PredefinedMessageHelper&
         *              A reference to an instance of a PredefinedMessageHelper object.
         *
         */
		static PredefinedMessageHelper& getInstance();


        /** 
          * getTtisMessage
          *
          * Given the id and library section of a ttis message, retrieve the message
          * text corresponding to the id.
          *
          * @param librarySection   The message type.
          * @param messageId        The message Id to search for.
          *
          * @return     the message text (empty if not found)
          */
        std::string getTtisMessage(TA_Base_Core::ELibrarySection librarySection, int messageId);

        /** 
          * getStisMessage
          *
          * Given the id and library section of an stis message, retrieve the message
          * text corresponding to the id.
          *
          * @param librarySection   The message type.
          * @param messageId        The message Id to search for.
          *
          * @return     the message text (empty if not found)
          */
        std::string getStisMessage(TA_Base_Core::ELibrarySection librarySection, int messageId);


        /** 
          * getTtisLibraryVersion
          *
          * gets the current ttis library version.
          *
          */
        unsigned short getTtisLibraryVersion();


        /** 
          * getStisLibraryVersion
          *
          * gets the current stis library version
          *
          */
        unsigned short getStisLibraryVersion();


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

        /** 
          * getPredefinedMessageForTag
          *
          * Given a pre-defined message tag, get the string for it.
          *
          * @param messageId The message tag
          * @param messageLibrary   The message library (TTIS or STIS)
          * @param librarySection   The library section, normal(0) or emergency(1)
          *                         (ELibrarySection)
          * @param libraryVersion   The library version the display was submitted with
          *
          * @return The text to be displayed
          *
          */
        static std::string getPredefinedMessageForTag(unsigned short messageId,
                                               std::string messageLibrary,
                                               int librarySection,
                                               unsigned short libraryVersion);


        /** 
          * getMessageString
          *
          * Based on the message type, get the message content to display.
          *
          * @param request the display request log item.
          *
          * @return the string to display for the message content
          */
        static std::string getMessageString(TA_IRS_Core::IDisplayRequest* request);

    private:
        
        /**
         * PredefinedMessageHelper
         *
         * Private constructors.
         */
        PredefinedMessageHelper();
        PredefinedMessageHelper& operator=(const PredefinedMessageHelper &) {};
        PredefinedMessageHelper( const PredefinedMessageHelper& ) {};


        /** 
          * loadPredefinedMessages
          *
          * load all pre-defined messages from the database
          * Appropriate errors are presented to the operator.
          */
        void loadPredefinedMessages();


        /** 
          * getMessage
          *
          * find a message in the given map
          *
          * @param messageMap the map to search
          * @param messageId  the message tag to find
          *
          * @return the matching text, empty string if none
          */
        std::string getMessage(std::map<unsigned short, std::string>& messageMap, int messageId);

        /**
          * registerForLibraryChanges
          *
          * registers for datapoint state changes for the library version.
          *
          */
        void registerForLibraryChanges();


        // class instance
        static PredefinedMessageHelper* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // the maps of messages
        std::map<unsigned short, std::string> m_ttisNormalMessages;
        std::map<unsigned short, std::string> m_ttisEmergencyMessages;
        std::map<unsigned short, std::string> m_stisNormalMessages;
        std::map<unsigned short, std::string> m_stisEmergencyMessages;

        // whether the maps have been loaded
        bool m_messagesLoaded;


        unsigned short m_tTisVersion;
        unsigned short m_tSisVersion;

        // the datapoints for the current library version
        TA_Base_Bus::DataPointProxySmartPtr m_currentStationLibraryDp;
        TA_Base_Bus::DataPointProxySmartPtr m_currentTrainLibraryDp;

		// 2008-07-18 to solve the scadaproxyfactory singleton usage
		TA_Base_Bus::ScadaProxyFactory* m_proxyFactory;


    };

} // end namespace TA_App

#endif 
