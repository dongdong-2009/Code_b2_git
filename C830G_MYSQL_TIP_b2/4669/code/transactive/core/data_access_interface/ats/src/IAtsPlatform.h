/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/IAtsPlatform.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IAtsPlatform is an interface to an AtsPlatform object. It allows the AtsPlatform object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IAtsPlatform_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IAtsPlatform_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IAtsPlatform : public virtual TA_Base_Core::IItem
    {

    public:

		virtual unsigned long getPlatformId() =0;
		virtual unsigned long getStationId() =0;


        /** 
          * getTisPlatformId
          *
          * gets the platform ID of this platform for sending
          * platform information to the STIS server.
          *
          * @return a single digit number (1-9)
          */
        virtual unsigned char getTisPlatformId() =0;

        
        virtual ~IAtsPlatform() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IAtsPlatform*> IAtsPlatforms;
    typedef IAtsPlatforms::iterator    IAtsPlatformsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IAtsPlatform_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
