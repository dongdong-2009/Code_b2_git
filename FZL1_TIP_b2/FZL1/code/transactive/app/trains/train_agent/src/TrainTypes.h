/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainTypes.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#if !defined TRAIN_TYPES_H
#define TRAIN_TYPES_H

#include <string>
#include <vector>

namespace TA_IRS_App
{
	// fwd decl
	class TrainCommand;
	class TrainEvent;

	typedef std::auto_ptr<TrainEvent> TrainEventPtr;

	/// @todo put into TrainAgent namespace ?!?

	typedef unsigned char TrainID;

    typedef std::vector<TrainID> TrainList;

	typedef std::string SessionID;

	typedef std::string TSI;
	
	/// Put enums into namespace to reduce pollution of TA_IRS_App
	namespace TrainTypes
	{
		/// Maps to TA_Base_Core::ILocation, reimplementing to abstract away DAI
		enum TrainAgentLocationType
		{
//	not possible 		LOC_UNKNOWN,
			LOC_OCC,
			LOC_DEPOT,
			LOC_STATION
		};

		/// @note Used as index to the array TrainStatus::TrainInformation::trainTSI so force enum values
		enum MPUState
		{
			MPU_Primary = 0,
			MPU_Secondary = 1,
			MPU_Unknown
		};

		enum Track
		{
			Track_Unknown,
			Track_Inbound,
			Track_Outbound
		};
				
		enum TTISStatus
		{
			TTIS_Unknown,
			TTIS_Library,
			TTIS_Schedule
		};
					
		enum TrainStatus 
		{
			IDLE, 
			CAB_TO_CAB,
			DRIVER_PA
		};
				
		// for internal management of PEC message flow
		enum PECSource { PEC_UNASSIGNED = 0, PEC_DRIVER, PEC_OPERATOR };

		enum AtsValid
		{
			AtsValid_Unknown,
			AtsValid_Invalid,
			AtsValid_Valid
		};

		enum EChangeArea
		{
			ChangeAreaMainline,
			ChangeAreaDepot
		};

		/*******************************************************************
		* This State enum was copied from an "internal" header file:
		* bus\security\duty_agent\duty_agent_state\src\SubsystemState.h
		* Reomve this local State enum when the original becomes available.
		* Train Agent will only be interested in the change from not SS_NORMAL
		* to SS_NORMAL.
		********************************************************************/
		enum DutyState
		{
			SS_NORMAL,
			SS_DELEGATED,
			SS_DEGRADED
		};

	}
	
	struct PECPoint
	{
		unsigned char    car;
		unsigned char    PEC;
		enum TrainTypes::PECSource   source;  // used to control state logic
		unsigned long	 creationTime; // seconds since 1 jan 1970
        bool             validity;      // This is only set if the PEC exists and the train is out of the control area
	};
				
    typedef std::vector<PECPoint> PECPointCollection;
	

	/**** various consts ****/
	
	/**
	* valid values: always, strict
	*/
	static const char * RPARAM_TrainTSIUpdateStrategy = "TrainTSIUpdateStrategy";

	/**** conversion from sds data to internal data - probably should be elsewhere ****/
	
	TA_IRS_App::TrainTypes::MPUState convertSDSDataOrigin(unsigned char origin);
	

	/**** decoupling corba - put elsewhere ****/

	struct ATSTrainMessage
	{
		TrainID				trainID;
		bool                isValid;             /// Is train valid
		bool                isLocalisationValid; /// Is train location info valid
		unsigned long		currentLocation;	 /// Key to current location of the train.
		double				currentCctvZone;	 /// cctv zone id
		bool				upDirection;		 /// direction of train (up track or down)
		std::string			serviceNumber;		 /// Service identifier for the train
		bool				isMute;
		TrainTypes::Track	track;
	};

	struct DutyTrainMessage
	{
        unsigned long           subsystemKey;
		TrainTypes::DutyState	addState;
		TrainTypes::DutyState	remState;

        // not always used, will be 0 if not used
        unsigned long           locationKey;
	};

	typedef std::vector<DutyTrainMessage> DutyTrainMessageVector;

	/**
	 * Types used for synchronisation of operation modes between train agents
	 * TrainOperationMode maps to the similar corba type
	 */
	typedef std::vector<unsigned long> ControlledLocations;

	struct TrainOperationMode
	{
	    bool fallbackMode;
		bool doubleAtsFailure;
		bool groupOffline;
		bool localDuty;

		ControlledLocations controlledLocations;
	};

};  // Namespace TA_IRS_App

// Helpers

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TrainAgentLocationType & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::MPUState & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::Track & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TTISStatus & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TrainStatus & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::PECSource & x);
std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::AtsValid & x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::PECPoint & x);
std::ostream & operator<< (std::ostream & os, const TA_IRS_App::ATSTrainMessage & x);
std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainOperationMode & x);

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainCommand & x);
std::ostream & operator<< (std::ostream & os, TA_IRS_App::TrainEventPtr & x);
std::ostream & operator<< (std::ostream & os, TA_IRS_App::TrainEvent & x);

#endif
