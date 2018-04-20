#if !defined(AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/4669/transactive/app/ecs/ecs_manager/src/ECSManagerCommonDefs.h $
  * @author:  Robert Young
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2011/05/26 16:20:00 $
  * Last modified by:  $Author: qi.huang $
  *
  * This holds all types and constants used by multiple ECSManager classes. 
  * They have been put in a special namespace (TA_ECSManagerCommonDefs) so 
  * as not too pollute the TA_IRS_App namespace.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//  -Prasanna- Prevent Warning C4786 being generated.
#pragma warning(disable: 4786) 
//  -Prasanna- 

#include <vector>
#include <string>

namespace TA_IRS_App
{
    const unsigned int WM_READ_PARAMETERS = WM_USER + 1;
    const unsigned int WM_CHECK_PERMISSIONS = WM_USER + 2;
    const unsigned int WM_SESSION_CHANGE = WM_USER + 3;
    const unsigned int WM_ENTITY_UPDATE_MESSAGE = WM_USER + 4;

    //
    // Typedefs to be used by client classes as well as this class.
    //
    struct OrderedLocation
    {
        unsigned long key;
        std::string name;
    };
    typedef std::vector< OrderedLocation >  OrderedLocations;
    typedef std::vector< unsigned long >    MasterModeKeys;
    typedef std::vector< unsigned long >    StationModeKeys;

    //
    // Some typedefs used by multiple ECSManager classes.
    //
    typedef std::vector< unsigned long >        LocationKeys;
    typedef std::vector< std::string >          LocationNames;
    typedef std::vector< unsigned long >        ZoneDirectionKeys;
    struct StationModeDetail
    {
        unsigned long   stationMode;
        std::string     stationModeAsString;
        std::string     operation;
        std::string     track;
        std::string     condition;
        unsigned long   locationKey;
    };

    typedef std::vector< StationModeDetail >    StationModeDetails;
    typedef std::vector< unsigned long >        StationModes;
    typedef std::vector< std::string >          StationModesAsStrings;

    struct MasterModeDetail
    {
        unsigned long           masterMode;
        std::string             masterModeAsString;
        std::string             condition;
        unsigned long           zoneID;
        std::string             zoneTrack;
        std::string             zoneLocation;
        std::string             evacuationDirection;
        std::string             airflowDirection;
		std::string             situation;
        StationModes            associatedStationModes;
        StationModesAsStrings   associatedStationModesAsStrings;
        unsigned long           zoneDirectionKey;
        std::string             displayDirection;
		std::string				verificationMessage;
    };
    typedef std::vector< MasterModeDetail >     MasterModeDetails;

    enum EModeProposalState
    {
        NOT_CURRENTLY_PROPOSED = 0,
        CURRENTLY_PROPOSED = 1,
        CURRENTLY_OVERRIDING_PROPOSED = 2
    };
	
	// IMPORTANT! This must be updated accordingly if a new colour is added or
	// removed for ECurrentStationModeType.
	const unsigned int STATION_COLOUR_NUM = 7;

    enum ECurrentStationModeType
    {
        CURRENT_STATION_MODE_NORMAL = 0,
        CURRENT_STATION_MODE_CONGESTION = 1,
        CURRENT_STATION_MODE_EMERGENCY = 2,
        CURRENT_STATION_MODE_FAILED = 3,
		CURRENTLY_PROPOSED_COLORREF = 4,
		CURRENTLY_OVERRIDING_PROPOSED_COLORREF = 5,
		CURRENT_STATION_LOCK = 6
    };
    // Colours for displaying current Station Modes Type above.
	
    enum EProposedStationModeType
    {
        PROPOSED_FOR_NORMAL = 0,
        PROPOSED_FOR_CONGESTION = 1,
        PROPOSED_FOR_EMERGENCY = 2,
        PROPOSED_IS_UNKNOWN = 3
    };

    typedef std::pair< int, TA_IRS_App::EProposedStationModeType > LocationIndexAndProposedType;

    typedef enum ECurrentStationAutoModeType
    {
        CURRENT_STATION_AUTO_MODE_OFF = 0,
        CURRENT_STATION_AUTO_MODE_ON = 1,
        CURRENT_STATION_AUTO_MODE_FAILED = 2
    };


    const char* const DIALOG_TYPE_RPARAM = "DialogType";
    const char* const ZONE_EQUIPMENT_RPARAM = "ZoneEquipment";

    const std::string EMERGENCY_COMPARE_STRING =    "Emergency";
    const std::string CONGESTION_COMPARE_STRING =   "Congested";

    const unsigned int FLASH_TIMER_ID = 2;
    const unsigned int FLASH_INTERVAL = 800; 

	const unsigned int STATION_MODE_NORMAL_UPPERBOUND = 10;
	const unsigned int STATION_MODE_CONGESTION_UPPERBOUND = 29;
	const unsigned int STATION_MODE_EMERGENCY_UPPERBOUND = 100;

} // namespace TA_Bus


#endif // !defined(AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
