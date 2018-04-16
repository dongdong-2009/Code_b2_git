
#ifndef PENERGY_DP_PROCESSOR_H__
#define PENERGY_DP_PROCESSOR_H__

#include <map>
#include "boost/shared_ptr.hpp"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include <core/threads/src/QueueProcessor.h>

namespace TA_Base_Bus
{
	class DataPoint;
};
namespace TA_Base_App
{
	class PEnergyDpEvent
	{
	public:
		enum PEnergyDpEventType
		{
			UnknownType,
			UpdateFieldValue,
			UpdateAllQualityToBad
		};
	public:
		PEnergyDpEvent::PEnergyDpEvent(PEnergyDpEventType 		type,
									 TA_Base_Bus::DataPoint*	pDataPoint,
									 boost::shared_ptr<TA_Base_Bus::DataPointState>  dataPointState);
		
		PEnergyDpEvent::PEnergyDpEvent(PEnergyDpEventType 	type = UnknownType);

		PEnergyDpEventType	getEventType();
		TA_Base_Bus::DataPoint*			getDataPoint();
		boost::shared_ptr<TA_Base_Bus::DataPointState> getDataPointState();
	private:
		PEnergyDpEventType 								m_type;
		TA_Base_Bus::DataPoint*							m_pDataPoint;
		boost::shared_ptr<TA_Base_Bus::DataPointState>	m_dataPointState;
	};

	class PEnergyDpProcessor:public TA_Base_Core::QueueProcessor<PEnergyDpEvent>
	{
	public:
		PEnergyDpProcessor(std::map < unsigned long, TA_Base_Bus::DataPoint * >& dataPoints);
		~PEnergyDpProcessor();
		void submitUpdateDataPoint(TA_Base_Bus::DataPoint* pDataPoint, boost::shared_ptr<TA_Base_Bus::DataPointState> state);
		void submitUpdateAllQualityToBad();
		virtual void processEvent( boost::shared_ptr<PEnergyDpEvent> newEvent );
	private:
		bool		checkUpdatable(TA_Base_Bus::DataPoint* dp,boost::shared_ptr<TA_Base_Bus::DataPointState> newUpdate);
	private:
		std::map < unsigned long, TA_Base_Bus::DataPoint * >&	m_dataPoints;

		typedef std::map <TA_Base_Bus::DataPoint *, boost::shared_ptr<TA_Base_Bus::DataPointState> > DataPointSavedUpdate;
		DataPointSavedUpdate	m_lastSavedUpdate;
		
	};	
}

#endif

