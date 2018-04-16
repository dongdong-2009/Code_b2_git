/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/SNMP_library/src/IWorkerPool.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef SNMP__I_WORKER_POOL_H_
#define SNMP__I_WORKER_POOL_H_

namespace TA_Base_Bus
{
	/** define WorkerPool's Task interface.
     *  
     */
	class IWorkerTask
	{
	public:
		virtual ~IWorkerTask(){};

		/** execute the task
         *
		 * @return void:
         */
		virtual void executeTask() = 0;

		/** determine whether the task execute time arrived.
         *
		 * @return bool: ture: need to execute task, false: no need to execute task.
         */
		virtual bool taskTimeExpired() = 0;
	};

	/** define WorkerPool's worker interface.
     *  
     */
	class IThreadWorker
	{
	public:
		virtual ~IThreadWorker(){};

		/** assign a task to worker to execute.
         *
         * @param task: need to executed task.
		 * @return bool: assigned successfully return true, else false.
         */
		virtual bool assignTask(IWorkerTask* task) = 0;
	};

	/** define WorkerPool's interface.
     *  
     */
	class IWorkerPool
	{

	public:
		virtual ~IWorkerPool(){};

		/** when ThreadWorker finished executing task, call this function tell WorkerPool.
         *
         * @param worker: the ThreadWorker.
		 * @param task: executed task.
         */
		virtual void finishedTask(IThreadWorker* worker, IWorkerTask* task) = 0;
	};

}

#endif
