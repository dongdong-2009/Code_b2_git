#if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )

#ifndef ACTIVE_ASSIGN_CAMERA_TO_MONITOR_STEP_HEADER_INCLUDED
#define ACTIVE_ASSIGN_CAMERA_TO_MONITOR_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */
namespace TA_Base_App
{
	//Class declaration
	class ActiveAssignCameraToMonitorStep : public ActiveStep
	{
	// Public methods
	public:

		ActiveAssignCameraToMonitorStep( const TA_Base_Core::StepDetail& stepDetail,
						const TA_Base_Core::AssignCameraToMonitorParameters& params,
						IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveAssignCameraToMonitorStep& operator=( const ActiveAssignCameraToMonitorStep& );
		ActiveAssignCameraToMonitorStep( const ActiveAssignCameraToMonitorStep& );

		const TA_Base_Core::AssignCameraToMonitorParameters m_params;
		IActivePlanFlow& m_flowControl;
		
	}; // ActiveCctvStep
}; // TA_Base_App

#endif // ACTIVE_ASSIGN_CAMERA_TO_MONITOR_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_ASSIGN_CAMERA_TO_MONITOR
