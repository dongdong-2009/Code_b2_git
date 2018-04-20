/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/ITIRatisTag.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  */
#ifndef ITI_RATIS_TAG_H_INCLUDED
#define ITI_RATIS_TAG_H_INCLUDED

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class ITiRatisTag : public virtual TA_Base_Core::IItem
    {
    public:
	
		virtual std::string	getTagGroup() = 0;

		virtual std::string	getDetail() = 0;

		virtual std::string	getRatisTag() = 0;

        virtual ~ITiRatisTag() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<ITiRatisTag*> ITiRatisTags;

    typedef ITiRatisTags::iterator    ITiRatisTagsIt;

} //close namespace TA_IRS_Core


#endif // !defined(ITiRatisTag_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
