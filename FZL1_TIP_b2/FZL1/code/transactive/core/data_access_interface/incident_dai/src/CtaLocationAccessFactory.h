/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * CtaLocationAccessFactory is a singleton that is used to retrieve Location objects from the
  * database. 
  */
#ifndef CTALOCATIONACCESSFACTORY_HH
#define CTALOCATIONACCESSFACTORY_HH

#include "core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h"

namespace TA_IRS_Core
{
	class CtaLocationAccessFactory : public virtual ItaLocationAccessFactory 
	{
		
	public:
		CtaLocationAccessFactory();
		virtual ~CtaLocationAccessFactory();
		
	public:
		virtual T_LocDisplayMap getAllLocations();

	private:
		T_LocDisplayMap m_locationsDisp; 
		bool m_bDataLoaded;
	};
} // end TA_IRS_Core
#endif // CTALOCATIONACCESSFACTORY_HH