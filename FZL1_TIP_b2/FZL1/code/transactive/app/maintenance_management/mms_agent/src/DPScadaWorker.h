/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/MaintenanceDataWrapper.h $
  * @author:   huirong.luo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/02/28 15:06:30 $
  * Last modified by:  $Author: weikun.lin $
  *
  * 
  */
#if !defined(EA_4DF218C4_683C_4247_BDED_EF4138721656__INCLUDED_)
#define EA_4DF218C4_683C_4247_BDED_EF4138721656__INCLUDED_
 
 
 
 
#include <map>
#include <vector>
#include <ctime>
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "app/maintenance_management/mms_agent/src/MaintenanceDataWrapper.h"

using TA_Base_Bus::ScadaUtilities;

namespace TA_IRS_App
{ 

	 
	
	class IDPScadaWorkerCallback
	{

	public:
		virtual void  dataNotify(unsigned long locationKey, std::vector<MaintenanceDataWrapper*>& vecData)=0;
		/**
		* Constructor
		*/
		IDPScadaWorkerCallback(){};

		/**
		* Destructor
		*/
		~IDPScadaWorkerCallback(){};
	};

	class DPScadaWorker : public virtual TA_Base_Core::Thread
	{
	public:

	 
		typedef std::map<unsigned long, MaintenanceDataWrapper*> UpdateMap;
		 

		/**
		* Constructor
		*/
		DPScadaWorker(unsigned long locationKey, IDPScadaWorkerCallback* callback);

		/**
		* Destructor
		*/
		virtual ~DPScadaWorker();

		/**
		* push
		*
		* This method enables clients to push a new IQueueItem onto
		* our queue
		*
		* @param A pointer to the IQueueItem to be pushed		*/
	
		virtual void push( std::vector<MaintenanceDataWrapper*>& dataVec);
	 
 		/**
		* clear
		*
		* This method clears the queue
		*/
		virtual void clear();

		/**
		* run
		*
		* Overides the pure virtual run method declared in Thread
		*/
		virtual void run();

		/**
		* terminate
		*
		* Overides the pure virtual terminate method declared in Thread
		*/
		virtual void terminate();
	 

	private:

		//don't allow duplicated request
		UpdateMap m_updateMap; 

		// Thread lock for the cache
		TA_Base_Core::NonReEntrantThreadLockable m_lock;
		TA_Base_Core::Semaphore m_queueSemaphore;

		bool m_terminated;

		unsigned long m_locaitonKey;

		IDPScadaWorkerCallback* m_callback;

	 

	};
}

#endif // !defined(EA_4DF218C4_683C_4247_BDED_EF4138721656__INCLUDED_)
