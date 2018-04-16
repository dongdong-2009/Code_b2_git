#if !defined(IECSStationModesView_6DBC251E_E6D5_4805_9896_1E43CC700518__INCLUDED_)
#define IECSStationModesView_6DBC251E_E6D5_4805_9896_1E43CC700518__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/IECSStationModesView.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This interface provides a contract between the ECS Station Modes user interface class 
  * and the business class that provides all the logic and data for it.
  *
  */

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

namespace TA_IRS_App
{
	class IBaseView
	{//TD18095, jianghp, to fix the performance of showing manager application
	public:
		virtual void init() = 0;
		virtual ~IBaseView() {};
	};

    class IECSStationModesView : public IBaseView
    {
    public:
        /** 
          * setLocations
          *
          * Populate user interface with list of location (Station) names.
          *
          * @param locationNames - Vector of location (station) names in inbound order.
          */
        virtual void setLocations( const TA_ECSManagerCommonDefs::LocationNames& locations ) = 0;

    
        /** 
          * setSelectedLocationIndex
          *
          * Set location selection control specific location.
          *
          * @param selectedLocationIndex - index of location to set as selected (index relative
          *                                to container of locations passed from setLocations)
          */
        virtual void setSelectedLocationIndex( unsigned int selectedLocationIndex ) = 0;


        /** 
          * enableLocationSelectionControl
          *
          * Enable/disable location selection control.
          *
          * @param doEnableLocationSelectionControl - true if should enable, false otherwise.
          */
        virtual void enableLocationSelectionControl( bool doEnableLocationSelectionControl ) = 0;

        
        /** 
          * enableApplyStationModeControl
          *
          * Enable/disable apply Station Mode control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyStationModeControl( bool doEnableApplyStationModeControl ) = 0;


        /** 
          * setStationModeDetails
          *
          * Populate user interface from vector of StationModeDetails.
          *
          * @param stationModeDetails - Vector of stationodeDetails ordered by mode.
          */
        virtual void setStationModeDetails( const TA_ECSManagerCommonDefs::StationModeDetails& stationModeDetails ) = 0;

		virtual void setCurrentStationVisualLock(bool isStationLocked, unsigned int index) = 0;


        /** 
          * setCurrentStationMode
          *
          * Populate user interface with current Station Mode and type for 
          * currently selected location.
          *
          * @param stationMode     - Current station mode value as string.
          * @param stationModeType - ECurrentStationModeType indicating type of current station mode.
          */
        virtual void setCurrentStationMode( const std::string& stationMode, TA_ECSManagerCommonDefs::ECurrentStationModeType currentStationModeType ) = 0;


        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode for currently selected location as being
          * in progress, or not in progress.
          *
          * @param doShowAsInProgress - true to show as in progress, false otherwise. 
          */
        virtual void setCurrentStationModeInProgress( bool doShowAsInProgress ) = 0;

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
          * setStationModeProposalState
          *
          * Update display of Station Mode for currently selected location with new
          * proposal state.
          *
          * @param stationModeIndex         - Index of Station Mode the update is for (index relative
          *                                   to container of Station Modes passed from setStationModeDetails).
          * @param stationModeProposalState - Enum corresponding to updated proposal state.
          *
          */
        virtual void setStationModeProposalState( unsigned int stationModeIndex, TA_ECSManagerCommonDefs::EModeProposalState stationModeProposalState ) = 0;

		virtual void setInboundStationLock(bool isStationLocked) = 0;

		virtual void setOutboundStationLock(bool isStationLocked) = 0;

		virtual void setColourMap(unsigned int index, COLORREF colour) = 0;


        /** 
          * setInboundLocation
          *
          * Set display of location adjacently inbound to currently selected location (Station).
          *
          * @param inboundLocation - Name of inbound location.
          */
        virtual void setInboundLocation( const std::string& inboundLocation ) = 0;


        /** 
          * setOutboundLocation
          *
          * Set display of location adjacently outbound to currently selected location (Station).
          *
          * @param outboundLocation - Name of outbound location.
          */
        virtual void setOutboundLocation( const std::string& outboundLocation ) = 0;

        
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
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication() = 0;


		/** 
          * addStatusLine
          *
          * Adds some text to the status text box.
          *
          * @param - Text which indicates current status of various ECS Modes at the current location.
          */
		virtual void addStatusLine(const std::string& statusText) = 0;


		virtual void setCurrentAppliedMode(int index, bool isOverriding) = 0;

        /** 
          * ~IECSStationModesView
          *
          * description
          *
          */
        virtual ~IECSStationModesView() {};
    };
} // namespace TA_IRS_App

#endif // !defined(IECSStationModesView_6DBC251E_E6D5_4805_9896_1E43CC700518__INCLUDED_)
