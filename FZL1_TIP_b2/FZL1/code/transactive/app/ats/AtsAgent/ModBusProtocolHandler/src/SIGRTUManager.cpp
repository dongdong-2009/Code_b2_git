/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/code/transactive/app/ats/AtsAgent/ModBusProtocolHandler/src/SIGRTUManager.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #19 $
  *
  * Last modification:	$DateTime: 2016/04/08 14:49:53 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The StandardRTUManager object is responsible for managing RTUs' running mode 
  * And processing RTUs' poll/write data
  *
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/SIGRTUManager.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/SIGStatusWorker.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/StandardRTU.h"
#include "bus/scada/rtu_library/src/CommonDpProcessor.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
    SIGRTUManager::SIGRTUManager(	)
        :
    TA_Base_Bus::StandardRTUManager(1,true,false,TA_Base_Bus::SINGLE_PRIMARY_RTU_PATTERN,false,false,0,10,"0F"),
	m_prevBadQuality(TA_Base_Core::ITA_QUALITY_BAD),
	m_isDpProcessorCreated(false)
    {
	}

    SIGRTUManager::~SIGRTUManager()
    {
    }

	TA_Base_Bus::IEntity* SIGRTUManager::createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);
		
		TA_Base_Bus::IEntity* rtuEntity = TA_Base_Bus::StandardRTUManager::createRTUEntity(EntityData);
 
		// cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr rtuEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);

		if (false == m_isDpProcessorCreated)
		{
			
			m_rtuPollingStartAddr = rtuEntityData->getRTUPollingTableStartAddress();
			m_rtuPollingEndAddr	= rtuEntityData->getRTUPollingTableEndAddress();
			m_scanTime	= rtuEntityData->getScanTimeMSecs();
			
			TA_Base_Bus::ModbusDpProcessor* dpProcessor = new TA_Base_Bus::CommonDpProcessor(rtuEntityData->getName(),
											   m_rtuPollingStartAddr,
											   m_rtuPollingEndAddr,
											   0,
											   false,
											   TA_Base_Bus::DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL);
			addDpProcessor(dpProcessor);
			m_isDpProcessorCreated = true;
		}
		return rtuEntity;
	}
	
    void SIGRTUManager::setRTUStatusWorker()
	{
		for (unsigned long i = 0; i < m_rtus.size(); ++i)
		{
			TA_Base_Bus::StandardRTU* standardRTU = dynamic_cast<TA_Base_Bus::StandardRTU*> (m_rtus[i]);
			TA_Base_Bus::RTUStatusWorker* rtuStatusWorker = new SIGStatusWorker();
			standardRTU->setRTUStatus(rtuStatusWorker);
		}	
		
	}

    void SIGRTUManager::setPollingScheduler()
	{
		addPollingScheduler(m_rtuPollingStartAddr,
							m_rtuPollingEndAddr,
							m_scanTime,
							TA_Base_Bus::FAST_POLLING);			
	}	
	
	void SIGRTUManager::dispatchDataBlock(TA_Base_Bus::RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch StandardRTU [%s] datablock to DpProcessors...", 
            rtu.getName().c_str() );

		TA_Base_Bus::RTU_STATUS rtuCommsStatus = rtu.getRTUStatus();
		
		if (TA_Base_Bus::RTU_PRIMARY == rtuCommsStatus)
		{
			processInternalDpProcessors(addressBlock, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
			processDataBlockForObserver(addressBlock);
			m_prevBadQuality = TA_Base_Core::ITA_QUALITY_GOOD;			
		}
		else if (TA_Base_Bus::RTU_STANDBY == rtuCommsStatus)
		{
			// FZ - notify holder about offline status i.e. Not(m_isOnline)
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Check err Quality as m_Online=false.");
			processInternalDpProcessors(addressBlock, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);				
			updateObserverAbtQualityStatus(TA_Base_Core::ITA_QUALITY_BAD_OUT_OF_SERVICE);
			m_prevBadQuality = TA_Base_Core::ITA_QUALITY_BAD_OUT_OF_SERVICE;
		}
		else if (TA_Base_Bus::RTU_COMMS_DOWN == rtuCommsStatus)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Set Quality Comms Failure as commsOk=false");
			//FZ - Polling_Wrkr ->m_commsError is true
			processInternalDpProcessors(addressBlock, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
			updateObserverAbtQualityStatus(TA_Base_Core::ITA_QUALITY_BAD_COMM_FAILURE); 
			m_prevBadQuality = TA_Base_Core::ITA_QUALITY_BAD_COMM_FAILURE;
		}
	}
	
	void SIGRTUManager::updateObserverAbtQualityStatus(const TA_Base_Core::EDataPointQuality& quality)
	{
		if( m_prevBadQuality == quality)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No change in Bad Quality Status(Value=%d) hence not dispatching status to Datablock observers.", quality);
			return ;
		}

		std::vector<TA_Base_Bus::DataBlockObserver*>::iterator it;
		for(it = m_observers.begin(); it != m_observers.end(); ++it)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch err %d quality status to Datablock observer.", quality);
			(*it)->updateQualityStatus(quality);
		}
 	}

	void SIGRTUManager::processInternalDpProcessors(const TA_Base_Bus::DataBlock< WORD > & addressBlock,const TA_Base_Bus::EQualityStatus& quality)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Processing Internal DPs, setting quality status as %d", quality);

        for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
			m_qualityStatus[i] = quality;
			m_rtuDpProcessors[i]->processDataBlock(addressBlock, m_qualityStatus[i]);
        }
		return;
	}	
}
