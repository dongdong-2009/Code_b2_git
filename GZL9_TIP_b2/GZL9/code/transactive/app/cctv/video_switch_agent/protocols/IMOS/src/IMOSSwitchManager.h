#if !defined IMOS_SWITCH_MANAGER_H
#define IMOS_SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h $
  * @author:   Rob Young 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/13 14:55:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Manages access to the protocol dependent entities (Cameras, VideoMonitors, 
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * This class is a singleton.
  */

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCamera.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSQuad.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSBVSStage.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSequence.h"

#include <map>

// Forward declarations.

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
    class IEntityData;
}

namespace TA_Base_Bus
{
    class IEntity;
}

namespace TA_IRS_App
{
    class IMOSCommunicationsHandler;
    class StateValidationCycle;
}

namespace TA_IRS_App
{
    class IMOSSwitchManager
    {
    public:

        enum EIMOSInputType
        {
            CAMERA,
            QUAD,
            SEQUENCE,
            BVS_STAGE
        };

        /**
          * getInstance
          *
          * Gets the one and only AgentModeMonitor.
          *
          * @return     IMOSSwitchManager&
          *             The only IMOSSwitchManager.
          */

		static IMOSSwitchManager& getInstance();

        /** 
          * removeInstance
          *
          * description
          *
          */
        static void removeInstance();
        
        /**
          * ~IMOSSwitchManager
          *
          * Standard destructor (only called after client calls removeInstance).
          */

        virtual ~IMOSSwitchManager();

        ///////////////////////////////////////////////////////////////////
        //
        // Accessor Methods
        //
        ///////////////////////////////////////////////////////////////////


        EIMOSInputType getInputType( unsigned long entityKey );

	    /**
          * getRecordingUnit
          *
	      * Retrieves a IMOSRecordingUnit by entity key.
          *
          * @return     IMOSRecordingUnit*
          *             The requested IMOSRecordingUnit or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSRecordingUnit to retrieve.
	      */

        IMOSRecordingUnit* getRecordingUnit( unsigned long entityKey );

        IMOSRecordingUnit* getRecordingUnitFromId( unsigned short id );

   	    /**
          * getCamera
          *
	      * Retrieves a IMOSCamera by entity key.
          *
          * @return     IMOSCamera*
          *             The requested IMOSCamera or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSCamera to retrieve.
	      */

         IMOSCamera* getCamera( unsigned long entityKey );

         IMOSCamera* getCameraFromLocationAndId( unsigned short location, unsigned short id );

	    /**
          * getQuad
          *
	      * Retrieves a IMOSQuad by entity key.
          *
          * @return     IMOSQuad*
          *             The requested IMOSQuad or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSQuad to retrieve.
	      */

        IMOSQuad* getQuad( unsigned long entityKey );

        IMOSQuad* getQuadFromId( unsigned short id );

	    /**
          * getBVSStage
          *
	      * Retrieves a IMOSBVSStage by entity key.
          *
          * @return     IMOSBVSStage*
          *             The requested IMOSBVSStage or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSBVSStage to retrieve.
	      */

      //  IMOSBVSStage* getBVSStage( unsigned long entityKey );

     //   IMOSBVSStage* getBVSStageFromId( unsigned short id );

  	    /**
          * getSequence
          *
	      * Retrieves a IMOSSequence by entity key.
          *
          * @return     IMOSSequence*
          *             The requested IMOSSequence or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSSequence to retrieve.
	      */

        IMOSSequence* getSequence( unsigned long entityKey );

        IMOSSequence* getSequenceFromId( unsigned short id );

	    /**
          * getVideoOutput
          *
	      * Retrieves a IMOSVideoOutput by entity key.
          *
          * @return     IMOSVideoOutput*
          *             The requested IMOSVideoOutput or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the IMOSVideoOutput to retrieve.
	      */

        IMOSVideoOutput* getVideoOutput( unsigned long entityKey );

        IMOSVideoOutput* getVideoOutputFromId( unsigned short id );

        std::vector<IMOSVideoOutput*>  getAllVideoOutputsWithAssignment( unsigned long entityKey );
    


		///////////////////////////////////////////////////////////////////
        //
        // Add/Remove Entity Methods
        //
        ///////////////////////////////////////////////////////////////////
  

        
        /** 
          * addRecordingUnit
          *
          * Adds the given IMOSRecordingUnit.
          *
          * @param      IMOSRecordingUnit& recordingUnit
          *             The new RecordingUnit.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSRecordingUnit with the given entity key already exists.
          */
        
        void addRecordingUnit( IMOSRecordingUnit& recordingUnit );

        /** 
          * removeRecordingUnit
          *
          * Removes the given IMOSRecordingUnit. This method does nothing if it
          * can't find a IMOSRecordingUnit with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The IMOSRecordingUnit to remove.
          */
        
        void removeRecordingUnit( unsigned long entityKey );
    
        /** 
          * addCamera
          *
          * Adds the given IMOSCamera.
          *
          * @param      IMOSCamera& camera
          *             The new IMOSCamera.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSCamera with the given entity key already exists.
          */
        
        void addCamera( IMOSCamera& camera );

        /** 
          * removeCamera
          *
          * Removes the given IMOSCamera. This method does nothing if it
          * can't find a IMOSCamera with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The IMOSCamera to remove.
          */
        
        void removeCamera( unsigned long entityKey );
 
        /** 
          * addQuad
          *
          * Adds the given IMOSQuad.
          *
          * @param      IMOSQuad& quad
          *             The new IMOSQuad.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSQuad with the given entity key already exists.
          */
        
        void addQuad( IMOSQuad& quad );

        /** 
          * removeQuad
          *
          * Removes the given IMOSQuad. This method does nothing if it
          * can't find a IMOSQuad with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The IMOSQuad to remove.
          */
        
        void removeQuad( unsigned long entityKey );

        /** 
          * addBVSStage
          *
          * Adds the given IMOSBVSStage.
          *
          * @param      IMOSBVSStage& quad
          *             The new IMOSBVSStage.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSBVSStage with the given entity key already exists.
          */
        
     //   void addBVSStage( IMOSBVSStage& bvsStage );

        /** 
          * removeBVSStage
          *
          * Removes the given IMOSBVSStage. This method does nothing if it
          * can't find a IMOSBVSStage with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The IMOSBVSStage to remove.
          */
        
   //     void removeBVSStage( unsigned long entityKey );

        /** 
          * addSequence
          *
          * Adds the given IMOSSequence.
          *
          * @param      IMOSSequence& sequence
          *             The new IMOSSequence.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSSequence with the given entity key already exists.
          */
        
        void addSequence( IMOSSequence& sequence );

         /** 
          * removeSequence
          *
          * Removes the given IMOSSequence. This method does nothing if it
          * can't find a IMOSSequence with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The IMOSSequence to remove.
          */
        
        void removeSequence( unsigned long entityKey );      

        /** 
          * addVideoOutput
          *
          * Adds the given IMOSVideoOutput. Use this for generic IMOSVideoOutputs like
          * ProjectionUnits that don't have their own specialied add method.
          *
          * @param      IMOSVideoOutput& videoOutput
          *             The new IMOSVideoOutput.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a IMOSVideoOutput with the given entity key already exists.
          */
        
        void addVideoOutput( IMOSVideoOutput& videoOutput );

        /** 
          * removeVideoOutput
          *
          * Removes the given IMOSVideoOutput. This method does nothing if it
          * can't find a VideoOutput with the given entity key. Use this for generic 
          * VideoOutputs like ProjectionUnits that don't have their own specialied 
          * remove method. 
          *
          * @param      unsigned long entityKey
          *             The IMOSVideoOutput to remove.
          */
        
        void removeVideoOutput( unsigned long entityKey );

        
        ///////////////////////////////////////////////////////////////////
        //
        // CommsHandler Methods
        //
        ///////////////////////////////////////////////////////////////////
     
        /**
          * getCommunicationsHandler
          *
	      * Gets the CommunicationsHandler.
          *
          * @return     IMOSCommunicationsHandler*
          *             The requested IMOSVideoInput.
          *
          * @exception  VideoSwitchAgentInternalException
          *             If the CommunicationsHandler has not been added yet.
	      */

        IMOSCommunicationsHandler* getCommunicationsHandler();

        /**
          * addCommunicationsHander
          * 
          * Adds the CommunicationsHandler.
          *
          * @param      IMOSCommunicationsHandler& commsHandler
          *             The handler to add.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the CommunicationsHandler already exists.
          */

        void addCommunicationsHander( IMOSCommunicationsHandler& commsHandler );

        /**
          * removeCommunicationsHander
          *
          * Removes the CommunicationsHandler.  NOTE: This WILL NOT clean up
          * the CommunicationsHandler.  It is expected that the object that
          * added the handler will clean it up.
          */

        void removeCommunicationsHander();


        /** 
          * getStateValidationCycle
          *
          * description
          *
          *
          * @return 
          */
        //StateValidationCycle& getStateValidationCycle();

    private:
        typedef std::map< unsigned long, EIMOSInputType >       InputTypeMap;
        typedef std::map< unsigned short, IMOSVideoOutput* >    IdToVideoOutput;
        typedef std::map< unsigned short, IMOSCamera* >         IdToCamera;
        typedef std::map< unsigned short, IdToCamera >         LocationAndIdToCamera;
        typedef std::map< unsigned short, IMOSQuad* >           IdToQuad;
    //    typedef std::map< unsigned short, IMOSBVSStage* >       IdToBVSStage;
        typedef std::map< unsigned short, IMOSSequence* >       IdToSequence;
        typedef std::map< unsigned short, IMOSRecordingUnit* >  IdToRecordingUnit;
        typedef std::map< unsigned long, IMOSVideoOutput* >     VideoOutputMap;
        typedef std::map< unsigned long, IMOSVideoInput* >      VideoInputMap;
        typedef std::map< unsigned long, IMOSCamera* >          CameraMap;
        typedef std::map< unsigned long, IMOSRecordingUnit* >   RecordingUnitMap;
        typedef std::map< unsigned long, IMOSQuad* >            QuadMap;
    //    typedef std::map< unsigned long, IMOSBVSStage* >        BVSStageMap;
        typedef std::map< unsigned long, IMOSSequence* >        SequenceMap;


        /**
          * IMOSSwitchManager
          *
          * Standard constructor. This is private because this class is a singleton.
          * Use getInstance() to get the one and only instance of this class.
          */

        IMOSSwitchManager();

        //
        // Disable copy constructor and assignment operator.
        //

        IMOSSwitchManager( const IMOSSwitchManager& theIMOSSwitchManager );
        IMOSSwitchManager& operator=( const IMOSSwitchManager& theIMOSSwitchManager );

		//
		// The one and only instance of this class.
		//

		static IMOSSwitchManager* m_theClassInstance;
		
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        //
        // The objects within these maps. 
        //
        InputTypeMap        m_inputTypes;
	    VideoOutputMap      m_videoOutputs;
        VideoInputMap       m_videoInputs;
        CameraMap           m_cameras;
        RecordingUnitMap    m_recordingUnits;
        QuadMap             m_quads;
   //     BVSStageMap         m_bvsStages;
        SequenceMap         m_sequences;

        IdToVideoOutput        m_idToVideoOutput;
        LocationAndIdToCamera  m_locationAndIdToCamera;
        IdToQuad               m_idToQuad;
    //    IdToBVSStage           m_idToBVSStage;
        IdToSequence           m_idToSequence;
        IdToRecordingUnit      m_idToRecordingUnit;

        //
        // The CommunicationsHandler for the IMOS Video Switch Agent.
        // Note: This is only stored to provide access to the object.  It is not
        // maintained (created, cleaned up, etc) by the IMOSSwitchManager.
        //
        IMOSCommunicationsHandler* m_commsHandler;

        //StateValidationCycle* m_stateValidationCycle;

    };

} // TA_IRS_App

#endif // IMOS_SWITCH_MANAGER_H
