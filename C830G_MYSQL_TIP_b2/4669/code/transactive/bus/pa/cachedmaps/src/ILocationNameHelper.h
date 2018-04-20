/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/ILocationNameHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Simple interface function to allow retrieval of a location name from a key value
  *
  */

#pragma once

namespace TA_IRS_Bus
{
    class ILocationNameHelper
    {
    public:
        /**
         * getLocationNameFromKey
         *
         * @return the name of the location identified by 'locationKey'
         *
         * @exception TransactiveException if the input key is invalid
         *
         */
        virtual std::string getLocationNameFromKey(unsigned long locationKey) = 0;
    };

} // end namespace TA_IRS_Bus