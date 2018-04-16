/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/src/DisplayDataPointProxy.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/01/25 15:23:55 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// DispalyDataPointProxy.h: interface for the DispalyDataPointProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DISPLAY_DATAPOINT_PROXY_H
#define DISPLAY_DATAPOINT_PROXY_H

#include <string>
#include <comdef.h>

#include "bus/scada/common_library/src/DpValue.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_App
{
	class AbstractDataObjectAttributes;
    
    class DisplayDataPointProxy  
    {
    public:

        DisplayDataPointProxy(unsigned long entityKey, AbstractDataObjectAttributes * eventProcessor);
        virtual ~DisplayDataPointProxy();

        bool updateState(TA_Base_Bus::DataPointDisplayState& newState);

        TA_Base_Bus::DisplayDataPointValue getValue();
	    _variant_t getVariantValue();

		bool isInAlarmState();
        TA_Base_Bus::EScadaAlarmAckState getAlarmAckState();
        WORD getStatus();
        void setStatus(unsigned long newStatus);

		AbstractDataObjectAttributes * getAttribute();
		void setAttribute(AbstractDataObjectAttributes * );
            
    private:    

        DisplayDataPointProxy();
        DisplayDataPointProxy(DisplayDataPointProxy&){};
    
    private:
		AbstractDataObjectAttributes * m_attribute;
		TA_Base_Core::ReEntrantThreadLockable m_lock;

        TA_Base_Bus::DataPointDisplayState m_state;

    };
}
#endif // !defined(DISPLAY_DATAPOINT_PROXY_H)
