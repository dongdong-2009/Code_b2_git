#if !defined(IECSMASTERMODESVIEW_157C02AF_4264_4210_80BE_4D57418ED11B__INCLUDED_)
#define IECSMASTERMODESVIEW_157C02AF_4264_4210_80BE_4D57418ED11B__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/IECSMasterModesView.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This interface provides a contract between the ECS Master Modes user interface class and the 
  * business class that provides all the logic and data for it.
  *
  */

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
//TD18095, jianghp, to fix the performance of showing manager application
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"

namespace TA_IRS_App
{
    class IECSMasterModesView: public IBaseView
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
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication() = 0;


        /** 
          * setMasterModeDetails
          *
          * Populate user interface from vector of MasterModeDetails.
          *
          * @param masterModeDetails - Vector of MasterModeDetails ordered by mode.
          */
        virtual void setMasterModeDetails( const MasterModeDetails& masterModeDetails ) = 0;


        /** 
          * enableApplyMasterModeControl
          *
          * Enable or diable user interface control which applies selected MasterMode. 
          *
          * @param doEnableApplyMasterModeControl - true if should enable, 
          *                                         false if should disable.
          */
        virtual void enableApplyMasterModeControl( bool doEnableApplyMasterModeControl ) = 0;


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
          * setCurrentProposedMasterMode
          *
          * Set display of Master Mode according to current proposal state.
          *
          * @param masterModeIndex - Index of Master Mode the proposal state is for (index relative to
          *                          vector of Master Mode Details passed using setMasterModeDetails).
          * @param proposalState   - Current proposal state to set display of MasterMode to.
          */
        virtual void setCurrentProposedMasterMode( int masterModeIndex, EModeProposalState proposalState ) = 0;


		/** 
          * addStatusLine
          *
          * Adds some text to the status text box.
          *
          * @param - Text which indicates current status of various ECS Modes at the current location.
          */
		virtual void addStatusLine(const std::string& statusText) = 0;
        /** 
          * ~IECSMasterModesView
          *
          * description
          *
          */
        virtual ~IECSMasterModesView() {}; 
    };
} // namespace TA_IRS_App

#endif // !defined(IECSMASTERMODESVIEW_157C02AF_4264_4210_80BE_4D57418ED11B__INCLUDED_)
