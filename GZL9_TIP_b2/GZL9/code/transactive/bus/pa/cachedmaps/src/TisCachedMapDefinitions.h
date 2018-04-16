/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/TisCachedMapDefinitions.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by: $Author: qi.huang $
  * 
  * Provides some definitions used by the various tis cached maps 
  * (for use by client code)
  *
  */

#ifndef _TIS_CACHED_MAP_DEFINITIONS_HEADER_
#define _TIS_CACHED_MAP_DEFINITIONS_HEADER_

#include<string>
#include <list>

namespace TA_IRS_Bus
{
   struct TisMessage
    {
        unsigned long ulKey;
		unsigned long ulTisLibrarySection;
		unsigned long ulTisLibraryVersion;
		unsigned long ulTisMessageTag;
    };
    typedef std::list<TisMessage>   TisMessages;
    typedef TisMessages::iterator   TisMessagesIt;

    struct TisZone
    {
		unsigned long ulKey;
		std::string strEquipmentEntity;
    };
    typedef std::list<TisZone>      TisZones;
    typedef TisZones::iterator      TisZonesIt;

} // end namespace TA_IRS_Bus

#endif // _CACHED_MAP_DEFINITIONS_HEADER_