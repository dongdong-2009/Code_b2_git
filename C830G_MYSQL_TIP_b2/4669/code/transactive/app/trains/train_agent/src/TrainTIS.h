/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainTIS.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainTIS.h
//  Implementation of the Class TrainTIS
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#if !defined(TRAINTIS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
#define TRAINTIS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_

#include <string>

//#include "bus/trains/TrainAgentCorbaDef/idl/src/ITISTrainCommandsCorbaDef.h"

#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "app/trains/train_agent/src/UnsolicitedTrainTimer.h"
#include "app/trains/train_agent/src/TrainWorkQueue.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/TrainBarrier.h"

namespace TA_IRS_App
{
	class ITrainManager;
    class ttisCommandReceivedEventReader;
    class ttisEndOfDownloadEventReader;
	struct TimeInterval 
	{
		unsigned char startHour;
		unsigned char startMin;
		unsigned char stopHour;
		unsigned char stopMin;
	};


    class TrainTIS : public ITrainEventReaderDelegator,
                     public ITrainDownloadDelegator,
                     public IUnsolicitedTimerCallback
    {

    public:
        // to decouple corba/c++
        enum TISNotification 
        {
            TIS_LibraryDownload,
            TIS_ScheduleDownload,
			TIS_LibraryVersion
        };

        struct TisReturnStatus
		{
			unsigned char trainID;
            std::string sessionID;
			bool		success;
            std::string	errorDetails;
		};

        typedef std::vector<unsigned char> LibraryDataType;

        TrainTIS(ITrainManager& callback, ITrainNotificationFacade& notifier,
			TrainWorkQueueManager& trainWorkQueueManager,
			OperationModeManager& operationModeManager);
        virtual ~TrainTIS();

	public:
        bool delegate(TrainEvent& event);
        bool delegate(unsigned char trainId, bool success, const std::string& failReason);

        //*** Servant implementation methods
		short getLibraryVersion(unsigned char trainID);
		short getScheduleVersion(unsigned char trainID);


		std::vector<TisReturnStatus*> displayFreeTextMessageParallel(const std::vector<unsigned char>,const std::vector<unsigned char>,
			unsigned char,unsigned char, const struct TimeInterval &,unsigned char,const char *,const char *);

        
        std::vector<TisReturnStatus*> displayStoredMessageParallel(const std::vector<unsigned char>,const std::vector<unsigned char>,
			short,unsigned char,unsigned char,unsigned char,const struct TimeInterval ,const char *);

        
        std::vector<TisReturnStatus*> resetMessageParallel(const std::vector<unsigned char>,const std::vector<unsigned char>,
			unsigned char,const char*);

        // origin OCC 
        // - Send (31) FreeText; wait for (42) CMD Recv [Timer:TIS1]
        void displayFreeTextMessage(unsigned char,unsigned char,unsigned char,unsigned char,
            const struct TimeInterval &,
            unsigned char,const char *,const char *);

        // origin OCC 
        // - Send (32) PreDefined; wait for (42) CMD Recv [Timer:TIS1]
        void displayStoredMessage(unsigned char,unsigned char,short,unsigned char,unsigned char,
            unsigned char,const struct TimeInterval &,	const char *);
            

        // origin OCC
        // - Send (35) Reset; wait for (42) CMD Recv [Timer:TIS1]
        void resetMessage(unsigned char,unsigned char,unsigned char);

        // origin OCC
        // - Send (33) Library Download; wait for (42) CMD Recv [Timer:TIS1]
        // - Do the download
        void libraryDownload(unsigned char,const LibraryDataType &,const char *);

        // origin OCC
        // - Send (43) Library Upgrade; wait for (42) CMD Recv [Timer:TIS1]
        // - Update TrainStatus
        void libraryUpgrade(unsigned char,const char *);

        // origin OCC
        // - Send (44) Schedule Download; wait for (42) CMD Recv [Timer:TIS1]
        // - Do the download
        void scheduleDownload(unsigned char,const LibraryDataType &,const char *);

        // origin OCC
        // - Send (45) Schedule Upgrade; wait for (42) CMD Recv [Timer:TIS1]
        // - Update TrainStatus
        void scheduleUpgrade(unsigned char,const char *);

        //*** Asynchronous-unsolicited messages

        // origin TIMS (39) End of Download
        // - check status of download
        // - notify OCC (TISLibraryDownload)
        void onTISEndOfDownload(TrainEvent & event, ttisEndOfDownloadEventReader * reader);

        virtual void handleTimeout(unsigned char trainId);

	private:

        void startDownloadTimer(unsigned char trainId);
        void stopDownloadTimer(unsigned char trainId);
        void onDownloadTimeout(unsigned char trainId);

		ITrainManager& m_trainManager;
        ITrainNotificationFacade& m_notifier;

        unsigned int m_timerTIS1;
        unsigned int m_timerTIS2;

        UnsolicitedTrainTimer m_ttis2TimerManager;
        TrainWorkQueueManager& m_workQueueManager;
		OperationModeManager& m_operationModeManager;
    };

	class TisDisplayFreeTextMessageCommand : public AbstractTrainWorkQueueItem
	{	
	public:
		TisDisplayFreeTextMessageCommand(
			TrainTIS& trainTIS,
            ITrainNotificationFacade& notifier,
			unsigned char trainID,
			std::vector<unsigned char> PIDs,
			unsigned char displayAttribute,
			unsigned char priority,
			const TimeInterval & displayPeriod,
			unsigned char repeatInterval,
			const char* message,
			const char* sessionID,
            TrainBarrier& barrier,
            TrainTIS::TisReturnStatus* returnStatus,
			OperationModeManager& operationModeManager
			):
			m_trainTis(trainTIS),
            m_notifier(notifier),
			m_trainID(trainID),		
			m_PIDs(PIDs),	
			m_displayAttribute(displayAttribute),
			m_priority(priority),
			m_displayPeriod(displayPeriod),
			m_repeatInterval(repeatInterval),
			m_message(message),
			m_sessionID(sessionID),
			m_barrier(barrier),
            m_returnStatus(returnStatus),
			m_operationModeManager(operationModeManager)
			{			
			}
			
			virtual int call();
			virtual std::string info();
		
	private:		
		TrainTIS& m_trainTis;
        ITrainNotificationFacade& m_notifier;
		unsigned char m_trainID;	
		std::vector<unsigned char>   m_PIDs;
        unsigned char m_displayAttribute;
        unsigned char m_priority;
		const TimeInterval& m_displayPeriod;
        unsigned char m_repeatInterval;
		const char* m_message;
		const char* m_sessionID;
		TrainBarrier&  m_barrier;	
        TrainTIS::TisReturnStatus* m_returnStatus;
		OperationModeManager& m_operationModeManager;
	};


	class TisDisplayStoredMessageCommand : public AbstractTrainWorkQueueItem
	{				
	public:
		TisDisplayStoredMessageCommand( 
			TrainTIS& trainTIS,
			ITrainNotificationFacade& notifier,
			unsigned char trainID,
			std::vector<unsigned char> PIDs,
			unsigned char libraryVersion,
			unsigned char identifierType,
			unsigned char identifierOffset,
			unsigned char priority,
			const TimeInterval  displayPeriod,
			std::string sessionID,
            TrainBarrier& barrier,
			TrainTIS::TisReturnStatus* returnStatus,
			OperationModeManager& operationModeManager)
			: m_trainTis(trainTIS),
			  m_notifier(notifier),
              m_trainID(trainID),
			  m_PIDs(PIDs),
              m_libraryVersion(libraryVersion),
			  m_identifierType(identifierType),
			  m_identifierOffset(identifierOffset),
			  m_priority(priority),
              m_displayPeriod(displayPeriod),
              m_sessionID(sessionID),
              m_barrier(barrier),
			  m_returnStatus(returnStatus),
			  m_operationModeManager(operationModeManager)
			{			

			}

        virtual int call();
		virtual std::string info();

	private:
        TrainTIS& m_trainTis;		
		ITrainNotificationFacade& m_notifier;
		unsigned char m_trainID;	
		std::vector<unsigned char>   m_PIDs;
        short m_libraryVersion;
		unsigned char m_identifierType;
		unsigned char  m_identifierOffset;
        unsigned char m_priority;
        const TimeInterval m_displayPeriod;
		std::string  m_sessionID;
        TrainBarrier&   m_barrier;
		TrainTIS::TisReturnStatus* m_returnStatus;
		OperationModeManager& m_operationModeManager;
	};

	class TisResetMessageCommand : public AbstractTrainWorkQueueItem
	{		
	public:
		TisResetMessageCommand(
			TrainTIS& trainTIS,
			ITrainNotificationFacade& notifier,
			unsigned char trainID,
			std::vector<unsigned char> PIDs,
			unsigned char messageType,
			const char* sessionID,
            TrainBarrier& barrier ,
			TrainTIS::TisReturnStatus* returnStatus,
			OperationModeManager& operationModeManager)
			: m_trainTis(trainTIS),
			  m_notifier(notifier),
              m_trainID(trainID),
			  m_PIDs(PIDs),	             
              m_messageType(messageType),
			  m_sessionID(sessionID),
              m_barrier(barrier),
			  m_returnStatus(returnStatus),
			  m_operationModeManager(operationModeManager)
			{
			}

        virtual int call();
		virtual std::string info();
		
	private:
        TrainTIS& m_trainTis;
		ITrainNotificationFacade& m_notifier;
		unsigned char m_trainID;
		std::vector<unsigned char>  m_PIDs;
        unsigned char m_messageType;
		const char*  m_sessionID;
        TrainBarrier&    m_barrier;
		TrainTIS::TisReturnStatus* m_returnStatus;
		OperationModeManager& m_operationModeManager;
	};

};
#endif // !defined(TRAINTIS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
