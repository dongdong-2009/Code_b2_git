#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"

namespace TA_IRS_App
{
	class MODBUSPDInterfaceFactory : public PDInterfaceFactory
	{
	public:
        /**
          * PDInterfaceFactory
          * 
          * Standard constructor.
          */

		MODBUSPDInterfaceFactory();

        /**
          * ~PDInterfaceFactory
          * 
          * Standard destructor.
          */

		virtual ~MODBUSPDInterfaceFactory();

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

        virtual IPDCommunicationsHandler* createCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData );

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

        virtual IPDVideoInput* createVideoInput( TA_Base_Core::VideoInputPtr videoInputData );

        /**
          * isSuperimposedTextControlSupported
          *
          * Checks if the protocol supports superimposed text control.
          *
          * @return     bool
          *             True if createSuperimposedTextControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isSuperimposedTextControlSupported();

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

        virtual IPDSuperimposedTextControl* createSuperimposedTextControl( TA_Base_Core::VideoInputPtr videoInputData );


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
		virtual IPDSequence* createSequence( TA_Base_Core::SequencePtr sequenceData );
		 

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
		virtual IPDQuad* createQuad( TA_Base_Core::QuadPtr quadData );
		 

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

        virtual IPDCamera* createCamera( TA_Base_Core::CameraPtr cameraData );

        /**
          * isMovementControlSupported
          *
          * Checks if the protocol supports movement control.
          *
          * @return     bool
          *             True if createMovementControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isMovementControlSupported();

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

        virtual IPDMovementControl* createMovementControl( TA_Base_Core::CameraPtr cameraData );

        /**
          * isFocusIrisControlSupported
          *
          * Checks if the protocol supports focus and/or iris control.
          *
          * @return     bool
          *             True if createFocusIrisControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isFocusIrisControlSupported();

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

        virtual IPDFocusIrisControl* createFocusIrisControl( TA_Base_Core::CameraPtr cameraData );
        
        /**
          * isWashWipeControlSupported
          *
          * Checks if the protocol supports wash and/or wipe control.
          *
          * @return     bool
          *             True if createWashWipeControl() does not return 0.
          *             False otherwise.
          */

        virtual bool isWashWipeControlSupported();

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

        virtual IPDWashWipeControl* createWashWipeControl( TA_Base_Core::CameraPtr cameraData );

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

        virtual IPDVideoOutput* createVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData );

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

        virtual IPDRecordingUnit* createRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData );

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
    };
}