
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Stub class created. (To be replaced by actual class from ripple
  */

#ifndef ITALOCATIONASSERFACTORY_DAI_HH
#define ITALOCATIONASSERFACTORY_DAI_HH

#include <string>
#include <map>

namespace TA_IRS_Core
{
	typedef std::map<std::string, std::string> T_LocDisplayMap;

	class ItaLocationAccessFactory
	{
	public:
		virtual ~ItaLocationAccessFactory() {};
		virtual T_LocDisplayMap getAllLocations() =0;
		
	};
}// end TA_IRS_Core
#endif //ITALOCATIONASSERFACTORY_DAI_HH