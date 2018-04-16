/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/IPidStationId.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IPidStationId is an interface to an PidStationId object. It allows the PidStationId object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IPidStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPidStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IPidStationId : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getStationId() =0;
        virtual unsigned long getTaLocation() =0;
        
        virtual ~IPidStationId() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IPidStationId*> IPidStationIds;
    typedef IPidStationIds::iterator    IPidStationIdsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IPidStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
