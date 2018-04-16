/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasHelpers.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(PASHELPERS_INCLUDED)
#define PASHELPERS_INCLUDED

#include <map>
#include <set>
#pragma warning ( disable : 4250 )
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#pragma warning ( default : 4250 )

#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core 
{ 
    class AuditMessageSender; 
    class StateUpdateMessageSender; 
    class CommsMessageSender;
    class NonUniqueAlarmHelper;
	class AlarmHelper;
    class MessageType;
}

#if defined(WIN32) && defined(_CONSOLE) && defined(_DEBUG)
    #define LOGOUT(x)                   \
    {                                   \
        std::cout << x << std::endl;    \
    }                                   
#else
    #define LOGOUT(x)
#endif

namespace TA_Base_Bus
{
    template<class DataType> class DataBlock;
}

namespace TA_IRS_App
{
class AbstractBroadcast;
class CachedConfig;
struct StdStationDvaConfig;
struct StdStationLiveConfig;
struct StdStationRecordingConfig;
struct ScheduleDvaConfig;
struct StdStationMusicConfig;

class PasHelpers  
{

public:

    PasHelpers();
    ~PasHelpers();

    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static PasHelpers* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();
	static std::string getProgressLogStr(AbstractBroadcast& broadcast);

    void corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refCorbaConfig, ScheduleDvaConfig& stToConverted );
    void corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refCorbaConfig, StdStationDvaConfig& stToConverted );
    void corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig& refCorbaConfig, StdStationLiveConfig& stToConverted );
    void corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationRecordingConfig& refCorbaConfig, StdStationRecordingConfig& stToConverted );
    void corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationMusicConfig& refCorbaConfig, StdStationMusicConfig& stToConverted );

	void stdConfigToCorbaConfig( const ScheduleDvaConfig& refStConfig, TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& corbaToConverted );

    void zoneKeysToCoverage(const std::vector<unsigned long>& keys, TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage);

    void coverageToZoneKeys(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, std::vector<unsigned long>& keys );

	bool isSourcePagingConsole(unsigned short sourceId, const std::string& session);	// TD19669

    unsigned short convertDaysInWeek( const bool bSun, const bool bMon, const bool bTue, const bool bWed, const bool bThu, const bool bFri, const bool bSat ); 
    void convertTimeToBcdFormat( const time_t tmTime, unsigned short& usHourMinute, unsigned short& usSecond );
	unsigned long convertTimeToHourMin( const time_t tmTime );
	unsigned short convertShortToBcdFormat( unsigned short usValue);

    std::string formatTrainIdForAudit( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& failedTrains );
    std::string formatDvaPriorityForAudit( const bool bEmergency );
    std::string formatDvaVersionForAudit( const unsigned long ulVersion );
    std::string formatDvaMsgIdForAudit( const unsigned long ulMsgKey );
    std::string formatDvaMsgNameForAudit( const unsigned long ulMsgKey );
    std::string formatDvaMsgNamesForAudit( const  TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& seqMessages );
    std::string formatFailReasonForAudit( const int localState );

    void formatPasOccZoneMatrix( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData );
	void formatPasStnZoneMatrix( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData );
    void formatPasOccStationMatrix( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData );
	//Added by lixiaoxia
    void formatMultiMessagesIds( const std::vector<unsigned short>& vecMsgIds, TA_Base_Bus::DataBlock<unsigned short>& dbData );

    void throwBroadcastIdNotFoundException(const std::string& what, const std::string& broadcastId=""); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    void throwFepcCommsFailureException(const std::string& what, const std::string& broadcastId=""); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    void throwBroadcastLimitReachedException(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    void throwTrainAgentCommsFailureException(const std::string& what = "Failure to communicate with local Train Agent", const std::string& broadcastId="");//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    void throwAccessDeniedException(const std::string& reason = "Action is not permitted", const std::string& broadcastId="");//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    void throwPaErrorException(const int eError, const std::string& what); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

    ////////////////////////////////////////////////////////
    //
    //            Audit message senders
    //
    ////////////////////////////////////////////////////////

    // @exception PaErrorException if one of the given parameters is invalid
    void submitPaBroadcastStationDvaRequestAuditMessage( bool hasChime, 
        const TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams* pStationCyclicParams,
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, 
        const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const std::string& sessionId );


    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaBroadcastStationLiveRequestAuditMessage( 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId );

	void submitPaBroadcastStationMusicRequestAuditMessage( 
        const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& musicType, const std::string& sessionId );

	// @exception PaErrorException if one of the given parameters is invalid
    void submitPaRecordAdhocRequestAuditMessage( unsigned short sourceId, 
        unsigned long messageKey, unsigned short usMsgType, const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaRetryStationBroadcastRequestAuditMessage( const int broadcastType,
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage( const std::string& broadcastId,
                                                                       unsigned long messageKey, 
                                                                       const std::string& messageVersionNumber, 
                                                                       bool hasChime, 
                                                                       bool isSynchronisedWithTis, 
                                                                       unsigned long durationInSecs, 
                                                                       unsigned short periodInSecs, 
                                                                       const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                                                                       const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage( const std::string& broadcastId,
                                                                                 unsigned long messageKey,
                                                                                 const std::string& messageVersionNumber,
                                                                                 bool hasChime,
                                                                                 bool isSynchronisedWithTis,
                                                                                 unsigned long durationInSecs,
                                                                                 unsigned short periodInSecs,
                                                                                 unsigned long stationKey,
                                                                                 const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage( const std::string& broadcastId,
                                                                              unsigned long messageKey,
                                                                              const std::string& messageVersionNumber,
                                                                              bool hasChime,
                                                                              bool isSynchronisedWithTis,
                                                                              unsigned long durationInSecs,
                                                                              unsigned short periodInSecs,
                                                                              const std::string& zoneArea,
                                                                              const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage( const std::string& broadcastId,
                                                                               unsigned long messageKey,
                                                                               const std::string& messageVersionNumber,
                                                                               bool hasChime,
                                                                               bool isSynchronisedWithTis,
                                                                               unsigned long durationInSecs,
                                                                               unsigned short periodInSecs,
                                                                               const std::string& zoneGroupLabel,
                                                                               const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaStationBroadcastResultAuditMessage( const std::string& broadcastId,
                                                     const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& failedZones,
                                                     bool broadcastFailed,
													 std::map<int, std::string> failedReasons,//TD10320
                                                     const std::string& sessionId = "" );

    void submitPaTrainBroadcastResultAuditMessage( const std::string& broadcastId,
                                                   const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& failedTrains,
                                                   bool broadcastFailed,
                                                   const std::string& sessionId = "" );

	void submitPaTerminateBroadcastAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                                                 const std::string& sessionId );

	void submitPaRemoveBroadcastAuditMessage( const std::string& broadcastId,
                                              const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaSetAdhocTypeAuditMessage( unsigned short oldType,
                                           unsigned short newType,
                                           unsigned long messageId,
                                           const std::string& sessionId );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaSetAdhocLabelAuditMessage( const std::string& oldLabel,
                                            const std::string& newLabel,
                                            unsigned long messageKey,
                                            const std::string& sessionId );

	void submitPaChangeBroadcastIdAuditMessage( const std::string& oldBroadcastId,
                                                const std::string& newBroadcastId,
                                                const std::string& sessionId );

	void submitPaAgentBroadcastStateAuditMessage( const std::string& broadcastId, const int broadcastState );

	void submitPaAgentBroadcastStateFailureAuditMessage( const std::string& broadcastId, const int globalState );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitPaAgentDVAMessageUpdateAuditMessage( const unsigned short oldType,
                                                    const unsigned short newType,
                                                    const std::string& oldLabel,
                                                    const std::string& newLabel,
                                                    const unsigned long messageKey );

    // @exception PaErrorException if one of the given parameters is invalid
	void submitTrainPAPrerecordedAuditMessage( const std::string& broadcastId,
                                               const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                               unsigned long messageKey, 
                                               unsigned long libraryVersion, 
                                               const bool bEmergency,
                                               const std::string& sessionId );

    void submitTrainPAPrerecordedAbortAuditMessage( const std::string& broadcastId,
                                                  const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                  const std::string& sessionId );

	void submitTrainPACyclicAuditMessage( const std::string& broadcastId,
                                          const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                          unsigned long messageKey, 
                                          unsigned long libraryVersion, 
                                          const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams,
                                          const std::string& sessionId );

	void submitTrainPASetupLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                         const std::string& sessionId );

	void submitTrainPAContinueLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                            const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
															const std::string& sessionId );

	void submitTrainPAEndLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                       const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
													   const std::string& sessionId );

	void submitPaZoneBroadcastFailureAuditMessage( unsigned long failedZone,
                                                   const std::string& reason,
                                                   const std::string& sessionId = "" );

	void submitPaTrainBroadcastFailureAuditMessage( unsigned long trainId,
                                                    unsigned long location,
                                                    const std::string& reason,
                                                    const std::string& sessionId = "" );

    void submitPaTrainBroadcastFailureAuditMessages( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                     unsigned long location,
                                                     const std::string& reason,
                                                     const std::string& sessionId = "" );

	void submitTerminateFireCountdownAuditMessage( const unsigned int usStationId, const std::string& sessionId = "" );

	void submitPAAgentOnlineUpdateAuditMessage( const std::string& parameter );
    
	// TD15128
	void submitPaPrioritySchemeUpdateAuditMessage();
	// TD15128


    ////////////////////////////////////////////////////////
    //
    //            State update message senders
    //
    ////////////////////////////////////////////////////////


    void sendPAAgentBroadcastParameterUpdate( const int parameter, const CORBA::Any& data, const std::string& broadcastId);
    void sendPAAgentRemovedBroadcastUpdate(const std::string& updateData);
    void sendPAAgentNewBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& updateData);
    void sendPAAgentRenamedBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData);
    void sendPAAgentPaSysParamUpdate(const TA_Base_Bus::IPAAgentCorbaDef::PaSysParameter& updateData);
    void sendPAAgentPasScheduleDvaIdUpdate(const std::set<unsigned short>& setIds);

    ////////////////////////////////////////////////////////
    //
    //            Comms message senders
    //
    ////////////////////////////////////////////////////////

    void sendCurrentBroadcastsUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData);
    void sendDvaMessagesUpdate(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData);
    void sendDvaMsgVersionUpdate(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData);

    ////////////////////////////////////////////////////////
    //
    //            Alarm message senders
    //
    ////////////////////////////////////////////////////////


    void raiseInvalidPAConfigurationAlarm( const std::string& detail );
    void closeInvalidPAConfigurationAlarm( );
	void raisePASCommsFailureAlarm();

    void closePASCommsFailureAlarm( );
    void raisePASVersionConflictAlarm( const std::string& stationInConflict );
    void closePASVersionConflictAlarm( );

    void raiseTrainLibraryVersionAlarm( int trainID, int libraryVersion, int iscsLibraryVersion );
    void closeTrainLibraryVersionAlarms();
    void closeTrainLibraryVersionAlarm(int trainID);
    void closeTrainLibraryVersionAlarmOnVersionChange(int newIscsVersion);


    ////////////////////////////////////////////////////////
    //
    //            Miscellaneous Utilities
    //
    ////////////////////////////////////////////////////////

    void tokenise(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

    // formats the time to the display format the same as the event viewer
    std::string formatTime(time_t time);
    std::string formatTimeToHourMinute(time_t time);
    std::string formatTimeToMinute(time_t time);

    // formats the zone list into a string
    // @exception PaErrorException for an invalid zone
    std::string formatZones(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
	std::string formatZone(int zoneID);

	std::string formatReasons(const std::map<int, std::string>& reasons, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);//TD10320

    // formats the train start and stop times
    std::string formatInterval( const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams );

    void sendBroadcastProgressUpdate( AbstractBroadcast& broadcast, bool bNotifyClient );

    bool isAdHocDvaMsg( const unsigned short usMsgType );

    bool isLiveBroadcast(const unsigned short usZoneState);

protected:

    void submitAuditMessage( const TA_Base_Core::MessageType& messageType,
        TA_Base_Core::DescriptionParameters& descriptionParameters, const std::string& sessionId = "" );

    static TA_Base_Core::AuditMessageSender*         m_auditSender;
    static TA_Base_Core::StateUpdateMessageSender*	m_stateUpdateMessageSender;
    static TA_Base_Core::CommsMessageSender*         m_paAgentCommsSender;
    static CachedConfig*                       m_cachedConfigInstance;
    static TA_Base_Core::NonReEntrantThreadLockable  m_lock;
    static PasHelpers*                          m_me;

	// jeffrey++ TD10393
	std::string PasHelpers::convertTypeString( unsigned short InType );
	std::string PasHelpers::checkLabel( const std::string& InLabel);
	// ++jeffrey TD10393

    struct TrainAlarm
    {
        std::string alarmId;
        int trainLibraryVersion;
    };

    typedef std::map<int, TrainAlarm> TrainIdToAlarmDetailsMap;
    TrainIdToAlarmDetailsMap    m_trainToAlarmIdMap;
//TD18607 marc_bugfix
	std::string m_pasAlarm;
	
//TD18607 marc_bugfix
};

}

#endif // !defined(PASHELPERS_INCLUDED)

