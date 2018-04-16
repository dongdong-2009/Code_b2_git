/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/PIDController.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class captures and performs the following control requests via the run params:
  * 1. Display the current message on a specific PID
  * 2. Turning a PID on/off
  * 3. Lock/unlock a PID.
  * 
  */

#if !defined(PID_CONTROLLER_H)
#define PID_CONTROLLER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/utilities/src/RunParams.h"
#include <vector>

namespace TA_IRS_App
{
    class PIDController : public TA_Base_Core::RunParamUser
    {
        
    public:
        PIDController();


        virtual ~PIDController() {}


        /** 
          * initialise
          *
          * We're pulling all the registration for run params outside the constructor
          * since we don't want this class to listen to the run param changes before
          * the application is fully initialised.
          *
          */
        void initialise();


        /** 
          * onRunParamChange
          *
          * Called for a runparam change. selection and deselection.
          *
          */
	    void onRunParamChange( const std::string& name, const std::string& value );

    private:


        /** 
          * getAndDisplayCurrentMessage
          *
          * This method gets the message that is currently being displayed by the PID from the agent.
          * It then tells the GraphWorX display to display the information.
          *
          * @param station 3 character station ID, e.g 'DBG'
          * @param name The full PID name DBG.TIS.Platform.PDP02
          * @param displayId The the identifier of the graphworx display we want to activate
          *      this is typically taken from the runparams <STN>.<displayIdentifier> identifier
          *      that is sent through in the head of every runparam message
          * @param displayCmd Display command that we need to pass to GraphWorX
          *
          */
        void getCurrentMessage(const std::string& station, const std::string& name, const long displayId, const std::string& displayCmd);


        /** 
          * changeState
          *
          * Initiate a "soft turn-on" or "soft turn-off" for the selected PID
          *
          * @param name The full PID name DBG.TIS.Platform.PDP02
          * @param on True if it is to turn on the PID.  False otherwise.
          *
          */
        void changeState(const std::string& name, const bool on);


        /** 
          * tokenizeString
          *
          * Copied from STISManagerDlg
          *
          * @param theString
          * @param 
          *
          * @return 
          *
          */
		std::vector<std::string> tokenizeString(const std::string& theString, const std::string& separatorList );


        /** 
          * getStationName
          *
          * Gets the station name from the pid name
          *
          * @param fullPidName The full PID name e.g. DBG.TIS.Platform.PDP02
          *
          * @return 
          *
          */
        std::string getStationName(const std::string& fullPidName);

		/** 
          * timeDateString
          *
          * Gets the tme string in the readable format.
          *
          * @param timestamp e.g."20040112152359" It is in the Format "YYYYMMDDHHMMSS" 
          *
          * @return 
          *
          */
		std::string timeDateString(const std::string& timeString);


        static const std::string RPARAM_GETMESSAGE;
        static const std::string RPARAM_STATE;
        static const std::string RPARAM_LOCK;
        static const std::string LOCK_PID;
        static const std::string SWITCH_ON_PID;
		static const std::wstring EMPTY_MESSAGE;
    };

} // end namespace TA_IRS_App

#endif 
