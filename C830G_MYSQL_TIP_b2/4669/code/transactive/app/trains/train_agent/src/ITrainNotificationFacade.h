/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/ITrainNotificationFacade.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  ITrainNotificationFacade.h
//  Implementation of the Interface ITrainNotificationFacade
//  Created on:      11-Dec-2003 10:05:04 AM
///////////////////////////////////////////////////////////

#if !defined(ITrainNotificationFacade_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_)
#define ITrainNotificationFacade_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <string>
#include <vector>
#include <omniORB4/CORBA.h>
#include "app/trains/train_agent/src/TrainTypes.h"


/// @todo separate TrainInformation struct to avoid this include
#include "app/trains/train_agent/src/TrainStatusManager.h"

#include "core/message/src/AuditMessageSender.h"

//	struct TrainInformation;

/**
 * This interface defines the call backs used to notify a client of changes to the
 * train status.  It will be used by the TrainAgent to synchronise the primary and
 * secondary train agents and to notify other location agents.
 */
namespace TA_IRS_App
{
//	typedef struct TrainInformation TrainInformation_T;

    class ITrainNotificationFacade
	{

	public:
//		typedef unsigned char Oa1Table[24];
//		typedef unsigned char AtcTable[22];
		typedef std::vector<unsigned char> Oa1Table;
		typedef std::vector<unsigned char> AtcTable;


		enum RadioCallType { RADIO_PEC, RADIO_PA, RADIO_DRIVER, RADIO_DRIVER_GROUP };

        enum EPaErrorType
		{
			NoError,
			InvalidParameter,
			TrainTimeout,
			TrainBusy,
			TrainGeneralFailure,
            TrainPaFirstCyclicCancelled,
            TrainAccessDenied
		};
        
		virtual void notifyATSofOa1(unsigned char trainID, Oa1Table x) = 0;
		virtual void notifyATSofAtc(unsigned char trainID, AtcTable x) = 0;
        // no notifications...        virtual void notifyCCTV() = 0;
        virtual void notifyPEC(unsigned char notification, TrainID trainID, PECPoint pec, unsigned char track, const SessionID & sessionID) = 0;
        virtual void notifyPEC(unsigned char notification, TrainID trainID, PECPointCollection peclist, unsigned char track, const SessionID & sessionID) = 0;
//        virtual void notifyPEC(unsigned char notification, unsigned char trainID, unsigned char car, unsigned char pec, unsigned char source, unsigned long creationTime, unsigned char track, const std::string & sessionID) = 0;
        virtual void notifyPublicAddress(unsigned char notification, unsigned char trainID, unsigned char announcementID, const std::string & sessionID, EPaErrorType error = NoError, const std::string& errorDetails = "", bool canOverride = false) = 0;
        virtual void notifyPublicAddressAnnouncement(unsigned char notification, unsigned char announcementID ) = 0;
        virtual void notifyPublicAddress(unsigned char trainID, short libraryVersion) = 0;
        virtual void notifyTIS(unsigned char notification, unsigned char trainID, bool success, const std::string & sessionID, short libVer = 0, short schedVer = 0) = 0;
        virtual void notifyCCTV(unsigned char trainID, bool carrierOn, bool isPreConflict, unsigned long stageId, const TrainStatus::CctvSwitchState& switchState) = 0;
		virtual void callRadioSendData(const std::string &ITSI, const std::vector<unsigned char>& data, const std::string & sessionID) = 0;
		virtual void callRadioCancelSendData(const std::string &ITSI) = 0;
        virtual void callRadioCallDriver(bool isStart, unsigned char trainID) = 0;
        virtual void notifyMisc(unsigned char notification, unsigned char trainID, const std::string & sessionID) = 0;
		virtual unsigned long callRadioMakeCall(unsigned char trainID, RadioCallType type, const std::string & sessionID) = 0;
		virtual unsigned long callRadioMakeCall(const std::vector<std::string>& ITSIList, RadioCallType type, const std::string & sessionID, long locationKey = 0 ) = 0;//TD19783
		virtual void callRadioReleaseCall(unsigned long callId, const std::string & sessionID) = 0;
		virtual void raiseCctvPreConflictAlarm(TrainID offendingTrain, TrainID owningTrain) = 0;
		virtual void closeCctvPreConflictAlarm(TrainID trainId) = 0;
		virtual void raiseCctvConflictAlarm(TrainID offendingTrain, TrainID owningTrain) = 0;
		virtual void defaultTrainCCTV(TrainID trainId) = 0;
		/**
		* Send TrainStatusUpdate to interested consumers
		* @todo How to handle TIS library
		*/
		virtual void sendSyncMessage(std::vector<TrainStatus::TrainInformation> &) = 0;
//		virtual void sendSyncMessage(std::vector<TrainInformation_T> &) = 0;
        virtual void sendSyncMessage(const std::list<unsigned long>& callList) = 0;
		virtual void sendOperationModeSyncMessage() = 0;

		/**
		* Request TrainStatusUpdate from somewhere else
		* @todo How to handle TIS library
		* @todo Maybe use idl method instead? But we would need to locate the TrainAgent NamedObject at another location.
		*/
		virtual void sendSyncRequest(unsigned int agentLocationKey, TrainList &) = 0;

        virtual void AuditOperation(const TA_Base_Core::MessageType& messageType, TA_Base_Core::DescriptionParameters parameters, std::string sessionId) = 0;

    };
};
#endif // !defined(ITrainNotificationFacade_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_)
