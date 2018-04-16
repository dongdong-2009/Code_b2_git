
#ifndef ATSBASDATA_H
#define ATSBASDATA_H

#include <time.h>

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/types/public_types/Constants.h"

namespace TA_IRS_Bus
{
    class AtsSCADAData  
    {
		protected:
			AtsSCADAData(TA_Base_Bus::ItaScada* tScada, std::string dataName);
			virtual ~AtsSCADAData();
			void setLocalDataPoint(const std::string& dpAddress, unsigned long dpValue);		 
			void setLocalDataPoint(const std::string& dpAddress, const std::string& dpValue);
			void setLocalDataPoint(const std::string& dpAddress, bool dpValue, TA_Base_Core::EDataPointQuality dpQuality);

			void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const std::string& value, TA_Base_Core::EDataPointQuality quality);
			void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const unsigned long value, TA_Base_Core::EDataPointQuality quality);
			void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const bool value, TA_Base_Core::EDataPointQuality quality);

			void setAllLocalDataPointQualitiesMatchingAddress (const std::string& addressSubString, TA_Base_Core::EDataPointQuality quality);
		private:
			TA_Base_Bus::ItaScada* m_itScada;
			std::string m_dataName;
    };
}

#endif // !defined(ATSBASDATA_H)
