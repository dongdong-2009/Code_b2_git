/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/IECSZone.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * IECSZone is an interface to an ECSZone object. It allows the ECSZone object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IECSZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IECSZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IECSZone : public virtual TA_Base_Core::IItem
    {

    public:

        virtual int getECSZoneId() =0;
        virtual std::string getECSLocation() =0;
        virtual std::string getTrack() =0;
        virtual unsigned long getOccEquipmentEntityKey() =0;
        virtual unsigned long getStnEquipmentEntityKey() =0;	
        virtual bool getIsIgnoringCongestion() =0;	


        virtual ~IECSZone() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IECSZone*> IECSZones;
    typedef IECSZones::iterator    IECSZonesIt;
} //close namespace TA_IRS_Core

#endif // !defined(IECSZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
