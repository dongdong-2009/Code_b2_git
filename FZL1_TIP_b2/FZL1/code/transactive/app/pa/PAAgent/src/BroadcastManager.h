#ifndef BroadcastManager_H
#define BroadcastManager_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  A. Parker
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  */

#include "core/corba/src/ServantBase.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/pa/pa_agent/IDL/src/IStationPaMethod.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/ModbusPacket.h"


namespace TA_Base_Bus
{ 
    class RightsLibrary;
}

namespace TA_IRS_App
{

	class BroadcastWorker;
    class ScheduleBroadcastWorker;
    class TriggeredBroadcastWorker;

	/**
	 * This class manages the state of broadcasts within the PA Agent.
	 */
    class BroadcastManager : public virtual POA_TA_Base_Bus::IStationPaMethod, public virtual TA_Base_Core::ServantBase
	{

	public:

	    /**
	     * Constructs an instance of this class.
	     * @param tableManager The table manager instance that will allow access
	     *                     to the table instances.
	     */
	    BroadcastManager();
	    virtual ~BroadcastManager();

	    void enable();
	    void disable();

		void setModeChangingFlag(bool flag);


        // IDL Interface functions
        virtual void agentLinkTest();
        virtual bool hardwareLinkTest(const char* broadcastId, const char* sessionId);
        virtual char* broadcastStationScheduleDva( const ::TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, ::CORBA::UShort dwellInSecs, ::CORBA::Boolean hasChime, ::CORBA::Boolean isSynchronisedWithTis, const char* groupName, const ::TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams& stCyclicParams, const char* sessionId );
        virtual char* broadcastStationDva( const ::TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, ::CORBA::UShort dwellInSecs, ::CORBA::Boolean hasChime, ::CORBA::Boolean isSynchronisedWithTis, const char* groupName, const char* sessionId );
        virtual char* playbackStationDva( CORBA::ULong messageKey, ::CORBA::UShort sourceId, const char* sessionId );
        virtual void stopBroadcastStationDva( const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId );
        virtual void enableZoneMonitoring( CORBA::Boolean bEnable, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId );
        virtual char* broadcastStationLive( ::CORBA::Boolean hasChime, ::CORBA::UShort sourceId, const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* groupName, const char* sessionId );
        virtual void stopBroadcastStationLive(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId);
        virtual void stopPlaybackStationDva( const char* sessionId );
        virtual TA_Base_Bus::IPAAgentCorbaDef::EBackupKeyState getBackupKeyStatus(const char* backupName);
        virtual void broadcastSigTriggeredStationDva(CORBA::ULong ulEventType, CORBA::ULong ulSrcStationLocationKey, CORBA::ULong ulDestStationLocationKey, CORBA::ULong ulDestPlatformId, CORBA::Boolean bIsLastTrain, CORBA::Boolean bTrainInfoValid, TA_Base_Core::DateTime dtStartTime);
        virtual void broadcastFasTriggeredStationDva(CORBA::ULong ulAlarmEntityKey, CORBA::ULong ulSrcStationLocationKey);
		virtual void broadcastStationMusic(TA_Base_Bus::IPAAgentCorbaDef::EMusicType musicType, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId);
        virtual char* recordAdhocMessage( ::CORBA::UShort sourceId, ::CORBA::ULong messageKey, ::CORBA::UShort usMsgType, const char* sessionId );
        virtual void retryStationBroadcast(const char* broadcastId, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId);
        virtual void terminateBroadcast(const char* broadcastId, const char* sessionId);
        virtual void restoreBroadcast( const char* broadcastId, const char* sessionId );
        virtual void removeBroadcast( const char* broadcastId, const char* sessionId );
        virtual void removeScheduleBroadcast( CORBA::ULong ulScheduleKey, const char* sessionId);
		virtual void forceTerminateAllExistPA(); //wuzhongyi CL-19136

        virtual ::CORBA::Boolean setAdhocType( ::CORBA::ULong messageKey, ::CORBA::UShort messageType, const char* sessionId );
        virtual ::CORBA::Boolean setAdhocLabel( ::CORBA::ULong messageKey, const char* label, const char* sessionId );
        virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* getBroadcastIdTypes();
        virtual void changeBroadcastId(const char* fromBroadcastId, const char* toBroadcastId, const char* sessionId);
        virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* getBroadcastConfig(const char* broadcastId);
        virtual TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* getProgressReport(const char* broadcastId);
        virtual TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* getStationDvaMessageRecords();
        virtual TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* getTrainDvaMessageRecords();
        virtual TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* getDvaVersionRecords();

        virtual TA_Base_Bus::IPAAgentCorbaDef::EMusicType getMusicStatus(const char* sessionId);

        virtual void broadcastEventTriggeredStationDva(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
        virtual void broadcastEventTriggeredStationDvaForStation(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId);
        virtual void broadcastEventTriggeredStationDvaForArea(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const char* zoneArea);
        virtual void broadcastEventTriggeredStationDvaForGroup(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const char* zoneGroupLabel);

        TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* getCompleteBroadcastStateSeqCopy();

        void processPAAgentNewBroadcastUpdate( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* updateData );
        void processPAAgentNewBroadcastUpdate( TA_Base_Bus::IPAAgentCorbaDef::TriggeredBroadcastConfig* updateData );
            // throw CorruptInternalStateDataException
        void processPAAgentRemovedBroadcastUpdate( unsigned long ulScheduleKey );
            // throw CorruptInternalStateDataException
        void processPAAgentRenamedBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData* updateData );
            // throw CorruptInternalStateDataException
        void processPAAgentBroadcastParameterUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter* updateData );
            // throw CorruptInternalStateDataException
        void processPAAgentBroadcastProgressUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* updateData );
            // throw CorruptInternalStateDataException
        void processPAAgentEntireAgentStateUpdate( TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* updateData );
            // throw CorruptInternalStateDataException


        public:

        static const std::string ATS_AGENT_SESSION_ID;
        static const std::string MANAGER_LAUNCHING_BROADCAST_ID;


        protected:

        // used by all event triggered station broadcast methods
        void broadcastEventTriggeredStationDvaInternal(
            const std::string& broadcastId, unsigned long messageKey, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
            unsigned long ulCyclicTimes, unsigned long ulDelaySeconds, TRIGGER_EVENT_TYPE eEventType );

        std::string getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType,
                                      const std::string& extraInformation = "");
            // TA_THROW CorruptInternalStateDataException

        bool isRightsCheckingAvailable();

        void throwAccessDeniedExceptionIfCannotPerformAction(unsigned long action, const std::string& sessionId, const std::string& broadcastId="");
            //throw(TA_Base_Core::AccessDeniedException);


        TA_Base_Core::ReEntrantThreadLockable                    m_lock;
        AbstractBroadcastCircularList*                      m_allBroadcasts;
        BroadcastWorker*                                    m_broadcastWorker;
        ScheduleBroadcastWorker*                     m_pScheduleBroadcastWorker;
        TriggeredBroadcastWorker*                     m_pTriggeredBroadcastWorker;
        TA_Base_Bus::RightsLibrary*                         m_rightsLibrary;
        unsigned long                                       m_resourceId;

        // m_MusicType: One of following values
        //  NoOccMusic or NoLocalMusic if no music is playing
        //  LocalMusic if local music is playing
        //  OccMusic if OCC music is playing
        TA_Base_Bus::IPAAgentCorbaDef::EMusicType m_MusicType;
    };


} // namespace TA_IRS_App

#endif

