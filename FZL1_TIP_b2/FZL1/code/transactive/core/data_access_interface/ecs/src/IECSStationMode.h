/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/IECSStationMode.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IECSStationMode is an interface to an ECSStationMode object. It allows the ECSStationMode object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IECSStationMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IECSStationMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IECSStationMode : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getStationMode() =0;
        virtual unsigned long getLocationKey() =0;
        virtual std::string getOperation() =0;
        virtual std::string getTrack() =0;
        virtual std::string getCondition() =0;
        virtual std::string getCongestionLogic() =0;

        virtual ~IECSStationMode() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IECSStationMode*> IECSStationModes;
    typedef IECSStationModes::iterator    IECSStationModesIt;

} //close namespace TA_IRS_Core


#endif // !defined(IECSStationMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
