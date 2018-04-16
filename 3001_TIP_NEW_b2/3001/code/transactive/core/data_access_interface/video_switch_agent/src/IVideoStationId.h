/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/IVideoStationId.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * IVideoStationId is an interface to an VideoStationId object. It allows the VideoStationId object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IVideoStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IVideoStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IVideoStationId : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getStationId() =0;
        virtual unsigned long getTaLocation() =0;
        
        virtual ~IVideoStationId() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IVideoStationId*> IVideoStationIds;
    typedef IVideoStationIds::iterator    IVideoStationIdsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IVideoStationId_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
