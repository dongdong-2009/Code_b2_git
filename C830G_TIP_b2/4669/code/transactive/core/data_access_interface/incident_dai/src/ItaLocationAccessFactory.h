
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Stub class created. (To be replaced by actual class from ripple
  */

#ifndef ITALOCATIONASSERFACTORY_DAI_HH
#define ITALOCATIONASSERFACTORY_DAI_HH

#include <string>
#include <vector>

namespace TA_IRS_Core
{
	
	class ItaLocationAccessFactory
	{
	public:
		virtual ~ItaLocationAccessFactory() {};
		virtual std::vector<std::string *> getAllLocations() =0;
		
	};
}// end TA_IRS_Core
#endif //ITALOCATIONASSERFACTORY_DAI_HH