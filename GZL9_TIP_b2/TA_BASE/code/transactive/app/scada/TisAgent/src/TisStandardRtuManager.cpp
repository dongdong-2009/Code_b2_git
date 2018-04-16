/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/CsrzzTrainData.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/04/06 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TisStandardRtuManager.
*
*/

#include "TisStandardRtuManager.h"
#include "TisDpProcessor.h"
#include "TrainInfoManager.h"
#include "bus/scada/rtu_library/src/RTU.h"

namespace TA_IRS_App
{
    TisStandardRtuManager::TisStandardRtuManager(int groupId)
    :
    StandardRTUManager(groupId, false, false, TA_Base_Bus::SINGLE_PRIMARY_RTU_PATTERN)        
    {
        FUNCTION_ENTRY("TisStandardRtuManager::TisStandardRtuManager");
        FUNCTION_EXIT;
    }

    TisStandardRtuManager::~TisStandardRtuManager()
    {
    }

    // override StandardRTUManager function, because normally data point quality is following whole data block quality,
    // but for Tis train data point will follow detailed train quality.
    void TisStandardRtuManager::dispatchDataBlock(TA_Base_Bus::RTU& rtu, const TA_Base_Bus::DataBlock<WORD>& addressBlock)
    {
        FUNCTION_ENTRY("TisStandardRtuManager::dispatchDataBlock");

        // to avoid multiple threads access at same time
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);

        // if there is no primary rtu, or this is primary rtu
        for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++i)
        {
            TisDpProcessor* tisDpProcessor = dynamic_cast<TisDpProcessor*>(m_rtuDpProcessors[i]);
            unsigned long startAddr = tisDpProcessor->getPollingStartAddr();
            unsigned long endAddr = tisDpProcessor->getPollingEndAddr();

            if (rtu.getIsCommsOK() && isValidForTrainBlock(addressBlock, startAddr, endAddr))
            {
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Dispatch good quality.");                
            }
            else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus[i])
            {
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Dispatch bad quality.");
            }

            m_rtuDpProcessors[i]->processDataBlock(addressBlock, m_qualityStatus[i]);            
        }

        FUNCTION_EXIT;
    }

    bool TisStandardRtuManager::isValidForTrainBlock(const TA_Base_Bus::DataBlock< WORD > & addressBlock, unsigned long startAddr, unsigned long endAddr)
    {
        FUNCTION_ENTRY("TisStandardRtuManager::isValidForTrainBlock");
        bool isValid = false;
        for (unsigned long i = startAddr; i <= endAddr; i++)
        {
            if (0xFFFF != addressBlock[i]) // if any data in this range is not 0xFFFF, we consider the dataBlock is valid
            {
                isValid = true;
                break;
            }
        }
                
        FUNCTION_EXIT;
        return isValid;
    }

} // namespace TA_IRS_App
