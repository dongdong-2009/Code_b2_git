/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PasHelpers.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/22 11:16:25 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PASHELPERS_INCLUDED)
#define PASHELPERS_INCLUDED

// liuyu++ TD4657
#include "app/pa/PAAgent/src/CachedConfig.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/types/src/ta_types.h"

#include <map>


namespace TA_Base_Core 
{ 
    class AuditMessageSender; 
    class StateUpdateMessageSender; 
    class CommsMessageSender;
    // liuyu++ TD4657
    class NonUniqueAlarmHelper;
    // ++liuyu TD4657
	class AlarmHelper;
}



#if defined(WIN32) && defined(_CONSOLE) && defined(_DEBUG)
    #define LOGOUT(x)                   \
    {                                   \
        std::cout << x << std::endl;    \
    }                                   
#else
    #define LOGOUT(x)
#endif

namespace TA_IRS_App
{
    class AbstractBroadcast;
}

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

    void PAZoneKeysToCoverage(const Keys& keys, TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage);

    void coverageToPAZoneKeys(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, Keys& keys );

    void keysToLocalCoverageBitmap(const Keys& keys, ta_uint32& localCoverageBitmap);

    void keysToGlobalCoverageBitmaps(const Keys& keys, ta_uint32* globalCoverageBitmaps);

    void localCoverageBitmapToKeys(const ta_uint32& localCoverageBitmap, Keys& keys, unsigned long locationKey);

    void globalCoverageBitmapsToKeys(const ta_uint32* globalCoverageBitmaps, Keys& keys);

    void coverageToLocalCoverageBitmap(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, ta_uint32& localCoverageBitmap );

    void localCoverageBitmapToCoverage(const ta_uint32& localCoverageBitmap, TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, unsigned long locationKey );

    void coverageToGlobalCoverageBitmaps(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, ta_uint32* globalCoverageBitmaps );

    void globalCoverageBitmapsToCoverage(const ta_uint32* globalCoverageBitmaps, TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage);

    bool zoneIsAPagingConsole(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
    bool zoneIsAPagingConsole(unsigned long zone);
	bool zoneIsAPagingConsole(unsigned long sourceId, const std::string& session);	// TD19669


    // Note can return -1 if messageKey is not found
    int getAdhocOffset(unsigned long messageKey);



    void hexDumpDebug( const char* prefix, const ta_uint8* buffer, int length );



    void setBit(ta_uint8* buffer, unsigned int bitOffset, bool value);

    bool getBit(ta_uint8* buffer, unsigned int bitOffset);

    void set8bit(ta_uint8* buffer, unsigned long offset, ta_uint8 value);

    // Host to Big endian / Network Byte order conversion
    //
    // 0x1234 ->   0x12      0x34
    //           [offset] [offset+1]
    //
    void set16bit(ta_uint8* buffer, unsigned long offset ,ta_uint16 value);

    // Host to Big endian / Network Byte order conversion
    //
    // 0x12345678 ->   0x12      0x34       0x56       0x78
    //               [offset] [offset+1] [offset+2] [offset+3]
    //
    void set32bit(ta_uint8* buffer, unsigned long offset ,ta_uint32 value);
 
    ta_uint8 get8bit(ta_uint8* buffer, unsigned long offset);

    // Big endian / Network Byte order to Host conversion
    //
    //    0x12      0x34     -> 0x1234
    //  [offset] [offset+1]
    //
    ta_uint16 get16bit(ta_uint8* buffer, unsigned long offset);

    // Big endian / Network Byte order to Host conversion
    //
    //    0x12      0x34       0x56       0x78    -> 0x12345678
    //  [offset] [offset+1] [offset+2] [offset+3]
    //
    ta_uint32 get32bit(ta_uint8* buffer, unsigned long offset);

    void throwBroadcastIdNotFoundException(const std::string& what, const std::string& broadcastId=""); //throw(TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdNotFound);
    void throwFepcCommsFailureException(const std::string& what, const std::string& broadcastId=""); //throw(TA_Base_Bus::IPAAgentCorbaDef::FepcCommsFailure);
    void throwBroadcastLimitReachedException(const std::string& broadcastId=""); //throw(TA_Base_Bus::IPAAgentCorbaDef::BroadcastLimitReached);
    void throwTrainAgentCommsFailureException(const std::string& what = "Failure to communicate with local Train Agent", const std::string& broadcastId="");//throw(TA_Base_Bus::IPAAgentCorbaDef::TrainAgentCommsFailure);
    void throwAccessDeniedException(const std::string& reason = "Action is not permitted", const std::string& broadcastId="");//throw(TA_Base_Core::AccessDeniedException);

/*
    void throwBadParameterException(const std::string& what, const std::string& broadcastId=""); //throw(TA_Base_Core::BadParameterException);
    void throwDatabaseErrorException(const std::string& reason = "Failure to access the database", const std::string& broadcastId=""); //throw(TA_Base_Core::DatabaseErrorException);
    void throwTisAgentCommsFailureException(const std::string& what = "Failure to communicate with local TIS Agent", const std::string& broadcastId="") ;
        //throw(TA_Base_Bus::IPAAgentCorbaDef::TisAgentCommsFailure);

*/
    void handleCachedMappingNotFoundException(const TA_Base_Core::CachedMappingNotFoundException& e, const std::string& description = "");


    ////////////////////////////////////////////////////////
    //
    //            Audit message senders
    //
    ////////////////////////////////////////////////////////

    // @exception BadParameterException if one of the given parameters is invalid
    void submitPaBroadcastStationDvaRequestAuditMessage( const std::string& broadcastId,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                                                         unsigned short dwellInSecs,
                                                         bool hasChime,
                                                         const TA_Base_Core::DateTime& startTime,
                                                         const TA_Base_Core::DateTime& stopTime,
                                                         unsigned short periodInSecs,
                                                         bool isSynchronisedWithTis,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                                                         bool isLocalPlayback,
                                                         const std::string& sessionId );


    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaBroadcastStationLiveRequestAuditMessage( const std::string& broadcastId,
                                                          unsigned short sourceId, 
                                                          const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, 
                                                          const std::string& sessionId );

	void submitPaBroadcastStationMusicRequestAuditMessage( const std::string& broadcastId,
                                                           const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& musicType,
                                                           const std::string& sessionId );

	// @exception BadParameterException if one of the given parameters is invalid
    void submitPaRecordAdhocRequestAuditMessage( const std::string& broadcastId,
                                                 unsigned char sourceId,
                                                 unsigned long messageKey,
                                                 const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaRetryStationBroadcastRequestAuditMessage( const std::string& broadcastId,
                                                           const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType& broadcastType,
                                                           const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, 
                                                           const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage( const std::string& broadcastId,
                                                                       unsigned long messageKey, 
                                                                       const std::string& messageVersionNumber, 
                                                                       bool hasChime, 
                                                                       bool isSynchronisedWithTis, 
                                                                       unsigned long durationInSecs, 
                                                                       unsigned short periodInSecs, 
                                                                       const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                                                                       const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage( const std::string& broadcastId,
                                                                                 unsigned long messageKey,
                                                                                 const std::string& messageVersionNumber,
                                                                                 bool hasChime,
                                                                                 bool isSynchronisedWithTis,
                                                                                 unsigned long durationInSecs,
                                                                                 unsigned short periodInSecs,
                                                                                 unsigned long stationKey,
                                                                                 const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage( const std::string& broadcastId,
                                                                              unsigned long messageKey,
                                                                              const std::string& messageVersionNumber,
                                                                              bool hasChime,
                                                                              bool isSynchronisedWithTis,
                                                                              unsigned long durationInSecs,
                                                                              unsigned short periodInSecs,
                                                                              const std::string& zoneArea,
                                                                              const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage( const std::string& broadcastId,
                                                                               unsigned long messageKey,
                                                                               const std::string& messageVersionNumber,
                                                                               bool hasChime,
                                                                               bool isSynchronisedWithTis,
                                                                               unsigned long durationInSecs,
                                                                               unsigned short periodInSecs,
                                                                               const std::string& zoneGroupLabel,
                                                                               const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaStationBroadcastResultAuditMessage( const std::string& broadcastId,
                                                     const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& failedZones,
                                                     bool broadcastFailed,
													 std::map<int, std::string> failedReasons,//TD10320
                                                     const std::string& sessionId = "" );

    void submitPaTrainBroadcastResultAuditMessage( const std::string& broadcastId,
                                                   const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& failedTrains,
                                                   bool broadcastFailed,
                                                   const std::string& sessionId = "" );

	void submitPaTerminateBroadcastAuditMessage( const std::string& broadcastId,
                                                 const std::string& sessionId );

	void submitPaRemoveBroadcastAuditMessage( const std::string& broadcastId,
                                              const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaSetAdhocTypeAuditMessage( char oldType,
                                           char newType,
                                           unsigned long messageKey,
                                           const std::string& sessionId );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaSetAdhocLabelAuditMessage( const std::string& oldLabel,
                                            const std::string& newLabel,
                                            unsigned long messageKey,
                                            const std::string& sessionId );

	void submitPaChangeBroadcastIdAuditMessage( const std::string& oldBroadcastId,
                                                const std::string& newBroadcastId,
                                                const std::string& sessionId );

	void submitPaAgentBroadcastStateAuditMessage( const std::string& broadcastId,
                                                  const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState& broadcastState );

	void submitPaAgentBroadcastStateFailureAuditMessage( const std::string& broadcastId,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::EGlobalState& globalState );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitPaAgentDVAMessageUpdateAuditMessage( char oldType,
                                                    char newType,
                                                    const std::string& oldLabel,
                                                    const std::string& newLabel,
                                                    unsigned long messageKey );

    // @exception BadParameterException if one of the given parameters is invalid
	void submitTrainPAPrerecordedAuditMessage( const std::string& broadcastId,
                                               const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                               unsigned long messageKey, 
                                               unsigned short libraryVersion, 
                                               const TA_Base_Bus::ITrainPaCorba::MessageSchedule& announcementPeriod,
                                               const std::string& sessionId );

	void submitTrainPACyclicAuditMessage( const std::string& broadcastId,
                                          const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                          unsigned long messageKey, 
                                          unsigned short libraryVersion, 
                                          const TA_Base_Bus::ITrainPaCorba::MessageSchedule& announcementPeriod,
                                          const std::string& sessionId );

	void submitTrainPASetupLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                         const std::string& sessionId );

	void submitTrainPAContinueLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                            unsigned long train );

	void submitTrainPAEndLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                       const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains );

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

	void submitPaAgentDependencyFailureAuditMessage( const std::string& component,
                                                     const std::string& impact );

	void submitPAAgentOnlineUpdateAuditMessage( const std::string& parameter );
    
	// TD15128
	void submitPaPrioritySchemeUpdateAuditMessage();
	// TD15128


    ////////////////////////////////////////////////////////
    //
    //            State update message senders
    //
    ////////////////////////////////////////////////////////


    void sendPAAgentBroadcastParameterUpdate( TA_Base_Bus::IPAAgentCorbaDef::EBroadcastParameter parameter, 
                                              const CORBA::Any& data, const char* broadcastId);
    void sendPAAgentRemovedBroadcastUpdate(const char * updateData);
    void sendPAAgentNewBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& updateData);
    void sendPAAgentRenamedBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData);

    ////////////////////////////////////////////////////////
    //
    //            Comms message senders
    //
    ////////////////////////////////////////////////////////

    void sendCurrentBroadcastsUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData);
    void sendDvaMessagesUpdate(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData);

    ////////////////////////////////////////////////////////
    //
    //            Alarm message senders
    //
    ////////////////////////////////////////////////////////


    void raiseInvalidPAConfigurationAlarm( const std::string& detail );
    void closeInvalidPAConfigurationAlarm( );
 	// TD 15128
    /*void raisePASPrioritySchemeUpdateNonUniqueAlarm( );*/
//TD18607 marc_bugfix



//TD18607
	void raisePASCommsFailureAlarm();
/*
	// TD 15128
    void raisePASCommsFailureAlarm( const std::string& address, const std::string& port );
    // liuyu++ TD4657
    void PasHelpers::closePASCommsFailureAlarm( const std::string& address, const std::string& port );
    // ++liuyu TD4657
*/


    void closePASCommsFailureAlarm( );
    void raisePASVersionConflictAlarm( const std::string& stationInConflict );
    void closePASVersionConflictAlarm( );

    void raiseTrainLibraryVersionAlarm( int trainID,
                                        int libraryVersion,
                                        int iscsLibraryVersion );
    void closeTrainLibraryVersionAlarms();
    void closeTrainLibraryVersionAlarm(int trainID);
    void closeTrainLibraryVersionAlarmOnVersionChange(int newIscsVersion);


    ////////////////////////////////////////////////////////
    //
    //            Miscellaneous Utilities
    //
    ////////////////////////////////////////////////////////

    void tokenise(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ");

    // formats the time to the display format the same as the event viewer
    std::string formatTime(time_t time);

    // formats the zone list into a string
    // @exception BadParameterException for an invalid zone
    std::string formatZones(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);

	//TD19881
	std::string formatZone(int zoneID);
	//TD19881

	std::string formatReasons(const std::map<int, std::string>& reasons, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);//TD10320

    // formats the train start and stop times
    std::string formatInterval(const TA_Base_Bus::ITrainPaCorba::MessageSchedule& interval);

    void sendBroadcastProgressUpdate(TA_IRS_App::AbstractBroadcast* broadcast);

protected:

    void submitAuditMessage( const TA_Base_Core::MessageType& messageType,
                             const TA_Base_Core::DescriptionParameters& descriptionParameters,
                             const std::string& sessionId = "" );

    static TA_Base_Core::AuditMessageSender*         m_auditSender;
    static TA_Base_Core::StateUpdateMessageSender*	m_stateUpdateMessageSender;
    static TA_Base_Core::CommsMessageSender*         m_paAgentCommsSender;
    static CachedConfig*                       m_cachedConfigInstance;
    static TA_Base_Core::NonReEntrantThreadLockable  m_lock;
    static PasHelpers*                          m_me;

	//TD17994, haipeng, TD18093
	//TA_Base_Core::AlarmHelper&         m_alarmHelper;
    // liuyu++ TD4657
    //TA_Base_Core::NonUniqueAlarmHelper&         m_nonUniqueAlarmHelper;
    //TD18093
    // ++liuyu TD4657
	// jeffrey++ TD10393
	std::string PasHelpers::convertTypeString( char InType );
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

#endif // !defined(PASHELPERS_INCLUDED)

