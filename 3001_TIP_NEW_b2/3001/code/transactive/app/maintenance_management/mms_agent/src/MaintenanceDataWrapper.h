/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/DPScadaWorker.h $
  * @author:   huirong.luo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/02/28 15:06:30 $
  * Last modified by:  $Author: weikun.lin $
  *
  * 
  */
#if !defined(EA_2840D85B_7772_449b_8425_BE5ADAB56A1D__INCLUDED_)
#define EA_2840D85B_7772_449b_8425_BE5ADAB56A1D__INCLUDED_
 
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include <map>
#include <vector>
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include <sys/timeb.h>
 
using TA_Base_Bus::ScadaUtilities;

namespace TA_IRS_App
{ 

	class MaintenanceDataWrapper
	{
	public:

		ScadaUtilities::MaintenanceData* getData();
		void setData(ScadaUtilities::MaintenanceData* value);
		timeb getLatestPlanDate();
		void setLatestPlanDate(timeb value);

		timeb getNextPlanDate();
	 

	
		MaintenanceDataWrapper();		
		virtual ~MaintenanceDataWrapper();
		MaintenanceDataWrapper(const MaintenanceDataWrapper& other);
		MaintenanceDataWrapper& operator = (const MaintenanceDataWrapper& other);


		void initSchedulingTime();
		void calcNextSchedulingTime();
	private:
		ScadaUtilities::MaintenanceData* m_data;
		timeb  m_latestPlanDate;
		timeb  m_nextPlanDate;
	};
	
	 
}

#endif // !defined(EA_2840D85B_7772_449b_8425_BE5ADAB56A1D__INCLUDED_)
