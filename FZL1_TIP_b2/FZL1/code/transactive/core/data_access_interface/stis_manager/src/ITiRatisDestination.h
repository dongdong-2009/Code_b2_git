/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/ITiRatisDestination.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef ITI_RATIS_DESTIONATION_H_INCLUDED
#define ITI_RATIS_DESTIONATION_H_INCLUDED

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class ITiRatisDestination : public virtual TA_Base_Core::IItem
    {
    public:
	
		virtual std::string	getDestination() = 0;

        virtual ~ITiRatisDestination() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<ITiRatisDestination*> ITiRatisDestinations;

    typedef ITiRatisDestinations::iterator    ITiRatisDestinationsIt;

} //close namespace TA_IRS_Core


#endif // !defined(ITiRatisDestination_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
