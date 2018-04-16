#ifndef MMS_PERIODIC_THREAD_H
#define MMS_PERIODIC_THREAD_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/mms_agent/src/PeriodicThread.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class checks to see if any periodic maintenance messages are due to be
  * sent and if so sends them
  */

#include "core/threads/src/Thread.h"
#include <vector>
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h"
#include "core/data_access_interface/mms_dai/src/IConfigMmScheduling.h"
#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"

 
#include "core/alarm/src/AlarmHelper.h" 
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "app/maintenance_management/mms_agent/src/DPScadaWorker.h"
#include "app/maintenance_management/mms_agent/src/MaintenanceDataWrapper.h"

namespace TA_Base_Core
{
	class MmsPeriodicEntityData;
	class ConfigUpdateMessageSender;
}

namespace TA_IRS_App
{
	class MmsConnectionException;

    


	class PeriodicThread : public TA_Base_Core::Thread, public IDPScadaWorkerCallback,  public virtual TA_Base_Core::IOnlineUpdatable
	{
	public:

		PeriodicThread();

		virtual ~PeriodicThread();

		/** Define a run() method to do your work. This should block until
		  * terminate() is called eg: CorbaUtil::GetInstance().Run() or
		  * m_dialog->DoModal()
		  *
		  * NOTE: do not call this method directly. It needs to be public so that it
		  *       can be called from the thread-spawning function
		 */
		virtual void run();

		/* The terminate() method should cause run() to return. eg:
		  * CorbaUtil::GetInstance().Shutdown() or EndDialog(m_dialog)
		  *
		  * NOTE: probably not a good idea to call this method directly - call
		  *       terminateAndWait() instead
		 */
		virtual void terminate();

        /**
          * initConfiguration
		  *
		  * Precondition: pData != 0 and has already been invalidated
		  * Will apply the data in the config to the object
		  *
		 */
		void initConfiguration(TA_Base_Core::MmsPeriodicEntityDataPtr pData);

		void setScanPeriodMinutes(unsigned int minutes);
	 
        //TD16871 
        //Mintao++
        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


		virtual void  dataNotify(unsigned long locationKey,std::vector<MaintenanceDataWrapper*>& vecData);


		void setToControlMode();
		 
		void setToMonitorMode();
		 
	private: //data
		bool m_stopRequest;

    	//online config params
		unsigned int m_scanPeriodMinutes;
	 	 
  

		static const CORBA::Double BAD_READ_VALUE;

		//chenlei++
		//maintain the Entitydata for initialization
		TA_Base_Core::MmsPeriodicEntityDataPtr m_periodicData;
		void initConfiguration();

		void checkScheduleData();
		 
		bool isScheduleOn(MaintenanceDataWrapper* data,timeb timeNow);

		TA_Base_Core::ReEntrantThreadLockable  m_dataLock;

        typedef  std::map<unsigned long, std::vector<MaintenanceDataWrapper*> > LocationDataMap;

		//cache configuration data
		LocationDataMap m_locationDataMap;
		
		//data point scada workers

		typedef  std::map<unsigned long,  DPScadaWorker*>  DPWorkerMap;

		DPWorkerMap  m_workerMap;

		bool m_isControlMode;



	};
} //end TA_App

#endif  // MMS_PERIODIC_THREAD_H
