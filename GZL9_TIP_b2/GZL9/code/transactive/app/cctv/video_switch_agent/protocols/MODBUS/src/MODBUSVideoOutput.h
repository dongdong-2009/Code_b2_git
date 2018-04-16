#if !defined(MODBUSVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define MODBUSVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.h $
  * @author:   Rob Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a VideoOutput
  * using the MODBUS protocol. 
  * Inherits from IPDVideoOutput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/src/IPDVideoOutput.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/IStateValidationItem.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MonitorStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
#include "core/data_access_interface/entity_access/src/VideoOutput.h"

// Forward declarations
const static int RL_MONITOR_SEQUENCE_NORMAL    = 12;
const static int RL_MONITOR_CAMERA_NORMAL      = 13;
const static int RL_MONITOR_SEQUENCE_HIGHLIGHT = 120;
const static int RL_MONITOR_CAMERA_HIGHLIGHT   = 130;
namespace TA_Base_Core
{
    class VideoOutput;
}

namespace TA_IRS_App
{
    class MODBUSVideoOutput : public IPDVideoOutput//, public virtual IStateValidationItem
    {
    public:
        /**
          * MODBUSVideoOutput
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::VideoOutput& videoOutputData
          *             The database object relating to this VideoOutput.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        MODBUSVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData );
        
        /**
          * ~MODBUSVideoOutput
          *
          * Standard destructor.
          */

        virtual ~MODBUSVideoOutput() {};
        
        virtual unsigned long getKey();


		int getDisplayOrder();

		unsigned long getConsoleKey();

        std::string getAddress();

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * notifyInputSwitchedToOutput
          *
          * This is called when data is received from the hardware saying that a
          * switch has occured, so the agent should be in Control mode.
          * Sends a message to notify agent clients that an input has
          * been switched to this output.
          *
          * @param      unsigned long videoInputKey
          *             The database (entity) key of the input that has
          *             been switched to this output.
          */

        void notifyInputSwitchedToOutput( unsigned long videoInputKey );

        /**
		  *
		  */
		void notifyOutputShowingAlarmStack( TA_MODBUS::AlarmStack alarmStack);

		/**
          * updateState
          *
          * Updates the state of the VideoOutput.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state
          *             The new state of the VideoOutput.
          */

        virtual void updateState( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state );
        
        ///////////////////////////////////////////////////////////////////////
        //
        // IPDVideoOutput Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getCurrentVideoInputKey
          *
          * Get the database (entity) key of the VideoInput currently switched to this VideoOutput.  
          * 0 indicates no VideoInput is siwtched to this VideoOutput.
          *
          * @return     unsigned long
          *             The database (entity) key of the VideoInput for this VideoOutput device or
          *             0 if there is no associated input.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
          */

        virtual unsigned long getCurrentVideoInputKey();

		/**
          * getVideoOutputState
          *
          * returns the entire state of this video output
          *
          * @return     VideoOutputState objhect asscociated with this videoOutput
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* getVideoOutputState();

        /**
          * requestSwitchOfInput
          *
          * Requests that the hardware switches the named VideoInput to this VideoOutput.
          *
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      bool isDemand = false
          *             If false => If the VideoInput is locked (hardware locking), the 
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. A polite request was made and the VideoInput is switch (hardware) locked.
          *             2. The VideoInput key is not recognised.
		  *				3. There is a communications failure with the hardware.
          */

        virtual void requestSwitchOfInput( unsigned long videoInputEntityKey, 
                                           bool isDemand );

	    virtual void requestSwitchOfInputs(unsigned long topRightVideoInputEntityKey,
			unsigned long topLeftVideoInputEntityKey,
			unsigned long botRightVideoInputEntityKey,
			unsigned long botLeftVideoInputEntityKey,
			unsigned long consoleKey,
			bool isDemand);

		virtual void connectSequence( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::UserDefinedSequenceConfig& newConfig);
		virtual void disconnectSequence();
		/**
		  * cycleToNextVideoInput
		  *
		  * Manually cycles sequence playback to next VideoInput.
		  *
		  */

		virtual void cycleToNextVideoInput();

		/**
		  * cycleToPreviousVideoInput
		  *
		  * Manually cycles sequence playback to previous VideoInput.
		  */

		virtual void cycleToPreviousVideoInput();

		/**
		  * pauses
		  *
		  * Pauses playback of sequence.
		  */

		virtual void pause();

		/**
		  * play
		  *
		  * Begin/Resume Playback of sequence.
		  *
		  */

		virtual void play();

		/**
		  * stop
		  *
		  * Stop playback of sequence (can be same behaviour as pause depending on hardware).
		  *
		  */

		virtual void stop();

		bool isInAlarmMode();

//		bool isBVSStageInAlarmStack(unsigned long bvsEntityKey);

        ///////////////////////////////////////////////////////////////////////
        //
        // IStateValidationItem Methods
        //
        ///////////////////////////////////////////////////////////////////////


        /** 
          * validateState
          *
          * Callback for StateValidationCycle to let this item know its time
          * to validate its state.
          */
        virtual void validateState();
		/*virtual bool OutputCameraPan(::CORBA::ULong cameraKey,char movement, char speed) ;
		virtual bool OutputCameraTilt(::CORBA::ULong cameraKey,char movement, char speed);
		virtual bool OutputCameraZoom(::CORBA::ULong cameraKey,char movement) ;
		virtual bool OutputCameraPTZ(::CORBA::ULong cameraKey, const ::TA_Base_Bus_GZL9::VideoOutputCorbaDef::CameraPTZ& cmd) ;*/
		virtual void activeVideoInput(::CORBA::ULong videoInputEntityKey, ::CORBA::Boolean active, ::CORBA::Boolean isDemand) ;
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        MODBUSVideoOutput( const MODBUSVideoOutput& theMODBUSVideoOutput );
        MODBUSVideoOutput& operator=( const MODBUSVideoOutput& );
		TA_MODBUS::AlarmStack alarmStateSequenceToAlarmStack(TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack alarmStack);
		TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack alarmStateSequenceFromAlarmStack(TA_MODBUS::AlarmStack alarmStack);
	
		void requestSingleCamToOutput(unsigned long camKey);
		// assign sequence to monitor and active
		void requestSequenceToOutput(unsigned long seqKey);
		void deactiveSequence(unsigned long seqKey);
		void requestQuadToOutput(unsigned long quadKey);

		void requestCamsToQuad(unsigned long topRightCamKey, unsigned long topLeftCamKey, unsigned long botRightCamKey, unsigned long botLeftCamKey);
		void requestSeqsToQuad(unsigned long topRightSeqKey, unsigned long topLeftSeqKey, unsigned long botRightSeqKey, unsigned long botLeftSeqKey);

		void stopMonitor();
        //
        // The configuration data object for this VideoOutput.
        //

        TA_Base_Core::VideoOutputPtr m_videoOutputData;
		TA_MODBUS::AlarmStack	m_alarmStack;
		bool		m_isInAlarmMode;
		
		//std::map<TA_MODBUS::AlarmState, unsigned long>	m_alarmStateToId;
		//std::map<unsigned long, TA_MODBUS::AlarmState>	m_idToAlarmState;
		
        //
        // The database (entity) key of the current VideoInput.
        //
        unsigned long m_currentVideoInputKey;

    }; // MODBUSVideoOutput

} // TA_IRS_App
    
#endif // !defined(MODBUSVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
