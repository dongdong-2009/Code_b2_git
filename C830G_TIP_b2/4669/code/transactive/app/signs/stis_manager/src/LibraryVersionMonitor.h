/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionMonitor.h $
  * @author:  Rob Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/08 14:06:20 $
  * Last modified by:  $Author: builder $
  *
  * This is used to keep track of the STIS predefined message library
  * version at each location
  *
  */

#if !defined(LIBRARY_VERSION_MONITOR_H)
#define LIBRARY_VERSION_MONITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_UPDATE_LIBRARY_VERSION  (WM_USER + 113)



#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

// TD11310 ~ added fwd decl.
namespace TA_Base_Bus
{
    class ScadaProxyFactory;
}


namespace TA_IRS_App
{
		// ISCS library versions
	const std::string CURRENT_ISCS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion");
    const std::string NEXT_ISCS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion");


	// STIS server library versions
    const std::string CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-CurrentSTISLibraryVersion");
    const std::string NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-NextSTISLibraryVersion");

	//hongran++ TD17500
	//******************* Central DB lib versions of STIS *****************************************************
    const std::string CENTRAL_CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME("OCC.TIS.STIS.SEV.aiiTISC-CurrentSTISLibraryVersion-CDB");
    const std::string CENTRAL_NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME("OCC.TIS.STIS.SEV.aiiTISC-NextSTISLibraryVersion-CDB");
	//++hongran TD17500

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

	typedef struct
	{
		unsigned long entityKey;
		unsigned short value;
	} LibraryVersionInfo;

	typedef struct
	{
		std::string name;
		unsigned long locationKey;

		// entitykeys of the particular library version datapoints at this location
		unsigned long currentISCSLibraryVersionKey;  
		unsigned long nextISCSLibraryVersionKey;
		unsigned long currentSTISLibraryVersionKey;
		unsigned long nextSTISLibraryVersionKey;
	} LocationInfo;

    class LibraryVersionMonitor : public TA_Base_Bus::IEntityUpdateEventProcessor
    {

    public:

        /**
         * ~LibraryVersionMonitor
         *
         * Standard destructor.
         */

        virtual ~LibraryVersionMonitor();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      LibraryVersionMonitor&
         *              A reference to an instance of a LibraryVersionMonitor object.
         */
		static LibraryVersionMonitor* getInstance();

        // TD11310 ~ added        
	    /**
	      * removeInstance
	      * 
	      * Terminates singleton instance of factory, releasing all resources 
          * associated with it.
	      */
        static void removeInstance();

        /** 
          * registerForChanges
          *
          * When the next version of the pre-defined message library is updated, window
          * will be given a WM_UPDATE_NEXT_STIS_VERSION message.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerForChanges( CWnd* window );

        /** 
          * deregisterForChanges
          *
          * window is no longer interested in being notified of pre-defined message library changes.
          *
          * @param window       The window to be removed
          */
        void deregisterForChanges( CWnd* window );


	    /** 
          * getLibraryVersion
          *
          * This returns the current value of the predefined message library datapoint
		  * identified by entityKey
          *
          * @return current version of the supplied message library datapoint
          */
        unsigned short getLibraryVersion( unsigned long entityKey );

	    /** 
          * getAllLibraryVersions
          *
          * This returns the current value of all message library version points
          *
          * @return current value of all message library points
          */
		std::map<unsigned long, unsigned short> getAllLibraryVersions();

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
         * LibraryVersionMonitor
         *
         * Private constructors.
         */
        LibraryVersionMonitor();
        LibraryVersionMonitor& operator=(const LibraryVersionMonitor &); // TD11310 ~ edited
        LibraryVersionMonitor( const LibraryVersionMonitor& );  // TD11310 ~ edited

        /**
          * setupVersionInfo
          *
          * registers for datapoint state changes for the library version.
          *
          */
        void setupVersionInfo();

        /** 
          * notifyWindowsOfChange
          *
          * Sends an update message to all interested windows
          * telling them the current library version has changed.
          *
          */
        void notifyWindowsOfChange( unsigned long entityKey, unsigned short newValue );
         
        // class instance
        static LibraryVersionMonitor* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        // Datapoints for each location
		std::map<unsigned long,LibraryVersionDataPointInfo*> m_libraryVersionDatapoints;
                
        // the window that must be notified of updates to the loaded library
	    CWnd* m_windowToNotify;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory; // TD11310 ~ added

        static int m_referenceCount;  // TD11310 ~ added
    };

} // end namespace TA_IRS_App

#endif 
