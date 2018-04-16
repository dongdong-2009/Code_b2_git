/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ReportEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef TL_REPORTEVENT_H
#define TL_REPORTEVENT_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class TL_ReportEvent : public TLCommon  
	{
		public:
			TL_ReportEvent();
			virtual ~TL_ReportEvent();

			unsigned int getTLDataLength();

			TLDeliveryStatus getDeliveryStatus();
			void setDeliveryStatus(TLDeliveryStatus status);

			bool parseData(const std::vector<unsigned char>& rawData);

		protected:
			virtual void buildMsg();

		private:
			// disabled methods
			TL_ReportEvent(const TL_ReportEvent&);
			void operator=(const TL_ReportEvent&);

		/* attributes */
		private:

			TLDeliveryStatus deliveryStatus;

		public: 
			static const int TL_REPORT_CM_TL_LENGTH;

	};
};
#endif
