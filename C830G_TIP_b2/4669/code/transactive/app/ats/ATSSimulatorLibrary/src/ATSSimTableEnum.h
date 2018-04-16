#ifndef ATS_SIM_TABLE_ENUMERATIONS_H
#define ATS_SIM_TABLE_ENUMERATIONS_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableEnum.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for enumerated typedefs common to ATS Tables. Some other
  * enumerations are in ATSSimCommon.h, namely the TableIdentifier,
  * LocationType and TableOutputFormat
  */
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

namespace TA_IRS_App
{
namespace ATS_Sim
{
#define Unknown 0

    ///
    /// Enumeration to represent the possible values of the Validity field on
    /// Ats tables
    typedef enum DataValidity
    {
        Irrelevant  = 0,
        Relevant    = 1
    } DataValidity;

    ///
    /// Enumeration to represent the possible values of the head cab direction
    /// Ats tables
    typedef enum HeadCabDirection
    {
        DownDirection   = 1,
        UpDirection     = 2
    } HeadCabDirection;

    ///
    ///
    typedef enum TalkativeStatus
    {
        Mute        = 0,
        Talkative   = 1
    } TalkativeStatus;

    ///
    ///
    typedef enum HoldStatus
    {
        NotHeld     = 0,
        Held        = 1
    } HoldStatus;

    ///
    ///
    typedef enum TrainReadinessMode
    {
        Shutdown                            = 0,
        Asleep                              = 1,
        Initialisation                      = 2,
        Prepared                            = 3,
        AM_Normal_Wake_Up                   = 4,
        AM_Normal_Standby                   = 5,
        AM_Normal_Depot_Movement            = 6,
        AM_Normal_Mainline_Service          = 7,
        AM_Normal_Mainline_Off_Service      = 8,
        AM_Creep                            = 9,
        Manual_Mode_CM                      = 10,
        Manual_Mode_RM_Forward              = 11,
        Manual_Mode_RM_Reverse              = 12,
        Manual_Mode_Safety_Isolated_Mode    = 13,
        Maintenance                         = 14
    } TrainReadinessMode;

    ///
    ///
    typedef enum StalledStatus
    {
        NotStalled  = 0,
        Stalled     = 1
    } StalledStatus;

    ///
    ///
    typedef enum EvacuationStatus
    {
        NoEvacuation    = 0,
        Evacuation      = 1
    } EvacuationStatus;

    ///
    ///
    typedef enum EBStatus
    {
        EBNotActivated  = 0,
        EBActivated     = 1
    } EBStatus;

    ///
    ///
    typedef enum TetraMobileStatus
    {
        AllNormal           = 1,
        AtLeastOneFailure   = 2,
        AllFailed           = 3
    } TetraMobileStatus;

    ///
    ///
    typedef enum PECUStatus
    {
        PECUNormal          = 1,
        PECUActivated       = 2,
        PECUSelected        = 3
    } PECUStatus;

    ///
    ///
    typedef enum CCTVCameraStatus
    {
        CCTVCameraNormal      = 1,
        CCTVCameraFailure     = 2
    } CCTVCameraStatus;

    ///
    ///
    typedef enum CCTVEquipmentStatus
    {
        CCTVEquipmentNormal      = 1,
        CCTVEquipmentMinorFault  = 2,
        CCTVEquipmentMajorFault  = 3
    } CCTVEquipmentStatus;

    ///
    ///
    typedef enum TrainPAStatus
    {
        TPANotActivated        = 1,
        TPAActivated           = 2,
        TPANotFullyOperational = 3,
        TPANotOperational      = 4
    } TrainPAStatus;

    ///
    ///
    typedef enum FireDetectionStatus
    {
        NoFireDetected          = 1,
        FireDetectionFailure    = 2,
        FireDetected            = 3
    } FireDetectionStatus;

    ///
    ///
    typedef enum InteriorSmokeDetectionStatus
    {
        NoPreAlarmDetected      = 1,
        MinorFaultDetected      = 2,
        PreAlarmDetected        = 3,
        AlarmDetected           = 4
    } InteriorSmokeDetectionStatus;

    ///
    ///
    typedef enum ExteriorSmokeDetectionStatus
    {
        NoExteriorSmokeDetected = 1,
        SmokeDetectionFailure   = 2,
        ExteriorSmokeDetected   = 3
    } ExteriorSmokeDetectionStatus;

    ///
    ///
    typedef enum ExtinguisherStatus
    {
        ExtinguisherAllNormal               = 1,
        ExtinguisherAtLeastOneRemoved       = 2
    } ExtinguisherStatus;

    ///
    ///
    typedef enum EHSStatus
    {
        EHSNotActivated            = 1,
        EHSActivated               = 2
    } EHSStatus;

    ///
    ///
    typedef enum DetrainmentDoorStatus
    {
        DDClosedAndLocked       = 1,
        DDCoverOpen             = 2,
        DDEvacuationRequired    = 3,
        DDNotLockedOrClosed     = 4
    } DetrainmentDoorStatus;

    ///
    ///
    typedef enum DriverConsoleCoverStatus
    {
        DCCClosed               = 1,
        DCCOpened               = 2
    } DriverConsoleCoverStatus;

    ///
    ///
    typedef enum TrainDoorStatus
    {
        TrainDoorClosedAndLocked         = 1,
        TrainDoorOpened                  = 2,
        TrainDoorIsolated                = 3,
        TrainDoorPushBack                = 4,
        TrainDoorObstacleDetection       = 5,
        TrainDoorDoorFailureClosed       = 6,
        TrainDoorDoorFailureOpened       = 7
    } TrainDoorStatus;

    ///
    ///
    typedef enum EmergencyHandleCoverStatus
    {
        EHAllCoverClosed          = 1,
        EHAtLeastOneCoverOpened   = 2
    } EmergencyHandleCoverStatus;

    ///
    ///
    typedef enum TTISStatus
    {
        TTISNormal                  = 1,
        TTISAtLeastOnePIDFailed     = 2,
        TTISVMCFailure              = 3
    } TTISStatus;

    /*

    ///
    ///
    typedef enum
    {
        Unknown                 = 0,
        XXXXX                   = 1
    } ;
    */

//     class EnumUtility
//     {
//     }; // class EnumUtility
} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_TABLE_ENUMERATIONS_H
