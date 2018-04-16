/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainPEC.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PEC commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainPEC.h
//  Implementation of the Class TrainPEC
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#if !defined(TrainPEC_INCLUDED_)
#define TrainPEC_INCLUDED_

#include <string>

//#include "bus/trains/TrainAgentCorbaDef/idl/src/IPECTrainCommandsCorbaDef.h"
#include "app/trains/train_agent/src/TrainTypes.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "app/trains/train_agent/src/UnsolicitedTrainTimer.h"

namespace TA_IRS_App
{
	class ITrainManager;
    class pecActivatedEventReader;
    class pecSelectedByDriverEventReader;
    class pecRequestForResetEventReader;
    class pecResetEventReader;
    class pecReadyForConversationEventReader;
	class OperationModeManager;

    class TrainPEC : public ITrainEventReaderDelegator,
                     public ITrainCallEndDelegator,
                     public IUnsolicitedTimerCallback
	{
    public: 
        // to decouple corba/c++
        enum PECNotification 
        {
            PEC_Activated,
            PEC_Driver,
			PEC_Operator,
            PEC_Reset,
            PEC_Continue,
            PEC_CallLost,
			PEC_ContinueTimeout,
			PEC_Invalidated
        };

        // for convenience
//        typedef struct TrainStatusManager::PECPoint PECPoint;

		public:
			TrainPEC(ITrainManager& callback, ITrainNotificationFacade& notifier, OperationModeManager& operationModeManager);
			virtual ~TrainPEC();

		public:
//            virtual delegatorCallback delegate(TrainEvent& event);
            bool delegate(TrainEvent& event);

            bool delegate(unsigned long callId, std::vector<unsigned char>& trains);


            /** 
              * Called when this agent loses control of a train.
              * Invalidates PEC calls for this train.
              */
            void invalidatePec(unsigned char trainId);

            //*** Servant implementation methods

            // invariant: on calls affecting train status, no other session using this train.

            // origin OCC
            // Interrogate TrainStatusManager for complete list of all active pecs for all trains
            TrainStatus::FullPECPointCollection getPECCalls();

            // origin OCC 
            // This call blocks until the full audio setup is complete and a (56) Ready
            // is received from TIMS; unless some other event causes premature exit. 
            // - Send (51) Answer; wait for (55) Command recv and notify OCC
            // - Update TrainStatus, If PROCEEDING, mark as PEC Call as Operator 
            // - Setup Audio
            // - Wait for PEC2 to expire or onPECReadyForConversation() to indicate status
            // postcondition: session is placed in TrainStatusInfo
            void answerPECCall(unsigned char trainID, const struct PECPoint& point, std::string sessionID);

            // origin OCC
            // Purpose: To stop talking to passenger; To force train to stop at next station
            // - Close down audio line
            // - Put PEC Call back into active list (ie. not current)
            // ! This is not a reset. Still expect (57) continues to arrive from TIMS.
            // ! How to re-establish audio? 
            // precondition: session exists and matches
            void endPECCall(unsigned char trainID, std::string sessionID);

            // origin OCC 
            // - Update TrainStatus
            // - Deactivate Audio
            // - Send TIMS (52) Reset; wait for (52) Reset ACK
            // precondition: session exists and matches ? can we force reset ?
            // if sendReset is true, the PEC will only appear reset after a successful reset command is sent
		    // if sendReset is false, the PEC will simply be removed from the active PEC list
			void resetPEC(unsigned char trainID, const struct PECPoint & point, bool sendReset, std::string sessionID);

            // Same as for resetPEC, but pass in 0 as PEC id and deal with ALL pecs
            // precondition: session exists and matches ? can we force reset ?
            // if sendReset is true, the PEC will only appear reset after a successful reset command is sent
		    // if sendReset is false, the PEC will simply be removed from the active PEC list
            void resetAllPEC(unsigned char trainID, bool sendReset, std::string sessionID);

            // origin OCC (58) Continue
            // - Update TrainStatus - manage retries
            // - Notify TIMS (58) Continue
            // precondition: session exists and matches
			void continuePECCall(unsigned char trainID, std::string sessionID);

            //*** Asynchronous-unsolicited messages

            // origin TIMS (53) Activation
            // - Update TrainStatus
            // - Notify OCC (PECActivated)
            // - If PEC call in progress, place on queue
            void onPECActivation(TrainEvent & event, pecActivatedEventReader * reader);

            // origin TIMS (54) Driver Selection
            // - Update TrainStatus - mark as Driver
            // - Notify OCC (PECDriver)
            void onPECDriverSelection(TrainEvent &event, pecSelectedByDriverEventReader * reader);

            // origin TIMS (57) Reset Req
            // - Update TrainStatus - manage retries
            // - Notify OCC (PECContinue) on 3rd retry
            void onPECResetRequest(TrainEvent &event, pecRequestForResetEventReader * reader);

            // origin TIMS (52) Reset
            // - Update TrainStatus - delete entry
            // - Notify OCC (PECReset)
            // - If Reset(0) Send ACK to TIMS (52) Reset(0)
            void onPECReset(TrainEvent &event, pecResetEventReader * reader);

            // origin TIMS (56) Ready
            // - Update TrainStatus
            // - No need to notify OCC - used to control answerCall()
            void onPECReadyForConversation(TrainEvent &event, pecReadyForConversationEventReader * reader);

            // expiry of PEC3 timer
            virtual void handleTimeout(unsigned char trainId);
			//TD 15938
			//zhou yuan++
			bool isOperatorBusy(const std::string& sessionID);
			//++zhou yuan

			/**
			 * Given a PEC number, get the string to display to the user
			 * 
			 * @return the PEC display string
			 * 
			 * @param pecNumber    The PEC number to lookup
			 */
			std::string getPecStringForPecId(int pecNumber);

        private:
            // starts the PEC 3 timer, resets it if it is running
            void startPEC3Timer(unsigned char trainId);
            // stops the PEC 3 timer, no op if not running
            void stopPEC3Timer(unsigned char trainId);
            // called when the PEC 3 times out
            void onPEC3Timeout(unsigned char trainId);

            // Keep a registry of messages we can listen to asynchronously, then 
            // figure out how to tell TrainEventSubscriber about them.

			ITrainManager& m_trainManager;
            ITrainNotificationFacade& m_notifier;

//            TA_Base_Core::CommsMessageSender* m_commsMessageSender;

//            unsigned int m_retryLimit;
            unsigned int m_timerPEC2;
            unsigned int m_timerPEC3;
            unsigned int m_timerPEC4;

            unsigned int m_retryLimit;

//            UnsolicitedTrainTimer m_pec3TimerManager;		//CL18444, hongzhi, ECP 237 - Deletion of the PEC autoreset by TIMS
			OperationModeManager& m_operationModeManager;
    };

};
#endif // !defined(TrainPEC_INCLUDED_)
