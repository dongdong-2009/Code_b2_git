#if defined ( STEPTYPE_LOG )


#ifndef ACTIVE_LOG_STEP_HEADER_INCLUDED
#define ACTIVE_LOG_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLogStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
namespace TA_Base_App
{

	class ActiveLogStep : public ActiveStep
	{
	public:
		ActiveLogStep( const TA_Base_Core::StepDetail& stepDetail,
					   const TA_Base_Core::LogParameters& params,
					   IActivePlanFlow& flowControl );

	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveLogStep& operator=( const ActiveLogStep& );
		ActiveLogStep( const ActiveLogStep& );

		const TA_Base_Core::LogParameters m_params;

		IActivePlanFlow& m_flowControl;

	};
}
#endif //ACTIVE_LOG_STEP_HEADER_INCLUDED


#endif //  STEPTYPE_LOG
