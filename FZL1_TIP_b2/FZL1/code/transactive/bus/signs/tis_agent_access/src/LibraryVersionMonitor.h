/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/signs/tis_agent_access/src/LibraryVersionMonitor.h $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is used to keep track of the predefined message library
  * version at each location
  *
  */

#ifndef LIBRARY_VERSION_MONITOR_H
#define LIBRARY_VERSION_MONITOR_H

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

namespace TA_IRS_Bus
{
	enum ELibraryVersionPointType
	{
		CURRENT_VERSION,
		NEXT_VERSION
	};

	typedef struct
	{
		ELibraryVersionPointType type;
		unsigned long locationKey;
		TA_Base_Bus::DataPointProxySmartPtr dataPoint;
	} LibraryVersionDataPointInfo;

    class ILibraryVersionListener;

    class LibraryVersionMonitor : public TA_Base_Bus::IEntityUpdateEventProcessor
    {

    public:
        
        /**
         * Returns the entity key associated with an entity name.
         */
    	static unsigned long getEntityKeyFromName( const std::string& entityName );


        /**
         * Fail-safe method to remove an ILibraryVersionListener.
         * This is so that an ILibraryVersionListener that has
         * been deleted or has gone out of scope does not cripple the entire
         * system.
         */
        static void removeFromAllMonitors( ILibraryVersionListener* listener );


        /** 
          * registerForChanges
          *
          * When the next version of the pre-defined message library is updated, window
          * will be given a WM_UPDATE_NEXT_TTIS_VERSION message.
          *
          * @param listener       The ILibraryVersionListener to notify.
          *
          */
        virtual void registerForChanges( ILibraryVersionListener* listener );

        /** 
          * deregisterForChanges
          *
          * window is no longer interested in being notified of pre-defined message library changes.
          *
          * @param listener       The ILibraryVersionListener to notify.
          */
        virtual void deregisterForChanges( ILibraryVersionListener* listener );

	    /** 
          * getAllLibraryVersions
          *
          * This returns the current value of all message library version points
          *
          * @return current value of all message library points
          */
		virtual std::map<unsigned long, unsigned short> getAllLibraryVersions();

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
		
		
		//TD 14395
		//zhou yuan++
        /**
         * setupVersionInfo
         *
         * Registers for datapoint state changes for the library version.
         *
         * 
         */
        void setupVersionInfo();
        
    protected:
        
        /**
         * LibraryVersionMonitor
         *
         * Private constructor.
         *
         * @param dpPostfix The postfix of the datapoint to be added after
         *                  the location.
         */
        LibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix );
        /**
         * ~LibraryVersionMonitor
         *
         * Standard destructor.
         */
        virtual ~LibraryVersionMonitor();

    private:

        LibraryVersionMonitor& operator=(const LibraryVersionMonitor &);
        LibraryVersionMonitor( const LibraryVersionMonitor& );

        /** 
          * notifyWindowsOfChange
          *
          * Sends an update message to all interested windows
          * telling them the current library version has changed.
          *
          */
        void notifyAllListeners( unsigned long entityKey, unsigned short newValue );

        // Datapoints for each location
		std::map<unsigned long,LibraryVersionDataPointInfo*> m_libraryVersionDatapoints;
                
        // the listeners that must be notified of updates to the loaded library
        std::vector<ILibraryVersionListener*> m_listeners;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        static std::vector<LibraryVersionMonitor*> monitors;

		//TD 14395
		//zhou yuan++
		std::map<std::string,ELibraryVersionPointType> m_dpPostfix;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory;
    };

} // end namespace TA_IRS_Bus

#endif 

