/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/uuid/src/TAuuid.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * TA_Uuid is used to generate and compare UUID's (Universally Unique Identifier).
  */

#include "core/uuid/src/TAuuid.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

namespace TA_Base_Core
{
    /**
      * generateUUID()
      *
      * generateUUID will generate a UUID and return it as a std::string
      *
      * @return std::string representation of the UUID
      */
    std::string TA_Uuid::generateUUID()
    {
        static boost::uuids::random_generator gen;
        return boost::to_upper_copy(to_string(gen()));
    }

    std::string TA_Uuid::uuid()
    {
        return generateUUID();
    }

    /**
	  * compareUUID()
	  * 
	  * compareUUID will compare to strings representing UUIDs.
	  *
	  * @return bool true if UUIDs are the same, false otherwise
	  */
	bool TA_Uuid::compareUUID(const std::string& subject, const std::string& test)
	{
		// Use a simple string comparison... not particularly efficient, but it works
		return ( subject.compare(test) == 0);
	}

};
