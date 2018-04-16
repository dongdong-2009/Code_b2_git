
#ifndef GENA_RAWTABLE_H
#define GENA_RAWTABLE_H

#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "core/types/public_types/Constants.h"

namespace TA_Base_Bus
{
	class GENATopicData;
}

namespace TA_IRS_App
{
	class GenaRawTable : public TA_IRS_Bus::RawTable
	{
	public:
		GenaRawTable(TA_Base_Core::EDataPointQuality quality);

		void setTopicData(const TA_Base_Bus::GENATopicDataPtr data);
		
		TA_Base_Bus::GENATopicDataPtr getTopicData();

		void setDataPointQuality(TA_Base_Core::EDataPointQuality dpQuality);

		TA_Base_Core::EDataPointQuality GenaRawTable :: getDataPointQuality();

		virtual ~GenaRawTable(){};

	private:
		//Disable default, copy and assignment constructor
		//GenaRawTable();
		//GenaRawTable(const GenaRawTable& );
		GenaRawTable & operator= (const GenaRawTable&);

		TA_Base_Bus::GENATopicDataPtr	m_pGenaTopicData;
		TA_Base_Core::EDataPointQuality		m_dpQuality;
	};
}

#endif //GENA_RAWTABLE_H