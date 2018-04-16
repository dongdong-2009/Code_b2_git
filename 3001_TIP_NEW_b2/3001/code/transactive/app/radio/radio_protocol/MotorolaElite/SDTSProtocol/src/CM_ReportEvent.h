
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_ReportEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CM_REPORTEVENT_H
#define CM_REPORTEVENT_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class CM_ReportEvent : public CMCommon2  
	{
		public:
			CM_ReportEvent();
			virtual ~CM_ReportEvent();

			CMDeliveryStatus getDeliveryStatus();
			void setDeliveryStatus(CMDeliveryStatus status);

			bool parseData(std::vector<unsigned char>& rawData);

		private:
			// disabled methods
			CM_ReportEvent(const CM_ReportEvent&);
			void operator=(const CM_ReportEvent&);

			CMDeliveryStatus deliveryStatus;

        public:
            static const int CM_REPORT_STATUS_INDEX;
			static const int CM_REPORT_LENGTH;
	};
};

#endif
