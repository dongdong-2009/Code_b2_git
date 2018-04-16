#include "app/scada/WFAgent/src/WFDpProcessor.h"

namespace TA_IRS_App
{		
	WfDpEvent::WfDpEvent(WfDpEventType type,
						 TA_Base_Bus::DataPoint* pDataPoint,
						 boost::shared_ptr<TA_Base_Bus::DataPointState>  dataPointState)
	:m_type(type),
	 m_pDataPoint(pDataPoint),
	 m_dataPointState(dataPointState)
	{
	}
	
	WfDpEvent::WfDpEvent(WfDpEventType 	type)
	:m_type(type),
	 m_pDataPoint(NULL),
	 m_dataPointState(static_cast<TA_Base_Bus::DataPointState*>(NULL))
	{
	}
	
	WfDpEvent::WfDpEventType WfDpEvent::getEventType()
	{
		return m_type;
	}
	TA_Base_Bus::DataPoint*	 WfDpEvent::getDataPoint()
	{
		return m_pDataPoint;
	}
	boost::shared_ptr<TA_Base_Bus::DataPointState> WfDpEvent::getDataPointState()
	{
		return m_dataPointState;
	}

	WfDpProcessor::WfDpProcessor(std::map < unsigned long, TA_Base_Bus::DataPoint * >& dataPoints)
	:m_isRecording(false),
	 m_dataPoints(dataPoints)
	{
		this->start();
	}
	
	WfDpProcessor::~WfDpProcessor()
	{
		this->terminateAndWait();
	}
	
	void WfDpProcessor::submitUpdateDataPoint(TA_Base_Bus::DataPoint* pDataPoint, boost::shared_ptr<TA_Base_Bus::DataPointState> state)
	{
		WfDpEvent *newWfDpEvent = new WfDpEvent(WfDpEvent::UpdateFiledValue, pDataPoint, state);
		boost::shared_ptr<WfDpEvent> newEvent(newWfDpEvent);
		this->insert( newEvent );
	}
	
	void WfDpProcessor::submitUpdateAllQualityToBad()
	{
		WfDpEvent *newWfDpEvent = new WfDpEvent(WfDpEvent::UpdateAllQulityToBad);
		boost::shared_ptr<WfDpEvent> newEvent(newWfDpEvent);
		this->insert( newEvent );
	}


	void WfDpProcessor::submitStartRecordDpUpdate()
	{
		WfDpEvent *newWfDpEvent = new WfDpEvent(WfDpEvent::StartRecordDpUpdate);
		boost::shared_ptr<WfDpEvent> newEvent(newWfDpEvent);
		this->insert( newEvent );
	}

	
	void WfDpProcessor::submitStopRecordDpUpdate()
	{
		WfDpEvent *newWfDpEvent = new WfDpEvent(WfDpEvent::StopRecordDpUpdate);
		boost::shared_ptr<WfDpEvent> newEvent(newWfDpEvent);
		this->insert( newEvent );
	}
	
	void WfDpProcessor::processEvent( boost::shared_ptr<WfDpEvent> newEvent )
	{
		try
        {
            switch( newEvent->getEventType() )
			{
			case WfDpEvent::UpdateFiledValue:
				{
					TA_Base_Bus::DataPoint* pDataPoint = newEvent->getDataPoint();
					TA_Base_Bus::DataPointState* pNewState = newEvent->getDataPointState().get();

					if(checkUpdatable(pDataPoint, newEvent->getDataPointState()))
					{
						pDataPoint->updateFieldState( *newEvent->getDataPointState().get() );
					}
					
					if(m_isRecording)
					{
						m_recordDataPoints.insert(pDataPoint);
					}
				}
				break;
			case WfDpEvent::UpdateAllQulityToBad:
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
			case WfDpEvent::StartRecordDpUpdate:
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StartRecordDpUpdate, isRecording %d",m_isRecording==true?1:0);
					if(!m_isRecording)
					{
						m_isRecording = true;
						m_recordDataPoints.clear();
					}
				}
				break;
			case WfDpEvent::StopRecordDpUpdate:
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StopRecordDpUpdate:total size %d, record size %d, isRecording %d",
						m_dataPoints.size(), m_recordDataPoints.size(), m_isRecording==true?1:0);

					if(m_isRecording && ( m_dataPoints.size() != m_recordDataPoints.size() ))
					{
						timeb currentSystemTime;  
						ftime ( &currentSystemTime );
						std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator DpIter;
						TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
						for (DpIter = m_dataPoints.begin(); DpIter != m_dataPoints.end(); ++ DpIter)
						{
							TA_Base_Bus::DataPoint * dp = DpIter->second;

							if(m_recordDataPoints.find(dp) == m_recordDataPoints.end() )
							{
								TA_Base_Bus::DataPointState newState( dp->getRawDpValue(),currentSystemTime,quality);
								dp->updateFieldState( newState );
							}
						}

						DataPointSavedUpdate::iterator saveUpdateItr = m_lastSavedUpdate.begin();
						for(;saveUpdateItr != m_lastSavedUpdate.end();++saveUpdateItr)
						{
							TA_Base_Bus::DataPoint * dp = saveUpdateItr->first;
							if(m_recordDataPoints.find(dp) == m_recordDataPoints.end() )
							{
								saveUpdateItr->second->setStatus(quality);
							}
						}

						m_isRecording = false;
						m_recordDataPoints.clear();
					}
				}
				break;
			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown WfDpEvent Type");
				break;
			}
        }
        catch (...) 
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when processing event");
        }
	}


	bool	WfDpProcessor::checkUpdatable(TA_Base_Bus::DataPoint* dp,boost::shared_ptr<TA_Base_Bus::DataPointState> newUpdate)
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
