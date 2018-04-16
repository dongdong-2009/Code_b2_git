/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainStatusManager.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class manages the status of the trains.  This information will be
  * synchronised between primary and secondary agents.  In addition, the
  * information will be synchronised in agents accross sites.
  *
  */

///////////////////////////////////////////////////////////
//  TrainStatusManager.h
//  Implementation of the Class TrainStatusManager
//  Created on:      05-Dec-2003 03:41:40 PM
///////////////////////////////////////////////////////////

#if !defined(TrainStatusManager_8EE2EB3D_6407_4280_8B6D_A4052FCF34C2__INCLUDED_)
#define TrainStatusManager_8EE2EB3D_6407_4280_8B6D_A4052FCF34C2__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>

#include "app/trains/train_agent/src/TrainTypes.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TTDFrame.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "TrainInformation.h"
//#include "app/trains/train_agent/src/TrainCCTV.h"

/**
 * This class manages the status of the trains.  This information will be
 * synchronised between primary and secondary agents.  In addition, the
 * information will be synchronised in agents accross sites.
 */
namespace TA_IRS_App
{
    
	class ITrainStatusUpdateMonitor;
	class ITrainNotificationFacade;
	class ITrainManager;
	class TrainEvent;
	class TrainEventReader;
	class TrainTSIUpdateStrategy;
	class TrainStatusManager;


	class TrainStatusManager : public ITrainStateUpdateDelegator
	{
		/// @todo we don't like friends ;) try inheriting from interface to gain access to relevant methods
		friend class TrainTSIUpdateStrategyAlways;
		friend class TrainTSIUpdateStrategyStrictWakeup;

		typedef TrainStatus::TrainInformation TrainInformation;

		public:
			TrainStatusManager(ITrainStatusUpdateMonitor& callback, ITrainNotificationFacade& notifier, unsigned int autoContinueAckLimit);
			virtual ~TrainStatusManager();

			bool delegate(std::vector<TrainStatus::TrainInformation> & trainList);

			void setTSIUpdateStrategy(TrainTSIUpdateStrategy * strategy);

		public:



            static const struct PECPoint emptyPoint;

            /** 
            * Function object adapter conforming to a stl Predicate
            */
            class comparePECPoint : public std::binary_function<PECPoint, PECPoint, bool > 
            {
            public:
                result_type operator() (const first_argument_type& a, const second_argument_type& b) const
                    { return (a.car == b.car) && (a.PEC == b.PEC); /* ignore source diffs */ };
            };



		public:
			void setTrainTSI(TrainEvent & event, TrainEventReader * reader, bool isSynchronisation=false);

		protected:
			/// called by setTrainTSI(TrainEvent &, TrainEventReader *);
			void setTrainTSI(unsigned int trainID, const std::string & TSI, TrainTypes::MPUState MPU,bool isSynchronisation=false);

		public:
			bool setTrainLocation(unsigned int trainID,unsigned long locationKey,bool force=false, bool isSynchronisation=false);
            void setTrainDetails(unsigned int trainID, unsigned long currentLocation, double currentCctvZone, std::string serviceNumber, TrainTypes::Track track, bool isMute, bool localisationValid);

			/**
			* Called via ATS notification processing.
			* An invalid train is one that ATS marks as invalid. It is still possible
			* that TIMS is contactable via Radio.
			*/
			void invalidateTrain(unsigned int trainID);
			void setAtsValidityToUnknown(unsigned int trainID);

			TrainList getTrainList(bool includeInvalid = false); /// @todo add flag to also give 'invalid' trains

            std::string getTrainTSI(unsigned int trainID);
            std::string getTrainTSI(unsigned int trainID, TrainTypes::MPUState mpuState);
			unsigned int getTrainMPUState(unsigned int trainID);
            /// @param ITSI - modified to be activeTSI, if the ITSI is found
            bool isTrain(std::string & ITSI);
			bool isTrainMute(unsigned int trainID);
			unsigned long getTrainLocation(unsigned int trainID);
            std::string getTrainServiceNumber(unsigned int trainID);
			TrainTypes::Track getTrainDirection(unsigned int trainID);
			bool isTrainLocalisationValid(unsigned int trainID);
			TrainTypes::AtsValid getTrainAtsValid(unsigned int trainID);
            unsigned char trainTSI2ID(const std::string & TSI);

            /// @return "" if no owner
            std::string getOwningSessionID(unsigned int trainID, TrainStatus::ResourceCategory resource);
            /**
            * @return false on failure (session already exists) [never returns on failure]
            * @exception TrainInvalidParameterException if session in use (and not forcing it to be set explicitly)
            */
            bool setOwningSessionID(unsigned int trainID, TrainStatus::ResourceCategory resource, const std::string& sessionID, bool force = false);
            /// @return true if sessionID matches what is registered against train.
            bool amOwningSessionID(unsigned int trainID, TrainStatus::ResourceCategory resource, const std::string& sessionID);
            /// @exception TrainInvalidParameterException if session in use by someone else 
            void verifyOwningSessionID(unsigned int trainID, TrainStatus::ResourceCategory resource, const std::string& sessionID);


			// set and get the calls to the train
			void setCallAllocation(unsigned int trainID, TrainStatus::ResourceCategory resource, unsigned long callId);
			unsigned long getCallAllocation(unsigned int trainID, TrainStatus::ResourceCategory resource);
			TrainTypes::TrainStatus getTrainStatus(unsigned int trainID);
            // get trains involved in a call
            // if any trains are involved in the call they will be added to trains
            // if any trains were added to the vector, then the associated resource will be set
            // if no trains were added then obviously no resource will be set
            void getCallParticipants(unsigned long callId, std::vector<unsigned char>& trains, TrainStatus::ResourceCategory& resource);

            void setCallDriverType(unsigned int trainID, TrainStatus::ECallDriverType callType);
            TrainStatus::ECallDriverType getCallDriverType(unsigned int trainID);


            // used to manage call IDs
            void addCallToWatchList(unsigned long callId);
            void removeCallFromWatchList(unsigned long callId);
            bool isCallInWatchList(unsigned long callId);
            // called when a state update is received
            void synchroniseWatchList(const std::list<unsigned long>& newCallIdWatchList);
            // only used for sync requests
            std::list<unsigned long> getCallWatchList();


            struct PECPoint getConnectedPECPoint(unsigned int trainID);
            TrainStatus::FullPECPointCollection getActivePECPoints();
            PECPointCollection getActivePECPoints(unsigned int trainID);
			PECPoint setActivePECPoint(unsigned int trainID, const PECPoint& point, bool setCreationTime = false);
			void removeActivePECPoint(unsigned int trainID, const PECPoint& point);
			void deleteActivePECPoint(unsigned int trainID, const PECPoint& point);

            void resetContinueCounter(unsigned int trainID, TrainStatus::ResourceCategory resource);
            void incrementContinueCounter(unsigned int trainID, TrainStatus::ResourceCategory resource);
            bool exceededContinueLimit(unsigned int trainID, TrainStatus::ResourceCategory resource);

            unsigned int getPALibraryVersion(unsigned int trainID);
            void setPALibraryVersion(unsigned int trainID, unsigned int version);
            unsigned char getPAAnnouncementID(unsigned int trainID);
            void setPAAnnouncementID(unsigned int trainID, unsigned char announcementID);
			std::vector<TrainStatus::TrainVersionInfo> getPALibraryVersion();

            // Given an announcement ID, get all trains in the announcment
            std::vector<unsigned char> getTrainsForAnnouncement(unsigned char announcementID);
            // Given an announcement ID, get the audio call ID
            unsigned long getCallForAnnouncement(unsigned char announcementID);

            unsigned int getTTISLibraryVersion(unsigned int trainID);
            void setTTISLibraryVersion(unsigned int trainID, unsigned int version);
            unsigned int getTTISScheduleVersion(unsigned int trainID);
            void setTTISScheduleVersion(unsigned int trainID, unsigned int version);
            TrainTypes::TTISStatus getTTISStatus(unsigned int trainID);
            void setTTISStatus(unsigned int trainID, TrainTypes::TTISStatus status);

            void setDownloadLibrary(unsigned int trainID, const TA_IRS_App::TTDFrame::TtisFrameList & lib);
            TTDFrame::TtisFrameList getDownloadLibrary(unsigned int trainID);
            void setResendFrames(unsigned int trainID, const std::vector<unsigned int> & frames);

            std::vector<std::string> getTrainSelectionList();
            TrainList getTrainSelection(const std::string & selectionName);
            bool addTrainSelection(const std::string & selectionName, const TrainList& trainList);
            void delTrainSelection(const std::string & selectionName);

			double getTrainCctvZone(TrainID trainID);
			bool isCctvActivated(TrainID trainID);
			bool isCctvInitialised(TrainID trainID);
			bool isPreConflict(TrainID trainID);
			TrainID getPreConflict(TrainID trainID);
			void setCctvActivated(TrainID trainID, bool activated); 
			void setPreConflict(TrainID trainID, TrainID owningTrain);
			std::string getPreConflictAlarmId(TrainID trainID);
			void setPreConflictAlarmId(TrainID trainId, std::string alarmId);
			void setCctvSwitchState(TrainID trainID, TrainStatus::CctvSwitchState switchState);
			TrainStatus::CctvSwitchState getCctvSwitchState(TrainID trainID);
			void defaultTrainCCTVSettings(TrainID trainID);
			static unsigned long getCctvStageId(double cctvZone);


			// Used by TrainActivityMonitor
			void setAtcTimerItem(unsigned int trainId, long timerItem);
			long clearAtcTimerItem(unsigned int trainId);
			std::string getAtcAlarmId(unsigned int trainId);
			void setAtcAlarmId(unsigned int trainId, std::string alarmId);
			void resetAtcAlarmId(unsigned int trainId, std::string alarmId = "");
			bool noAtcAlarms();

			void setOa1TimerItem(unsigned int trainId, long timerItem);
			long clearOa1TimerItem(unsigned int trainId);
			std::string getOa1AlarmId(unsigned int trainId);
			void setOa1AlarmId(unsigned int trainId, std::string alarmId);
			void resetOa1AlarmId(unsigned int trainId, std::string alarmId = "");
			bool noOa1Alarms();
			
            std::string getRadioModeAlarmId(unsigned int trainId);
            void setRadioModeAlarmId(unsigned int trainId, std::string alarmId);
			void resetRadioModeAlarmId(unsigned int trainId, std::string alarmId = "");
            bool noRadioModeAlarms();

			/// Only called via TrainStateUpdateObserver
			void onStateChange(TrainID trainID, enum TrainStatus::SyncState syncState); 
			void onTrainStateUpdate(std::vector<TrainStatus::TrainInformation>& updateList);

			// Used during agent synchronisation
			void setSyncState(TrainID trainID, TrainStatus::SyncState syncState);

			//TD 15938
			//zhou yuan++
			bool isOperatorBusy(const std::string& sessionID, TA_IRS_App::TrainStatus::ResourceCategory resource);
			//++zhou yuan

        private:

			TrainStatusManager(const TrainStatusManager& theTrainStatusManager);
			TrainInformation* getTrainInformation(unsigned int trainID);
			TrainInformation* addTrainInformation(unsigned int trainID);
			TrainInformation* getOrAddTrainInformation(unsigned int trainID);

			std::string printTrainTSIAssignment(TrainID trainID);
			void setCctvInitialised(TrainID trainID, bool initialised);
			void clearActivePECPoints(unsigned int trainID);
			void setConnectedPECPoint(unsigned int trainID, const PECPoint& point);
			std::string getResourceSessionID(TrainID trainID, const TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource);
			unsigned long getResourceCallID(TrainID trainID, const TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource);

			typedef   std::map<int,TrainInformation*>  TrainInformationMap;
			TrainInformationMap m_trainInfoMap;

            typedef std::map<std::string, TrainList> TrainSelectionMap;
            TrainSelectionMap m_trainSelectionMap;

			/**
			 * Control concurrent access to the stored information
			 */
			TA_Base_Core::ReEntrantThreadLockable   m_storeLock;
			/**
			 * Holds details of callback, notified when status store is updated.
			 */
			ITrainStatusUpdateMonitor&   m_callback;
			ITrainNotificationFacade&	 m_notifier;
			
			unsigned int m_autoContinueAckLimit;

			/*** for sync ***/

			enum DirtyField
			{
				Dirty_Anything,   /// except for TIS library
				Dirty_TISLibrary
				/* not used yet...
				Dirty_Resources,
				Dirty_Location,
				Dirty_TSI,
				Dirty_TTISState,
				Dirty_PAState,
				Dirty_CallDriverState,
				Dirty_PECState
				*/
			};
			/// Track relevant fields that need synchronisation
			std::map<TrainID, std::vector<DirtyField> > dirtyData;



			TrainTSIUpdateStrategy * m_tsiUpdateStrategy; // set strategy via ctor

            // this is used to keep track of all open calls.
            // when a call ended comes in if the call id is in this list
            // action needs to be taken, otherwise it can be ignored
            TA_Base_Core::NonReEntrantThreadLockable   m_callLock;
            std::list<unsigned long> m_callIdWatchList;
	};

	/****************************/

	/// @note TrainStatusManager is the context. setTrainTSI() is the interface.
	class TrainTSIUpdateStrategy
	{
	public:
		TrainTSIUpdateStrategy( TrainStatusManager & mgr );

		virtual void setTSI( TrainEvent & event, const TrainEventReader * reader, bool isSync ) = 0;
	protected:
		TrainStatusManager & m_context;
	};

	class TrainTSIUpdateStrategyAlways : public TrainTSIUpdateStrategy
	{
	public:
		TrainTSIUpdateStrategyAlways( TrainStatusManager & mgr ) : TrainTSIUpdateStrategy(mgr) {};
		virtual void setTSI( TrainEvent & event, const TrainEventReader * reader, bool isSync );
	};

	class TrainTSIUpdateStrategyStrictWakeup : public TrainTSIUpdateStrategy
	{
	public:
		TrainTSIUpdateStrategyStrictWakeup( TrainStatusManager & mgr ) : TrainTSIUpdateStrategy(mgr) {};
		virtual void setTSI( TrainEvent & event, const TrainEventReader * reader, bool isSync );
	};

	/**
	* To enable train state update notification to be sent.
	*
	* Makes use of destructor to automatically notify TrainStateManager to 
	* perform the update when the class leaves scope. 
	*
	* To use, calling method instantiates object.
	*
	* @note Is this really a variation of the Observer pattern, considering usage,
	*       or does another pattern match better?
	*/
	class TrainStateUpdateObserver
	{
	public:
		TrainStateUpdateObserver(TrainStatusManager & mgr, TrainID train, enum TrainStatus::SyncState syncState);
		void synchronise();
		~TrainStateUpdateObserver();

	private:
		TrainStatusManager & m_trainStatusManager;
		TrainID m_trainID;
		enum TrainStatus::SyncState m_syncState;
		bool m_synchronise;
	};



}; // Namespace

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::TrainInformation & x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::SessionAllocationMap& x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::CallAllocationMap& x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::AutoContinueAckMap& x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::SyncState& x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::CctvSwitchState& x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::PECPointCollection& x);

#endif // !defined(TrainStatusManager_8EE2EB3D_6407_4280_8B6D_A4052FCF34C2__INCLUDED_)
