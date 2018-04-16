#if !defined(PDInterfaceFactory_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_)
#define PDInterfaceFactory_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a factory that creates protocol dependent objects
  * for the Video Switch Agent.  For all functions returning new objects
  * in this class, it is the *CALLER'S* responsibility to clean up the object.
  *
  * This interface MUST be implemented by the protocol library.
  * When implementing a protocol library for the Video Switch Agent,
  * a cpp implementation file of this class should be created within
  * the library.
  *
  * If there are any methods that will return 0 (e.g. you don't have quads, so 
  * you don't implement the IPDQuad interface), you should log a message saying
  * "Returning 0 because the IPDQuad interface is not required.".
  *
  * Design note: The create***Control() methods have an accompanying is***ControlSupported()
  * method.  These methods are used to determine if the protocol INDEPENDENT object of
  * that type should be created.  For example, if superimposed text control is not supported
  * by the protocol, then a SuperimposedTextControl object for the given VideoInput will not
  * be created.  This is different to the Camera, VideoInput, VideoOutput, etc classes, which
  * are required no matter what the protocol supports.
  */

#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoOutput.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/Quad.h"
//#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/entity_access/src/RecordingUnit.h"

namespace TA_IRS_App
{
    class IPDCamera;
    class IPDCommunicationsHandler;
    class IPDFocusIrisControl;
    class IPDMovementControl;
    class IPDSuperimposedTextControl;
    class IPDVideoInput;
    class IPDVideoOutput;
    class IPDWashWipeControl;
	class IPDSequence;
	class IPDQuad;
//	class IPDBVSStage;
    class IPDRecordingUnit;
}

namespace TA_IRS_App
{
	class PDInterfaceFactory
	{
	public:
        /**
          * PDInterfaceFactory
          * 
          * Standard constructor.
          */

		PDInterfaceFactory() {};

        /**
          * ~PDInterfaceFactory
          * 
          * Standard destructor.
          */

		virtual ~PDInterfaceFactory() {};

        /**
          * createCommunicationsHandler
          *
          * Creates a protocol dependent object for the CommunicationsHandler.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDCommunicationsHandler*
          *             The new CommunicationsHandler.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDCommunicationsHandler cannot be created.
          */

        virtual IPDCommunicationsHandler* createCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent Object Creation Methods For VideoInputs
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * createVideoInput
          *
          * Creates a protocol dependent object for VideoInputs.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDVideoInput*
          *             The new VideoInput.
          *
          * @param      TA_Base_Core::VideoInput& videoInputData
          *             The database object representing this VideoInput.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDVideoInput cannot be created.
          */

        virtual IPDVideoInput* createVideoInput( TA_Base_Core::VideoInputPtr videoInputData ) = 0;

        /**
          * isSuperimposedTextControlSupported
          *
          * Checks if the protocol supports superimposed text control.
          *
          * @return     bool
          *             True if createSuperimposedTextControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isSuperimposedTextControlSupported() = 0;

        /**
          * createSuperimposedTextControl
          *
          * Creates a protocol dependent object to control superimposed text.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * Pre-condition:  isSuperimposedTextControlSupported()
          *
          * @return     IPDSuperimposedTextControl*
          *             The new text control.
          *
          * @param      TA_Base_Core::VideoInput& videoInputData
          *             The database object representing this VideoInput.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDSuperimposedTextControl cannot be created.
          */

        virtual IPDSuperimposedTextControl* createSuperimposedTextControl( TA_Base_Core::VideoInputPtr videoInputData ) = 0;


		/**
		  * createSequence
		  *
		  * Creates a protocol dependent object to control sequences
		  * It is the *CALLER'S* responsibility to clean up the returned object.
		  *
          * @return     IPDSequence*
          *             The new sequence.
          *
          * @param      TA_Base_Core::Sequence& sequenceData
          *             The database object representing this Sequence.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDSequence cannot be created.
          */
		virtual IPDSequence* createSequence( TA_Base_Core::SequencePtr sequenceData ) = 0;
		 

		/**
		  * createQuad
		  *
		  * Creates a protocol dependent object to control quads
		  * It is the *CALLER'S* responsibility to clean up the returned object.
		  *
          * @return     IPDQuad*
          *             The new quad.
          *
          * @param      TA_Base_Core::Quad& quadData
          *             The database object representing this Quad.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDQuad cannot be created.
          */
		virtual IPDQuad* createQuad( TA_Base_Core::QuadPtr quadData ) = 0;
		 

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent Object Creation Methods For Cameras
        //
        ///////////////////////////////////////////////////////////////////////
       
        /**
          * createCamera
          *
          * Creates a protocol dependent object for Cameras.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDCamera*
          *             The new Camera or 
          *             0 if the protocol does not support Camera alarms.
          *
          * @param      TA_Base_Core::Camera& cameraData
          *             The database object representing this Camera.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDCamera cannot be created.
          */

        virtual IPDCamera* createCamera( TA_Base_Core::CameraPtr cameraData ) = 0;

        /**
          * isMovementControlSupported
          *
          * Checks if the protocol supports movement control.
          *
          * @return     bool
          *             True if createMovementControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isMovementControlSupported() = 0;

        /**
          * createMovementControl
          *
          * Creates a protocol dependent object to control Camera movement.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * Pre-condition:  isSuperimposedTextControlSupported()
          *
          * @return     IPDMovementControl*
          *             The new movement control.
          *
          * @param      TA_Base_Core::Camera& cameraData
          *             The database object representing this Camera.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDMovementControl cannot be created.
          */

        virtual IPDMovementControl* createMovementControl( TA_Base_Core::CameraPtr cameraData ) = 0;

        /**
          * isFocusIrisControlSupported
          *
          * Checks if the protocol supports focus and/or iris control.
          *
          * @return     bool
          *             True if createFocusIrisControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isFocusIrisControlSupported() = 0;

        /**
          * createFocusIrisControl
          *
          * Creates a protocol dependent object to control Camera focus and iris.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * Pre-condition:  isFocusIrisControlSupported()
          *
          * @return     IPDFocusIrisControl*
          *             The new focus and iris control.
          *
          * @param      TA_Base_Core::Camera& cameraData
          *             The database object representing this Camera.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDFocusIrisControl cannot be created.
          */

        virtual IPDFocusIrisControl* createFocusIrisControl( TA_Base_Core::CameraPtr cameraData ) = 0;
        
        /**
          * isWashWipeControlSupported
          *
          * Checks if the protocol supports wash and/or wipe control.
          *
          * @return     bool
          *             True if createWashWipeControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isWashWipeControlSupported() = 0;

        /**
          * createWashWipeControl
          *
          * Creates a protocol dependent object to control Camera wash and wipe.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * Pre-condition:  isWashWipeControlSupported()
          *
          * @return     IPDWashWipeControl*
          *             The new wash and wipe control.
          *
          * @param      TA_Base_Core::Camera& cameraData
          *             The database object representing this Camera.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDWashWipeControl cannot be created.
          */

        virtual IPDWashWipeControl* createWashWipeControl( TA_Base_Core::CameraPtr cameraData ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent Object Creation Methods For VideoOutputs
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * createVideoOutput
          *
          * Creates a protocol dependent object for VideoOutputs.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDVideoOutput*
          *             The new VideoOutput.
          *
          * @param      TA_Base_Core::VideoOutput& videoOutputData
          *             The database object representing this VideoOutput.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDVideoOutput cannot be created.
          */

        virtual IPDVideoOutput* createVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent Object Creation Methods For RecordingUnits
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * createRecordingUnit
          *
          * Creates a protocol dependent object for RecordingUnits.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDRecordingUnit*
          *             The new RecordingUnit.
          *
          * @param      TA_Base_Core::RecordingUnit& recordingUnitData
          *             The database object representing this RecordingUnit.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDRecordingUnit cannot be created.
          */

        virtual IPDRecordingUnit* createRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent Object Creation Methods For BVSStages
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * createBVSStage
          *
          * Creates a protocol dependent object for BVSStages.
          * It is the *CALLER'S* responsibility to clean up the returned object.
          *
          * @return     IPDBVSStage*
          *             The new BVSStage.
          *
          * @param      TA_Base_Core::BVSStage& BVSStageData
          *             The database object representing this BVSStage.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if a new IPDBVSStage cannot be created.
          */

    //    static IPDBVSStage* createBVSStage( TA_Base_Core::BVSStagePtr BVSStageData );

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        PDInterfaceFactory( const PDInterfaceFactory& thePDInterfaceFactory);
		PDInterfaceFactory& operator=( const PDInterfaceFactory& );
    };

	void initPDInterfaceFactory();
	PDInterfaceFactory& getPDInterfaceFactory();

} // TA_IRS_App

#endif // PDInterfaceFactory_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_