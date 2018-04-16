/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TrainAlarmAgent/src/TisDpProcessor.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/05/18 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TisDpProcessor is inheriting from ModbusDpProcessor (it's in rtu_lib in bus layer).
* its major function is implementing some special data point processing of TisAgent,
* but other nomal working still flow ModbusDpProcessor which is in rtu_lib laryer.
*
*/

#pragma once

#ifndef TISAGENT_TISDPPROCESSOR_H_INCLUDED
#define TISAGENT_TISDPPROCESSOR_H_INCLUDED

// #include "bus/scada/rtu_library/src/ModbusDpProcessor.h"
#include "bus/scada/rtu_library/src/CommonDpProcessor.h"

namespace TA_IRS_App
{
    class TisDpProcessor : public TA_Base_Bus::CommonDpProcessor
    {
    public:
        // constroctor        
        TisDpProcessor( const std::string& 	name,
						unsigned long 		pollingStartAddress,
						unsigned long 		pollingEndAddress,
						unsigned long 		offsetAddress,
						bool 		  		reverseDiBitOrder,
						int 		  		analogueDpUpdateInterval);
        
        // destoryer
        virtual ~TisDpProcessor();
        
        unsigned long getPollingStartAddr();
        unsigned long getPollingEndAddr();

        virtual bool addDataPoint(TA_Base_Bus::DataPoint * dp);        

    protected:
        int processSpecialDataPoint(); // override
        int populateSpecialDataPoints(); // override

    private:        
        unsigned long m_pollingStartAddr, m_pollingEndAddr;
        std::vector<TA_Base_Bus::DataPoint * > m_specialDps;

    }; // TisDpProcessor
}

#endif // TISAGENT_TISDPPROCESSOR_H_INCLUDED
