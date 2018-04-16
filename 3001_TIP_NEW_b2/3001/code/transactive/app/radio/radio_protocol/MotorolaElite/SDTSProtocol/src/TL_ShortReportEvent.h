/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ShortReportEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef TL_SHORTREPORTEVENT_H
#define TL_SHORTREPORTEVENT_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class TL_ShortReportEvent : public TLCommon  
	{
		public:
			TL_ShortReportEvent();
			virtual ~TL_ShortReportEvent();

			TLDeliveryStatus getDeliveryStatus();
			void setDeliveryStatus(TLDeliveryStatus status);

			bool parseData(const std::vector<unsigned char>& rawData);

		private:
			// disabled methods
			TL_ShortReportEvent(const TL_ShortReportEvent&);
			void operator=(const TL_ShortReportEvent&);

		/* attributes */
		private:

			TLDeliveryStatus deliveryStatus;
	};
};
#endif
