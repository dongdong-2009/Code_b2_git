#if !defined COE_SWITCH_MANAGER_H
#define COE_SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h $
  * @author:   Rob Young 
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/18 14:09:13 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Manages access to the protocol dependent entities (Cameras, VideoMonitors, 
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * This class is a singleton.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEBVSStage.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"

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
    class DataPointFactory;
    class DataPoint;
}

namespace TA_IRS_App
{
    class COECommunicationsHandler;
    class StateValidationCycle;
}

namespace TA_IRS_App
{
    class COESwitchManager
    {
    public:

        enum ECOEInputType
        {
            CAMERA,
            QUAD,
            SEQUENCE,
            BVS_STAGE
        };
		static const unsigned short SYSTEM_DATAPOINT_ID;
		//static const unsigned short TRANSMISSION_DATAPOINT_ID;
		static const unsigned short CAMERA_DATAPOINT_ID;
		static const unsigned short ANCILLARY_DATAPOINT_ID;
		//19444
		static const unsigned short DVR_DATAPOINT_ID;
        /**
          * getInstance
          *
          * Gets the one and only AgentModeMonitor.
          *
          * @return     COESwitchManager&
          *             The only COESwitchManager.
          */

		static COESwitchManager& getInstance();

        /** 
          * removeInstance
          *
          * description
          *
          */
        static void removeInstance();
        
        /**
          * ~COESwitchManager
          *
          * Standard destructor (only called after client calls removeInstance).
          */

        virtual ~COESwitchManager();

        ///////////////////////////////////////////////////////////////////
        //
        // Accessor Methods
        //
        ///////////////////////////////////////////////////////////////////


        ECOEInputType getInputType( unsigned long entityKey );

   	    /**
          * getCamera
          *
	      * Retrieves a COECamera by entity key.
          *
          * @return     COECamera*
          *             The requested COECamera or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the COECamera to retrieve.
	      */

         COECamera* getCamera( unsigned long entityKey );

         COECamera* getCameraFromLocationAndId( unsigned short location, unsigned short id );

	    /**
          * getQuad
          *
	      * Retrieves a COEQuad by entity key.
          *
          * @return     COEQuad*
          *             The requested COEQuad or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the COEQuad to retrieve.
	      */

     //   COEQuad* getQuad( unsigned long entityKey );

     //   COEQuad* getQuadFromId( unsigned short id );

	    /**
          * getBVSStage
          *
	      * Retrieves a COEBVSStage by entity key.
          *
          * @return     COEBVSStage*
          *             The requested COEBVSStage or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the COEBVSStage to retrieve.
	      */

   //     COEBVSStage* getBVSStage( unsigned long entityKey );

   //     COEBVSStage* getBVSStageFromId( unsigned short id );

  	    /**
          * getSequence
          *
	      * Retrieves a COESequence by entity key.
          *
          * @return     COESequence*
          *             The requested COESequence or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the COESequence to retrieve.
	      */

        COESequence* getSequence( unsigned long entityKey );

        COESequence* getSequenceFromId( unsigned short id );

	    /**
          * getVideoOutput
          *
	      * Retrieves a COEVideoOutput by entity key.
          *
          * @return     COEVideoOutput*
          *             The requested COEVideoOutput or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the COEVideoOutput to retrieve.
	      */

        COEVideoOutput* getVideoOutput( unsigned long entityKey );

        COEVideoOutput* getVideoOutputFromId( unsigned short id );

        std::vector<COEVideoOutput*>  getAllVideoOutputsWithAssignment( unsigned long entityKey );
    
		TA_Base_Bus::DataPoint* getDataPointFromId( unsigned short id );

		TA_Base_Bus::DataPoint* getDataPointFromKey( unsigned long pkey );

        //TD16761 Mintao++
		std::map< unsigned short, TA_Base_Bus::DataPoint* > getAllDataPoints();
		///////////////////////////////////////////////////////////////////
        //
        // Add/Remove Entity Methods
        //
        ///////////////////////////////////////////////////////////////////
  

        // Static function pointer to createDataPoint
        static TA_Base_Bus::IEntity* staticCreateDataPoint( TA_Base_Core::IEntityDataPtr entityData );

        
        /** 
          * createDataPoint
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        TA_Base_Bus::IEntity* createDataPoint( TA_Base_Core::IEntityDataPtr entityData );

    
        /** 
          * addCamera
          *
          * Adds the given COECamera.
          *
          * @param      COECamera& camera
          *             The new COECamera.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a COECamera with the given entity key already exists.
          */
        
        void addCamera( COECamera& camera );

        /** 
          * removeCamera
          *
          * Removes the given COECamera. This method does nothing if it
          * can't find a COECamera with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The COECamera to remove.
          */
        
        void removeCamera( unsigned long entityKey );
 
        /** 
          * addQuad
          *
          * Adds the given COEQuad.
          *
          * @param      COEQuad& quad
          *             The new COEQuad.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a COEQuad with the given entity key already exists.
          */
        
       // void addQuad( COEQuad& quad );

        /** 
          * removeQuad
          *
          * Removes the given COEQuad. This method does nothing if it
          * can't find a COEQuad with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The COEQuad to remove.
          */
        
      //  void removeQuad( unsigned long entityKey );

        /** 
          * addBVSStage
          *
          * Adds the given COEBVSStage.
          *
          * @param      COEBVSStage& quad
          *             The new COEBVSStage.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a COEBVSStage with the given entity key already exists.
          */
        
      //  void addBVSStage( COEBVSStage& bvsStage );

        /** 
          * removeBVSStage
          *
          * Removes the given COEBVSStage. This method does nothing if it
          * can't find a COEBVSStage with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The COEBVSStage to remove.
          */
        
      //  void removeBVSStage( unsigned long entityKey );

        /** 
          * addSequence
          *
          * Adds the given COESequence.
          *
          * @param      COESequence& sequence
          *             The new COESequence.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a COESequence with the given entity key already exists.
          */
        
        void addSequence( COESequence& sequence );

         /** 
          * removeSequence
          *
          * Removes the given COESequence. This method does nothing if it
          * can't find a COESequence with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The COESequence to remove.
          */
        
        void removeSequence( unsigned long entityKey );      

        /** 
          * addVideoOutput
          *
          * Adds the given COEVideoOutput. Use this for generic COEVideoOutputs like
          * ProjectionUnits that don't have their own specialied add method.
          *
          * @param      COEVideoOutput& videoOutput
          *             The new COEVideoOutput.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a COEVideoOutput with the given entity key already exists.
          */
        
        void addVideoOutput( COEVideoOutput& videoOutput );

        /** 
          * removeVideoOutput
          *
          * Removes the given COEVideoOutput. This method does nothing if it
          * can't find a VideoOutput with the given entity key. Use this for generic 
          * VideoOutputs like ProjectionUnits that don't have their own specialied 
          * remove method. 
          *
          * @param      unsigned long entityKey
          *             The COEVideoOutput to remove.
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
          * @return     COECommunicationsHandler*
          *             The requested COEVideoInput.
          *
          * @exception  VideoSwitchAgentInternalException
          *             If the CommunicationsHandler has not been added yet.
	      */

        COECommunicationsHandler* getCommunicationsHandler();

        /**
          * addCommunicationsHander
          * 
          * Adds the CommunicationsHandler.
          *
          * @param      COECommunicationsHandler& commsHandler
          *             The handler to add.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the CommunicationsHandler already exists.
          */

        void addCommunicationsHander( COECommunicationsHandler& commsHandler );

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
        StateValidationCycle& getStateValidationCycle();

        /** 
          * registerDataPointFactory
          *
          * description
          *
          *
          * @return 
          */
        void registerDataPointFactory(TA_Base_Bus::DataPointFactory* dataPointFactory);

    private:
        typedef std::map< unsigned long, ECOEInputType >       InputTypeMap;
        typedef std::map< unsigned short, COEVideoOutput* >    IdToVideoOutput;
        typedef std::map< unsigned short, COECamera* >         IdToCamera;
        typedef std::map< unsigned short, IdToCamera >         LocationAndIdToCamera;
      //  typedef std::map< unsigned short, COEQuad* >           IdToQuad;
      //  typedef std::map< unsigned short, COEBVSStage* >       IdToBVSStage;
        typedef std::map< unsigned short, COESequence* >       IdToSequence;
        typedef std::map< unsigned short, TA_Base_Bus::DataPoint* > IdToDataPoint;
        typedef std::map< unsigned long, COEVideoOutput* >     VideoOutputMap;
        typedef std::map< unsigned long, COEVideoInput* >      VideoInputMap;
        typedef std::map< unsigned long, COECamera* >          CameraMap;
    //    typedef std::map< unsigned long, COEQuad* >            QuadMap;
    //    typedef std::map< unsigned long, COEBVSStage* >        BVSStageMap;
        typedef std::map< unsigned long, COESequence* >        SequenceMap;
		typedef std::map< short,std::string>				   DPBitNameMap;

        /**
          * COESwitchManager
          *
          * Standard constructor. This is private because this class is a singleton.
          * Use getInstance() to get the one and only instance of this class.
          */

        COESwitchManager();

        //
        // Disable copy constructor and assignment operator.
        //

        COESwitchManager( const COESwitchManager& theCOESwitchManager );
        COESwitchManager& operator=( const COESwitchManager& theCOESwitchManager );

		//
		// The one and only instance of this class.
		//

		static COESwitchManager* m_theClassInstance;
		
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
    //    QuadMap             m_quads;
    //    BVSStageMap         m_bvsStages;
        SequenceMap         m_sequences;

        IdToVideoOutput        m_idToVideoOutput;
        LocationAndIdToCamera  m_locationAndIdToCamera;
    //    IdToQuad               m_idToQuad;
     //   IdToBVSStage           m_idToBVSStage;
        IdToSequence           m_idToSequence;
        IdToDataPoint          m_idToDataPoint;
		DPBitNameMap		   m_DPBitNameMap;
        //
        // The CommunicationsHandler for the COE Video Switch Agent.
        // Note: This is only stored to provide access to the object.  It is not
        // maintained (created, cleaned up, etc) by the COESwitchManager.
        //
        COECommunicationsHandler* m_commsHandler;

        StateValidationCycle* m_stateValidationCycle;

        TA_Base_Bus::DataPointFactory* m_dataPointFactory;

        static const std::string SUPPORTED_DATAPOINT_TYPE;
    };

} // TA_IRS_App

#endif // COE_SWITCH_MANAGER_H
