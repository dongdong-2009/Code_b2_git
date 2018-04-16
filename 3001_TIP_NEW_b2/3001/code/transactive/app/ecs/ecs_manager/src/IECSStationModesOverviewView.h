#if !defined(IECSSTATIONMODESOVERVIEWVIEW_705A9831_75F4_4550_8668_3D5FD3882E2C__INCLUDED_)
#define IECSSTATIONMODESOVERVIEWVIEW_705A9831_75F4_4550_8668_3D5FD3882E2C__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/IECSStationModesOverviewView.h $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/06 09:00:06 $
  * Last modified by:  $Author: qi.huang $
  *
  * This interface provides a contract between the ECS Station Modes Overview user interface class 
  * and the business class that provides all the logic and data for it.
  *
  */


#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
//TD18095, jianghp, to fix the performance of showing manager application
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "bus/scada/common_library/src/CommonDefs.h"


namespace TA_IRS_App
{

    class IECSStationModesOverviewView: public IBaseView
    {

    public:

        /** 
          * setLocations
          *
          * Populate user interface with list of location (Station) names.
          *
          * @param locationNames - Vector of location (station) names in inbound order.
          */
        virtual void setLocations( const LocationNames& locationNames ) = 0;


        /** 
          * displayErrorMessage
          *
          * Display error message to user.
          *
          * @param errorMessage - Error message to display.
          */
        virtual void displayErrorMessage( const std::string& errorMessage ) = 0;


        /** 
          * displayWarningMessage
          *
          * Display error message to user.
          *
          * @param warningMessage - Warning message to display.
          */
        virtual void displayWarningMessage( const std::string& warningMessage ) = 0;


        /** 
          * aquireConfirmation
          *
          * Acquire confirmation from user.
          *
          * @param confirmationMessage - Description of action requiring confirmation.
          *
          * @return true if confirmed, false otherwise.
          */
        virtual bool aquireConfirmation( const std::string& confirmationMessage ) = 0;


        /** 
          * aquireUserDecision
          *
          * Present user with number of options and return user's decision.
          *
          * @param decisionMessage - Message to display along with choices.
          * @param choices         - Container of choices to present user.
          *
          * @return int corresponding to user's choice (-1 used to indicate no choice)
          */
        virtual int aquireUserDecision( const std::string& decisionMessage, const std::vector< std::string >& choices ) = 0;


        /** 
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication() = 0;


        /** 
          * enableApplyStationModeControl
          *
          * Enable/disable apply Station Mode control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyStationModeControl( bool doEnableApplyStationModeControl ) = 0;


        /** 
          * enableApplyAllCongestedStationModesControl
          *
          * Enable/disable apply all Station Modes for congestion control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyAllCongestedStationModesControl( bool doEnableApplyAllCongestedStationModesControl ) = 0;


        /** 
          * setCurrentStationMode
          *
          * Populate user interface with current Station Mode and type for 
          * particular location.
          *
          * @param locationIndex   - Index of location Station Mode is for (index relative to
          *                          vector of locations passed using setLocations).
          * @param stationMode     - Current station mode value as string.
          * @param stationModeType - ECurrentStationModeType indicating type of current station mode.
          */
        virtual void setCurrentStationMode( int locationIndex, const std::string& stationMode, ECurrentStationModeType stationModeType ) = 0;


        virtual void setCurrentStationAutoMode( int locationIndex, bool autoModeStatus ) = 0;

		virtual void setCurrentStationAutoModeControlState( int locationIndex, TA_Base_Bus::EDataPointControlState controlState) = 0;
    

        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode (for particular location) as being currently
          * in progress, or not in progress.
          *
          * @param locationIndex      - Index of location the "in progress" setting is for (index relative to
          *                             vector of locations passed using setLocations).
          * @param doShowAsInProgress - true to show as in progress, false otherwise. 
          */
        virtual void setCurrentStationModeInProgress( int locationIndex, bool doShowAsInProgress ) = 0;

		virtual void setCurrentStationVisualLock(int locationIndex, bool isStationLocked) = 0;


        /** 
          * setCurrentProposedStationMode
          *
          * Update user interface with new proposed Station Mode (or clear existing one).
          *
          * @param locationIndex           - Index of location the "in progress" setting is for (index relative to
          *                                  vector of locations passed using setLocations).
          * @param proposedStationModeType - Enum corresponding to type of proposed Station Mode.
          * @param proposedStationMode     - String value of proposed Station Mode.
          * @param proposalState           - Enum corresponding to proposal State.
          *
          */
        virtual void setCurrentProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType, 
            const std::string& proposedStationMode, EModeProposalState proposalState ) = 0;

		/** 
          * addStatusLine
          *
          * Adds some text to the status text box.
          *
          * @param - Text which indicates current status of various ECS Modes at the current location.
          */
		virtual void addStatusLine(const std::string& statusText) = 0;
    
        /** 
          * ~IECSStationModesOverviewView
          *
          * description
          *
          */
        virtual ~IECSStationModesOverviewView() {};
    };
} // namespace TA_IRS_App

#endif // !defined(IECSSTATIONMODESOVERVIEWVIEW_705A9831_75F4_4550_8668_3D5FD3882E2C__INCLUDED_)
