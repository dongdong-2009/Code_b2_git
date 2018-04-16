/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/pa_4669/src/IPaTisZoneMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/11/20 09:51:49 $
  * Last modified by: $Author: qi.huang $
  * 
  * IPaTisZoneMap is an interface to a PaTisZoneMap object.  It allows the PA DVA Message object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#ifndef IPATISZONEMAP_INCLUDED_
#define IPATISZONEMAP_INCLUDED_

#include <string>
#include <vector>
#include <map>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaTisZoneMap : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getPaZoneKey() =0;
        virtual std::string getTisZoneEntity() =0;

        virtual ~IPaTisZoneMap() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaTisZoneMap*> IPaTisZoneMaps;
    typedef IPaTisZoneMaps::iterator    IPaTisZoneMapsIt;

} //close namespace TA_Base_Core


#endif // !defined(IPaTisZoneMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
