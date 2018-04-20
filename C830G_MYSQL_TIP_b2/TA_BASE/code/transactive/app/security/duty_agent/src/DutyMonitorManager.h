/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorManager.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/06/20 18:29:56 $
  * Last modified by:  $Author: shurui.liu $
  *
  * Listens to state changes and manages the DutyMonitor.
  *
  */

#ifndef DUTYMONITORMANAGER_H_INCLUDED
#define DUTYMONITORMANAGER_H_INCLUDED

#include "DutyMonitorTask.h"

namespace TA_Base_App
{
	class DutyMonitorManager : public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>,
		                       public TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> ,
							   public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::StateUpdateSubjectImpl>
	{
	public:
		DutyMonitorManager();
		virtual ~DutyMonitorManager();

		void start();
		void stop();

		virtual void update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange , const TA_Base_Bus::DutyState & state);
		virtual void update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate );
		virtual void update( TA_Base_Bus::StateUpdateSubjectImpl&     stateUpdate  ,const TA_Base_Bus::DutyState & state   );

	private:

		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl> PeerStateParent;
		typedef TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>  AgentTerminateParent;
		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::StateUpdateSubjectImpl>     StateUpdateParent;

		DutyMonitorTask m_dutyMonitorTask;
	};
}
#endif // DUTYMONITORMANAGER_H_INCLUDED
