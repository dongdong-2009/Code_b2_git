/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/LibraryVersionMonitor.h $
  * @author:  Kent Yip
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/04/22 17:18:57 $
  * Last modified by:  $Author: huang.wenbo $
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
		CURRENT_VERSION = 0,
		NEXT_VERSION = 1,
		CURRENT_ISCS_VERSION = 0,
		NEXT_ISCS_VERSION = 1,
		CURRENT_STIS_VERSION = 2,
		NEXT_STIS_VERSION = 3
	};

	struct LibraryVersionUpdateInfo
	{
		ELibraryVersionPointType type;
		unsigned long locationKey;
		unsigned long newValue;
		unsigned long entityKey;
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
		virtual std::map<unsigned long, unsigned long> getAllLibraryVersions();

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
         * setupVersionInfo
         *
         * Registers for datapoint state changes for the library version.
         *
         * 
         */
        void setupVersionInfo();
        
    protected:
        
		bool isNoListener();
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
        void notifyAllListeners( unsigned long entityKey, const LibraryVersionUpdateInfo& updateInfo);

        // Datapoints for each location
		std::map<unsigned long,LibraryVersionDataPointInfo*> m_libraryVersionDatapoints;
                
        // the listeners that must be notified of updates to the loaded library
        std::vector<ILibraryVersionListener*> m_listeners;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

		std::map<std::string,ELibraryVersionPointType> m_dpPostfix;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory;
    };

} // end namespace TA_IRS_Bus

#endif 

