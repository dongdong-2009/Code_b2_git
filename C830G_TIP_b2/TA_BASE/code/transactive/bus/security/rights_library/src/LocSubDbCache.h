#ifndef LOCSUBDBCACHE_H_INCLUDED
#define LOCSUBDBCACHE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/security/rights_library/src/LocSubDbCache.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/17 16:55:38 $
  *  Last modified by:  $Author: shurui.liu $
  *
  *  Caches locations and subsystems associated with resources from the database.
  */

namespace TA_Base_Bus
{
	class LocSubDbCache
	{
	public:
		LocSubDbCache();
		~LocSubDbCache();

		unsigned long getLocation ( unsigned long resourceKey );
		unsigned long getSubsystem( unsigned long resourceKey );
		bool getLocSub(unsigned long  recoureKey, unsigned long & location, unsigned long & subsystem);

	private:

		LocSubDbCache( const LocSubDbCache& );
		LocSubDbCache& operator=( const LocSubDbCache& );
		typedef std::map<unsigned long, TA_Base_Core::IEntityData*> DbMap;
		DbMap m_dbMap;
		
		static const unsigned long UNALLOCATED_SUBSYSTEM;

		DbCacheLookup< unsigned long, TA_Base_Core::IEntityData, TA_Base_Core::EntityAccessFactory> m_dbLookup;
	};
}

#endif // LOCSUBDBCACHE_H_INCLUDED
