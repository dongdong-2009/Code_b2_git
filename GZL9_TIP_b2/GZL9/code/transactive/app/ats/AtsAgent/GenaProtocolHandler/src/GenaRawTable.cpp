
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"

namespace TA_IRS_App
{
	GenaRawTable :: GenaRawTable(TA_Base_Core::EDataPointQuality quality):RawTable(0,0)
	{
		m_dpQuality = quality;
	}


	void GenaRawTable :: setTopicData(const TA_Base_Bus::GENATopicDataPtr data)
	{
		/*if(!m_pGenaTopicData)
		{
			m_pGenaTopicData = TA_Base_Bus::GENATopicDataPtr (new TA_Base_Bus::GENATopicData(*data));
		}
		else
		{
			m_pGenaTopicData = data;
		}*/	
		m_pGenaTopicData = data;
	}
		
	TA_Base_Bus::GENATopicDataPtr GenaRawTable :: getTopicData()
	{
		return m_pGenaTopicData;
	}

	TA_Base_Core::EDataPointQuality GenaRawTable :: getDataPointQuality()
	{
		return m_dpQuality;
	}	

	void GenaRawTable :: setDataPointQuality(TA_Base_Core::EDataPointQuality dpQuality)
	{
		m_dpQuality = dpQuality ;
	}

}

