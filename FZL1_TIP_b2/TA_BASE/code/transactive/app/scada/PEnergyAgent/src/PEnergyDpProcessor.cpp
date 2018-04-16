#include "app/scada/PEnergyAgent/src/PEnergyDpProcessor.h"

namespace TA_Base_App
{		
	PEnergyDpEvent::PEnergyDpEvent(PEnergyDpEventType 							type,
								 TA_Base_Bus::DataPoint*						pDataPoint,
								 boost::shared_ptr<TA_Base_Bus::DataPointState> dataPointState)
	:m_type(type),
	 m_pDataPoint(pDataPoint),
	 m_dataPointState(dataPointState)
	{
	}
	
	PEnergyDpEvent::PEnergyDpEvent(PEnergyDpEventType 	type)
	:m_type(type),
	 m_pDataPoint(NULL),
	 m_dataPointState(static_cast<TA_Base_Bus::DataPointState*>(NULL))
	{
	}
	
	PEnergyDpEvent::PEnergyDpEventType PEnergyDpEvent::getEventType()
	{
		return m_type;
	}
	TA_Base_Bus::DataPoint*	 PEnergyDpEvent::getDataPoint()
	{
		return m_pDataPoint;
	}
	boost::shared_ptr<TA_Base_Bus::DataPointState> PEnergyDpEvent::getDataPointState()
	{
		return m_dataPointState;
	}

	PEnergyDpProcessor::PEnergyDpProcessor(std::map < unsigned long, TA_Base_Bus::DataPoint * >& dataPoints)
	:m_dataPoints(dataPoints)
	{
		this->start();
	}
	
	PEnergyDpProcessor::~PEnergyDpProcessor()
	{
		this->terminateAndWait();
	}
	
	void PEnergyDpProcessor::submitUpdateDataPoint(TA_Base_Bus::DataPoint* pDataPoint, boost::shared_ptr<TA_Base_Bus::DataPointState> state)
	{
		PEnergyDpEvent* newPEnergyDpEvent = new PEnergyDpEvent(PEnergyDpEvent::UpdateFieldValue, pDataPoint, state);
		boost::shared_ptr<PEnergyDpEvent> newEvent(newPEnergyDpEvent);
		this->insert( newEvent );
	}
	
	void PEnergyDpProcessor::submitUpdateAllQualityToBad()
	{
		PEnergyDpEvent *newPEnergyDpEvent = new PEnergyDpEvent(PEnergyDpEvent::UpdateAllQualityToBad);
		boost::shared_ptr<PEnergyDpEvent> newEvent(newPEnergyDpEvent);
		this->insert( newEvent );
	}
	
	void PEnergyDpProcessor::processEvent( boost::shared_ptr<PEnergyDpEvent> newEvent )
	{
		try
        {
            switch( newEvent->getEventType() )
			{
			case PEnergyDpEvent::UpdateFieldValue:
				{
					TA_Base_Bus::DataPoint* pDataPoint = newEvent->getDataPoint();
					TA_Base_Bus::DataPointState* pNewState = newEvent->getDataPointState().get();

					if(checkUpdatable(pDataPoint, newEvent->getDataPointState()))
					{
						pDataPoint->updateFieldState( *newEvent->getDataPointState().get() );
					}
				}
				break;
			case PEnergyDpEvent::UpdateAllQualityToBad:
				{
					timeb currentSystemTime;  
					ftime ( &currentSystemTime );
					std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator DpIter;
					TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin update all datapoint's Quality to %d",quality);
					for (DpIter = m_dataPoints.begin(); DpIter != m_dataPoints.end(); ++ DpIter)
					{
						TA_Base_Bus::DataPoint * dp = DpIter->second;
						TA_Base_Bus::DataPointState newState( dp->getRawDpValue(),currentSystemTime,quality);
						dp->updateFieldState( newState );
					}
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End update all datapoint's Quality to %d",quality);

					DataPointSavedUpdate::iterator itr = m_lastSavedUpdate.begin();
					for(;itr != m_lastSavedUpdate.end(); ++itr)
					{
						itr->second->setStatus(quality);
					}
				}
				break;
			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown PEnergyDpEvent Type");
				break;
			}
        }
        catch (...) 
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when processing event");
        }
	}


	bool PEnergyDpProcessor::checkUpdatable(TA_Base_Bus::DataPoint* dp,boost::shared_ptr<TA_Base_Bus::DataPointState> newUpdate)
	{
		bool rtn = false;
		DataPointSavedUpdate::iterator itr = m_lastSavedUpdate.find(dp);
		
		if(itr  == m_lastSavedUpdate.end() )
		{
			m_lastSavedUpdate.insert(std::make_pair(dp, newUpdate));
			rtn = true;
		}
		else if( itr->second->getValue() != newUpdate->getValue() || 
				 itr->second->getStatus() != newUpdate->getStatus() )
		{
			itr->second = newUpdate;
			rtn = true;
		}
		else
		{
			rtn = false;
		}
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "checkUpdatable [%d] [%s]",dp->getDataPointRegisterAddress(),rtn?"true":"false");
		return rtn;
	}
}
