#if defined ( STEPTYPE_DATA_POINT_TIME_CHECK )

#ifndef ACTIVEDATAPOINTTIMECHECKSTEP_H_INCLUDED
#define ACTIVEDATAPOINTTIMECHECKSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointTimeCheckStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  <description>
  */
#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveDataPointTimeCheckStep : public ActiveStep
	{
	// Public methods
	public:
	    ActiveDataPointTimeCheckStep( const TA_Base_Core::StepDetail& stepDetail, 
            const TA_Base_Core::DataPointTimeCheckParameters& params, IActivePlanFlow& flowControl );

	protected:
	    virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveDataPointTimeCheckStep& operator=( const ActiveDataPointTimeCheckStep& );
		ActiveDataPointTimeCheckStep( const ActiveDataPointTimeCheckStep& );

		const TA_Base_Core::DataPointTimeCheckParameters m_params;

		TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef,
            TA_Base_Bus::IDataPointAccessCorbaDef_ptr, TA_Base_Bus::IDataPointAccessCorbaDef_var> m_dataPoint;

        unsigned long m_ulDpKey;
        bool m_bLastQualityGood;
	}; // ActiveDataPointTimeCheckStep
}; // TA_Base_App

#endif // ACTIVEDATAPOINTTIMECHECKSTEP_H_INCLUDED


#endif //  STEPTYPE_DATA_POINT_TIME_CHECK

