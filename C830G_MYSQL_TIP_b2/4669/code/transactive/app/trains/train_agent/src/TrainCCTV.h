/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainCCTV.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the CCTV commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainCCTV.h
//  Implementation of the Class TrainCCTV
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#if !defined(TrainCCTV_72E802AE_AD83_414e_BC2E_1C91D7016EE0__INCLUDED_)
#define TrainCCTV_72E802AE_AD83_414e_BC2E_1C91D7016EE0__INCLUDED_

#include <string>

#include "app/trains/train_agent/src/TrainWorkQueue.h"

#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "app/trains/train_agent/src/TrainTypes.h"

#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"

namespace TA_IRS_App
{
	class ITrainManager;
	class OperationModeManager;
	
	/**
	* Used to Encode/Decode a type of conflicts
	*/
	enum ConflictTypes
	{
		noConflict,
		preConflict,
		imminentConflict
	};

	/**
	* Storage for a CCTV conflict
	*/
	typedef struct
	{
		ConflictTypes type;
		TrainID owningTrain;
		TrainID offendingTrain;
		unsigned long stage;
	}CarrierConflict;
	
	/**
	* This class is responsible for managing all train CCTV events:
	*     Single screen events - Manage displaying 1 CCTV
	*     Quad Screen events - Manage displaying a group of 4 CCTVs
	*     Alarm images - Manage displaying alarm images.  Each alarm image
	*     represents a train alarm and is displayed to alert an operator eg.
	*     Fire Extinguisher Alarm, Doors Alarm etc
	*     CCTV states persist in the Train Information class.
	*     CCTV states are managed by the TrainStatus manager.
	*     CCTV states are synchronised with other Train Agents.
	*     CCTV events are vetted by the OperationModeManager.
	*/
	class TrainCCTV : public ITrainEventReaderDelegator
	{

		public:
			TrainCCTV(ITrainManager& callback, ITrainNotificationFacade& notifier, OperationModeManager& operationModeManager);
			virtual ~TrainCCTV();
			TrainCCTV(const TrainCCTV& theTrainCCTV);

		public:
            bool delegate(TrainEvent& event);

            void cctvCarrierOn(unsigned int trainID,std::string sessionID);
			void cctvCarrierOff(unsigned int trainID,std::string sessionID);
			void cctvQuadScreen(unsigned int trainID,std::string sessionID,unsigned char cameraW,unsigned char cameraX,
				            unsigned char cameraY,unsigned char cameraZ);
			void cctvSingleScreen(unsigned int trainID,std::string sessionID, unsigned char cameraX);
			void cctvSequence(unsigned int trainID,std::string sessionID, unsigned char sequenceNo, unsigned char sequenceMode);
			void CCTVAddAlarmImage(unsigned int trainID,std::string sessionID, const std::vector<unsigned char>& cameras);
			void CCTVRemoveAlarmImage(unsigned int trainID,std::string sessionID, unsigned char cameraX);
		
			void onCctvZoneUpdate(TA_IRS_App::ATSTrainMessage & atsMsg);

		private:

            void aboutToActivate(unsigned int trainID);
			CarrierConflict checkConflicts(TrainID trainID, bool willBeActivated, bool hasBeenDeactivated);
			static unsigned long getStageId(double cctvZone);
			static unsigned long getNextStageId(double cctvZone);
			static bool isPreConflict(double cctvZone);
			static bool isStationBoundary(double cctvZone);

			//TD 15954
			//zhou yuan++
			void resetCCTVState(unsigned int trainID);
			//++zhou yuan

			ITrainManager& m_trainManager;
            ITrainNotificationFacade& m_notifier;
			OperationModeManager& m_operationModeManager;

			unsigned int m_timerCCTV1;

	};

	/**
	* The command sent to a train containing default CCTV settings.  It is
	* sent when a train wakes up.
	*/
	class TrainCCTVDefaultTrainCommand : public AbstractTrainWorkQueueItem
	{
	public:
		TrainCCTVDefaultTrainCommand(TrainCCTV & trainCctv, TrainID trainID, const std::string & sessionID) 
			: m_trainCctv(trainCctv), m_trainId(trainID), m_session(sessionID)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainCCTV & m_trainCctv;
		TrainID m_trainId;
		std::string m_session;
	};
};
#endif // !defined(TrainCCTV_72E802AE_AD83_414e_BC2E_1C91D7016EE0__INCLUDED_)
