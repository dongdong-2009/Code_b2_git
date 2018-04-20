#if defined ( STEPTYPE_DATA_POINT_SET )

#ifndef ACTIVEDATAPOINTSETSTEP_H_INCLUDED
#define ACTIVEDATAPOINTSETSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointSetStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */
namespace TA_Base_App
{
	//Class declaration
	class ActiveDataPointSetStep : public ActiveStep, public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	// Public methods
	public: 

			ActiveDataPointSetStep( const TA_Base_Core::StepDetail& stepDetail,
								const TA_Base_Core::DataPointSetParameters& params,
							IActivePlanFlow& flowControl,
							bool isGRCType3Step	);

			virtual void processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType);
			void setExecResult(ACE_Future<bool> & execResult)
			{
				m_execResult = execResult;
			}
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveDataPointSetStep& operator=( const ActiveDataPointSetStep& );
		ActiveDataPointSetStep( const ActiveDataPointSetStep& );

		const TA_Base_Core::DataPointSetParameters m_params;
		IActivePlanFlow& m_flowControl;
		TA_Base_Core::TimedWaitSemaphore m_semaphore;
		TA_Base_Core::TimedWaitSemaphore m_cfgvalidsem;
		TA_Base_Bus::DataPointProxySmartPtr m_pDataPoint;
		TA_Base_Bus::ScadaProxyFactory*	m_proxyFactory;

		// TD20339 Yanrong++
		bool m_setValueSuccess;
		// ++Yanrong

		//TD20350
		ACE_Future<bool> m_execResult;
		bool m_isGRCType3Step;

	}; // ActiveDataPointSetStep
}; // TA_Base_App

#endif // ACTIVEDATAPOINTSETSTEP_H_INCLUDED

#endif //  STEPTYPE_DATA_POINT_SET
