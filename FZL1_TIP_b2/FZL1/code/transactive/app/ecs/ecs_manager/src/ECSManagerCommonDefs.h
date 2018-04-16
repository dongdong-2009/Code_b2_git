#if !defined(AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSManagerCommonDefs.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
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
#include <Windows.h>

#define WM_READ_PARAMETERS	(WM_USER+1)
#define WM_CHECK_PERMISSIONS (WM_USER+2)
#define WM_SESSION_CHANGE (WM_USER+3)	// CL-20847

namespace TA_IRS_App
{
    namespace TA_ECSManagerCommonDefs
    {
        //
        // Some typedefs used by multiple ECSManager classes.
        //
        typedef std::vector< unsigned long >        LocationKeys;
        typedef std::vector< std::string >          LocationNames;
        typedef std::vector< unsigned long >        ZoneDirectionKeys;
        typedef struct
        {
            unsigned long   stationMode;
            std::string     stationModeAsString;
            std::string     operation;
            std::string     track;
            std::string     condition;
            unsigned long   locationKey;
        }                                           StationModeDetail;
        typedef std::vector< StationModeDetail >    StationModeDetails;
        typedef std::vector< unsigned long >        StationModes;
        typedef std::vector< std::string >          StationModesAsStrings;
        typedef struct
        {
            unsigned long           masterMode;
            std::string             masterModeAsString;
            std::string             condition;
            unsigned long           zoneID;
            std::string             zoneTrack;
            std::string             zoneLocation;
            std::string             evacuationDirection;
            std::string             airflowDirection;
            StationModes            associatedStationModes;
            StationModesAsStrings   associatedStationModesAsStrings;
            unsigned long           zoneDirectionKey;
            std::string             displayDirection;
			std::string				verificationMessage;
        }                                           MasterModeDetail;
        typedef std::vector< MasterModeDetail >     MasterModeDetails;
        typedef enum
        {
            NOT_CURRENTLY_PROPOSED = 0,
            CURRENTLY_PROPOSED = 1,
            CURRENTLY_OVERRIDING_PROPOSED = 2
        }                                           EModeProposalState;
		
		// IMPORTANT! This must be updated accordingly if a new colour is added or
		// removed for ECurrentStationModeType.
		const unsigned int STATION_COLOUR_NUM = 7;

        typedef enum
        {
            CURRENT_STATION_MODE_NORMAL = 0,
            CURRENT_STATION_MODE_CONGESTION = 1,
            CURRENT_STATION_MODE_EMERGENCY = 2,
            CURRENT_STATION_MODE_FAILED = 3,
			CURRENTLY_PROPOSED_COLORREF = 4,
			CURRENTLY_OVERRIDING_PROPOSED_COLORREF = 5,
			CURRENT_STATION_LOCK = 6
        }  ECurrentStationModeType;
        // Colours for displaying current Station Modes Type above.
		
        typedef enum
        {
            PROPOSED_FOR_NORMAL = 0,
            PROPOSED_FOR_CONGESTION = 1,
            PROPOSED_FOR_EMERGENCY = 2,
            PROPOSED_IS_UNKNOWN = 3
        }                                           EProposedStationModeType;

        // liuyu++ 10745a
        typedef enum
        {
            CURRENT_STATION_AUTO_MODE_OFF = 0,
            CURRENT_STATION_AUTO_MODE_ON = 1,
            CURRENT_STATION_AUTO_MODE_FAILED = 2
        }  ECurrentStationAutoModeType;
        // liuyu++ 10745a


        const char* const DIALOG_TYPE_RPARAM = "DialogType";
        const char* const ZONE_EQUIPMENT_RPARAM = "ZoneEquipment";

        const std::string EMERGENCY_COMPARE_STRING =    "Emergency";
        const std::string CONGESTION_COMPARE_STRING =   "Congested";

        const unsigned int FLASH_TIMER_ID = 2;
        const unsigned int FLASH_INTERVAL = 800; 

		// marvin++
		const unsigned int STATION_MODE_NORMAL_UPPERBOUND = 10;
		const unsigned int STATION_MODE_CONGESTION_UPPERBOUND = 30;
		// ++marvin


    } // namespace TA_ECSManagerCommonDefs

} // namespace TA_Bus


#endif // !defined(AFX_ECSMANAGERCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
