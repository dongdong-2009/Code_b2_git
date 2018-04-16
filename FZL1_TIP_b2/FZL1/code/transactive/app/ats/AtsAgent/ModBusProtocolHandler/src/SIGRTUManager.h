/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/code/transactive/app/ats/AtsAgent/ModBusProtocolHandler/src/SIGRTUManager.h $
  * @author:	Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The BaseRTUManager object is the interface which manage RTUs' running mode and processing RTUs' poll/write data
  *
  */

#if !defined(AFX_SIG_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_SIG_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/scada/rtu_library/src/StandardRTUManager.h"

namespace TA_IRS_App
{
    class SIGRTUManager : public TA_Base_Bus::StandardRTUManager
    {
    public:
        SIGRTUManager();

        virtual  ~SIGRTUManager();
 
	    virtual TA_Base_Bus::IEntity* createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData);
        virtual void setRTUStatusWorker();
        void setPollingScheduler();
        virtual void dispatchDataBlock(TA_Base_Bus::RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);
		
	private:
		void processInternalDpProcessors(const TA_Base_Bus::DataBlock< WORD > & addressBlock,const TA_Base_Bus::EQualityStatus& quality);
		void updateObserverAbtQualityStatus(const TA_Base_Core::EDataPointQuality& quality = TA_Base_Core::ITA_QUALITY_BAD);
	
		bool 									m_isDpProcessorCreated;
		TA_Base_Core::EDataPointQuality 		m_prevBadQuality;
    	TA_Base_Core::ReEntrantThreadLockable   m_dpProcessorLock;

    	unsigned long m_rtuPollingStartAddr;
    	unsigned long m_rtuPollingEndAddr;
    	int           m_scanTime;
		
	};
}

#endif // !defined(AFX_STANDARD_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
