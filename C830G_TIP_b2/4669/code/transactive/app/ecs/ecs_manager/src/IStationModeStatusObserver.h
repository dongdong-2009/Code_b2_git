#if !defined(IStationModeStatusObserver_E031DE47_BE0B_41e5_A7AE_A108337AC00B__INCLUDED_)
#define IStationModeStatusObserver_E031DE47_BE0B_41e5_A7AE_A108337AC00B__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/IStationModeStatusObserver.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Interface definition for objects interested in updates to the value and validity
  * (validity meaning the value is confirmed from its source) of the Station Mode "Status"
  * and "In Progress" values.
  *
  */
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_IRS_App
{
    class IStationModeStatusObserver
    {

    public:

        /** 
          * stationModeStatusUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Mode Status value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Mode Status value.
          */
        virtual void stationModeStatusUpdated( unsigned long locationKey, bool isValid, int value ) = 0;


        /** 
          * stationModeInProgressUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Mode In Progress value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Mode In Progress value.
          */
        virtual void stationModeInProgressUpdated( unsigned long locationKey, bool isValid, int value ) = 0;

		/** 
          * stationVisualLockUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Visual Lock value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Visual Lock value.
          */
		virtual void stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value ) = 0;

		/** 
          * stationAutoModeStatusUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Auto Mode Status value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Auto Mode Status value.
          */
		virtual void stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value ) = 0;

		virtual void stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState ) = 0;

  
        /** 
          * ~IStationModeStatusObserver
          *
          * description
          *
          */
        virtual ~IStationModeStatusObserver() {};
    };
} // namespace TA_IRS_App

#endif // !defined(IStationModeStatusObserver_E031DE47_BE0B_41e5_A7AE_A108337AC00B__INCLUDED_)
