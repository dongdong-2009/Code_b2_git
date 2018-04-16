/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/trains/train_agent/src/ITrainManager.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface provides access to the sendTrainCommand
  *
  */

///////////////////////////////////////////////////////////
//  ITrainManager.h
//  Implementation of the Interface ITrainManager
//  Created on:      26-Nov-2003 02:54:15 PM
///////////////////////////////////////////////////////////

#if !defined(ITrainManager_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
#define ITrainManager_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_

#include <memory>

#include "app/trains/train_agent/src/TrainInformation.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"


// forward declaration
namespace TA_Base_Core
{
	class TrainAgentEntityData;
	class TrainSettingsEntityData;
}

namespace TA_IRS_App
{
	class TrainEvent;
	class TrainCommand;
	class TrainStatusManager;
    class TrainTransactionManager;
    class ITrainEventReaderDelegator;
    class ITrainATSDelegator;
	class ITrainStateUpdateDelegator;
    class ITrainDutyDelegator;
    class ITrainCallEndDelegator;
    class ITrainDownloadDelegator;
    class ITrainEventListener;
    class TrainTimer;

	typedef std::auto_ptr<TrainEvent> TrainEventPtr;

	/**
	 * This interface provides access to the sendTrainCommand
	 */
	class ITrainManager
	{
		public:
            virtual void addListener(ITrainEventListener *listener) = 0;

            virtual void addDelegate(ITrainEventReaderDelegator *delegate, bool priorityEvent = false) = 0;
            virtual void addDelegate(ITrainATSDelegator *delegate, bool priorityEvent = false) = 0;
            virtual void addDelegate(ITrainStateUpdateDelegator *delegate, bool priorityEvent = false) = 0;
            virtual void addDelegate(ITrainDutyDelegator *delegate, bool priorityEvent = false) = 0;
            virtual void addDelegate(ITrainCallEndDelegator *delegate, TrainStatus::ResourceCategory resource) = 0;
            virtual void addDelegate(ITrainDownloadDelegator *delegate) = 0;

            virtual void sendTrainCommandNoReply(unsigned char trainID, TrainCommand& trainCommand) = 0;
            virtual TrainEventPtr sendTrainCommandWithWait(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeout = 0, unsigned int retry = 0) =0;
            virtual TrainEventPtr sendTrainCommandWithWaitNoRetries(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis = 0)=0;
			virtual TA_Base_Core::TrainSettingsEntityDataPtr getGlobalTrainAgentEntity() = 0;
			virtual TA_Base_Core::TrainAgentEntityDataPtr getLocalTrainAgentEntity() = 0;
   			virtual TrainStatusManager& getTrainStatusManager() =0;
            virtual TrainTransactionManager& getTrainTransactionManager() = 0;
            virtual TrainTimer & getTrainTimer() = 0;
    };
};
#endif // !defined(ITrainManager_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
