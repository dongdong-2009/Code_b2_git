/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/trains/train_agent/src/ITrainEventReaderDelegator.h $
  * @author:  Glen Kidd
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface provides access to the delegate hook for TrainEventReaderDelegator
  *
  */


#if !defined(ITrainEventReaderDelegator_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
#define ITrainEventReaderDelegator_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_

#include <vector>

/// @todo move tsm:ti so we don't need this include
//#include "app/trains/train_agent/src/TrainStatusManager.h"

namespace TA_IRS_App
{
	class TrainEvent;
	class TrainEventReader;
	struct ATSTrainMessage;
	struct DutyTrainMessage;
	namespace TrainStatus
	{
		class TrainInformation;
	}
	using namespace TrainStatus;

	/**
	 * This interface provides access to the Train Corba Impl Adapters
	 */
    class ITrainEventReaderDelegator
	{
		public:
            // Note: need a method pointer to look up onEventXXX(), but
            // no common base class methods used for the adapters ;(
            //typedef void (ITrainEventReaderDelegator::*delegatorCallback)(TrainEvent&);
			//virtual delegatorCallback delegate(TrainEvent& event) = 0;

            virtual bool delegate(TrainEvent& event) = 0;

	};

    /**
     * This is similar to ITrainEventReaderDelegator
     * ITrainEventListeners cannot consume events, they are interested only in the fact a train event
     * was received.
     * All processing is done AFTER the readers have done their processing, so as not to cause a timeout
     * on open transactions.
     */
    class ITrainEventListener
	{
		public:
            virtual void receive(TrainEvent& event) = 0;
	};


    class ITrainATSDelegator
	{
		public:
            virtual bool delegate(ATSTrainMessage& event) = 0;
	};


    class ITrainStateUpdateDelegator
	{
		public:
            virtual bool delegate(std::vector<TrainInformation>& updateList) = 0;
	};

    class ITrainDutyDelegator
	{
		public:
            virtual bool delegate(DutyTrainMessage& event) = 0;
	};

    class ITrainCallEndDelegator
	{
		public:
            virtual bool delegate(unsigned long callId, std::vector<unsigned char>& trains) = 0;
	};

    class ITrainDownloadDelegator
	{
		public:
            virtual bool delegate(unsigned char trainId, bool success, const std::string& failReason) = 0;
	};
};
#endif // !defined(ITrainEventReaderDelegator_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
