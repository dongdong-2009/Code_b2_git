/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/TisCachedMaps.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by: $Author: qi.huang $
  * 
  * Maintains all the cached maps for PA applications
  *
  */

#if !defined(TIS_CACHED_MAPS_INCLUDED)
#define TIS_CACHED_MAPS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/pa/cachedmaps/src/TisCachedMapDefinitions.h"

namespace TA_IRS_Bus
{   
    class TisCachedMaps
    {
     public:
        /**
         * getInstance
	     *
	     * Returns an instance of the class
         *
         * @return the pointer to this singleton instance
         *
         */
	    static TisCachedMaps& getInstance();

        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         *
         */
        static void removeInstance();

        /** 
         * refreshPaTisMapping
         *
         * Reads the PA_ZONE_HELPER and PA_TIS_MESSAGE_MAP tables and populates the record map collection
         * for all records matching the optional locationKey.  To update the map with all records, 
         * do not specify the the parameter.
         *
         * @param  ulLocationKey (optional) filters the table on the LOCATIONKEY field.
		 *         isMaster (optional) is OCC mode if true
         *
         */ 
		void refreshPaTisMapping( unsigned long ulLocationKey, bool bAllZones = false );

        /** 
         * isPADvaMsgMapped
         *
         * Judge whether this message is Tis Message or not.
         *
         * @param  ulMsgKey is Pa DVA message key.
         *
		 * @return  Return true if this message is tis message
		 *
         */
		bool isPADvaMsgMapped(unsigned long ulMsgKey);

		/** 
         * isPAZoneMapped
         *
         * Judge whether this zone is Tis zone or not.
         *
         * @param  ulZoneKey  is Pa zone key.
         *
		 * @return  Return true if this zone is tis zone
		 *
         */
		bool isPAZoneMapped(unsigned long ulZoneKey);

        /** 
         * areAllPaDvaMsgMapped
         *
         * Judge whether all messages are Tis messages or not.
         *
         * @param  vecMsgKeys  is all Pa messages' key collections.
         *
		 * @return  Return true if all messages are tis messages
		 *
         */
		bool areAllPaDvaMsgMapped(const std::vector<unsigned long>& vecMsgKeys);

		/** 
         * areAllPaZoneMapped
         *
         * Judge whether all zones are Tis zone or not.
         *
         * @param  vecZoneKeys  is all Pa zones' key collections.
         *
		 * @return  Return true if all zones are tis zones
		 *
         */
		bool areAllPaZoneMapped(const std::vector<unsigned long>& vecZoneKeys);

		/** 
         * getTisMsgsByPADvaMsgKey
         *
         * Return all tis messages via PaDvaMsgKey filtering.
         *
         * @param  ulMsgKey is Pa DVA message key..
         *
		 * @return  Return tis messages
		 *
         */
		TisMessages getTisMsgsByPADvaMsgKey(unsigned long ulMsgKey);

		/** 
         * getTisZonesByPaZoneKey
         *
         * Return all tis zones via PaZoneKey filtering.
         *
         * @param  ulZoneKey is Pa zone key..
         *
		 * @return  Return tis zones
		 *
         */
		TisZones getTisZonesByPaZoneKey(unsigned long ulZoneKey);

    protected:
        TisCachedMaps();
	    virtual ~TisCachedMaps();

        static TisCachedMaps* s_pInstance;
        static TA_Base_Core::NonReEntrantThreadLockable s_lockForSingleton;

		// Synchronization locks for each of the maps
        TA_Base_Core::NonReEntrantThreadLockable m_lockForZoneMaps;
        TA_Base_Core::NonReEntrantThreadLockable m_lockForMessageMaps;

		std::map<unsigned long, TisMessages> m_mapPaDvaToTisMsgs;
		std::map<unsigned long, TisZones> m_mapPaZoneToTisZones;
    };

} // end namespace TA_IRS_Bus
    
    
#endif // !defined(TIS_CACHED_MAPS_INCLUDED)
