/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainBarrier.h $
  * @author:  zhou yuan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#if !defined TRAIN_BARRIER_H
#define TRAIN_BARRIER_H

#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{
	/**
	*  Receives and consumes an ATS message
	*/
	class TrainBarrier
	{
	public:
		TrainBarrier(int boundary=1);

		virtual ~TrainBarrier();
		
		void wait();
		void post();
		
	private:
		
		TA_Base_Core::NonReEntrantThreadLockable	m_counterLock;
		TA_Base_Core::Semaphore						m_semaphore;
		int											m_counter; 
		int											m_boundary;

	};

};  // Namespace TA_IRS_App


#endif
