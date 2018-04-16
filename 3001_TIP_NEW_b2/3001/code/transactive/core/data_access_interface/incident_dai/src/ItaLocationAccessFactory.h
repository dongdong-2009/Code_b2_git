
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/15 17:23:02 $
  * Last modified by:  $Author: lu.xiangmei $
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