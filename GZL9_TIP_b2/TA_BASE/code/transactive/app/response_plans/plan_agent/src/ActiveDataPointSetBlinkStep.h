#if defined ( STEPTYPE_DATA_POINT_SET_BLINK )

#ifndef ACTIVEDATAPOINTSETBLINKSTEP_H_INCLUDED
#define ACTIVEDATAPOINTSETBLINKSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointSetBlinkStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2014/10/07 14:07:45 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  <description>
  */

#include "ace/Future.h"

#include "core/timers/src/TimedWaitSemaphore.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"


namespace TA_Base_App
{
    class IActivePlanFlow;

	//Class declaration
	class ActiveDataPointSetBlinkStep : public ActiveStep, public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	// Public methods
	public: 
		ActiveDataPointSetBlinkStep( const TA_Base_Core::StepDetail& stepDetail, 
            TA_Base_Core::Steps_var& varAllSteps, IActivePlanFlow& flowControl );
        ~ActiveDataPointSetBlinkStep();

		virtual void processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType);

	protected:
	    virtual TA_Base_Core::EActiveStepState executeImpl( void );
		bool sendPScadaBlink(const std::string &strValue, unsigned int nIndex);	
        bool initialize(std::string &strValue, unsigned int nIndex);
	private:
		ActiveDataPointSetBlinkStep& operator=( const ActiveDataPointSetBlinkStep& );
		ActiveDataPointSetBlinkStep( const ActiveDataPointSetBlinkStep& );

		std::vector<TA_Base_Core::DataPointSetParameters> m_dataPointParameters;
		TA_Base_Core::TimedWaitSemaphore m_agentConnectedSem;
		TA_Base_Bus::DataPointProxySmartPtr m_pDataPoint;
		unsigned long m_dataPointKey;
		std::string m_strSession;
	}; // ActiveDataPointSetBlinkStep
}; // TA_Base_App

#endif // ACTIVEDATAPOINTSETBLINKSTEP_H_INCLUDED

#endif //  STEPTYPE_DATA_POINT_SET_BLINK
