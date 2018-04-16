/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/CtaReportAccessFactory.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The class provide the access matrix for the incident report access
  */

#ifndef CtaReportAccessFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_
#define CtaReportAccessFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_

#include "core/data_access_interface/incident_dai/src/ItaReportAccessFactory.h"
#include <string>

namespace TA_IRS_Core
{
	class CtaReportAccessFactory : public virtual ItaReportAccessFactory 
	{
		
	public:
		CtaReportAccessFactory();
		virtual ~CtaReportAccessFactory();
		
		virtual AccessMatrix getAccessMetrix(bool isIncident);
		// TD15203 
		virtual void updateAccessMatrix(const bool isIncident, const AccessMatrix& newMatrix);
		
	private:
		
		static const std::string IN_REPORT_ACCESS;
		static const std::string CREATE_SEPGRO_ID_COL;
		static const std::string ACCESS_SEPGRO_ID_COL;
		static const std::string ACCESS_LEVEL_COL; 
	};
} // end TA_IRS_Core
#endif
