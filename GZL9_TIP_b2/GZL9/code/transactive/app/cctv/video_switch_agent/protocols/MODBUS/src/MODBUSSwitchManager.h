#if !defined MODBUS_SWITCH_MANAGER_H
#define MODBUS_SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h $
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

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCamera.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSQuad.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSBVSStage.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSequence.h"

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
    class MODBUSCommunicationsHandler;
    class StateValidationCycle;
}

namespace TA_IRS_App
{
    class MODBUSSwitchManager
    {
    public:

        enum EMODBUSInputType
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
          * @return     MODBUSSwitchManager&
          *             The only MODBUSSwitchManager.
          */

		static MODBUSSwitchManager& getInstance();

        /** 
          * removeInstance
          *
          * description
          *
          */
        static void removeInstance();
        
        /**
          * ~MODBUSSwitchManager
          *
          * Standard destructor (only called after client calls removeInstance).
          */

        virtual ~MODBUSSwitchManager();

        ///////////////////////////////////////////////////////////////////
        //
        // Accessor Methods
        //
        ///////////////////////////////////////////////////////////////////


        EMODBUSInputType getInputType( unsigned long entityKey );

	    /**
          * getRecordingUnit
          *
	      * Retrieves a MODBUSRecordingUnit by entity key.
          *
          * @return     MODBUSRecordingUnit*
          *             The requested MODBUSRecordingUnit or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSRecordingUnit to retrieve.
	      */

        MODBUSRecordingUnit* getRecordingUnit( unsigned long entityKey );

        MODBUSRecordingUnit* getRecordingUnitFromId( unsigned short id );

   	    /**
          * getCamera
          *
	      * Retrieves a MODBUSCamera by entity key.
          *
          * @return     MODBUSCamera*
          *             The requested MODBUSCamera or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSCamera to retrieve.
	      */

         MODBUSCamera* getCamera( unsigned long entityKey );

         MODBUSCamera* getCameraFromLocationAndId( unsigned short location, unsigned short id );

	    /**
          * getQuad
          *
	      * Retrieves a MODBUSQuad by entity key.
          *
          * @return     MODBUSQuad*
          *             The requested MODBUSQuad or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSQuad to retrieve.
	      */

        MODBUSQuad* getQuad( unsigned long entityKey );

        MODBUSQuad* getQuadFromId( unsigned short id );

	    /**
          * getBVSStage
          *
	      * Retrieves a MODBUSBVSStage by entity key.
          *
          * @return     MODBUSBVSStage*
          *             The requested MODBUSBVSStage or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSBVSStage to retrieve.
	      */

      //  MODBUSBVSStage* getBVSStage( unsigned long entityKey );

     //   MODBUSBVSStage* getBVSStageFromId( unsigned short id );

  	    /**
          * getSequence
          *
	      * Retrieves a MODBUSSequence by entity key.
          *
          * @return     MODBUSSequence*
          *             The requested MODBUSSequence or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSSequence to retrieve.
	      */

        MODBUSSequence* getSequence( unsigned long entityKey );

        MODBUSSequence* getSequenceFromId( unsigned short id );

	    /**
          * getVideoOutput
          *
	      * Retrieves a MODBUSVideoOutput by entity key.
          *
          * @return     MODBUSVideoOutput*
          *             The requested MODBUSVideoOutput or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the MODBUSVideoOutput to retrieve.
	      */

        MODBUSVideoOutput* getVideoOutput( unsigned long entityKey );

		MODBUSVideoOutput* getVideoOutPutByDisplayOrderAndConsole(int displayOrder, unsigned long consoleKey);

        MODBUSVideoOutput* getVideoOutputFromId( unsigned short id );

        std::vector<MODBUSVideoOutput*>  getAllVideoOutputsWithAssignment( unsigned long entityKey );
    
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
          * addRecordingUnit
          *
          * Adds the given MODBUSRecordingUnit.
          *
          * @param      MODBUSRecordingUnit& recordingUnit
          *             The new RecordingUnit.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSRecordingUnit with the given entity key already exists.
          */
        
        void addRecordingUnit( MODBUSRecordingUnit& recordingUnit );

        /** 
          * removeRecordingUnit
          *
          * Removes the given MODBUSRecordingUnit. This method does nothing if it
          * can't find a MODBUSRecordingUnit with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The MODBUSRecordingUnit to remove.
          */
        
        void removeRecordingUnit( unsigned long entityKey );
    
        /** 
          * addCamera
          *
          * Adds the given MODBUSCamera.
          *
          * @param      MODBUSCamera& camera
          *             The new MODBUSCamera.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSCamera with the given entity key already exists.
          */
        
        void addCamera( MODBUSCamera& camera );

        /** 
          * removeCamera
          *
          * Removes the given MODBUSCamera. This method does nothing if it
          * can't find a MODBUSCamera with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The MODBUSCamera to remove.
          */
        
        void removeCamera( unsigned long entityKey );
 
        /** 
          * addQuad
          *
          * Adds the given MODBUSQuad.
          *
          * @param      MODBUSQuad& quad
          *             The new MODBUSQuad.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSQuad with the given entity key already exists.
          */
        
        void addQuad( MODBUSQuad& quad );

        /** 
          * removeQuad
          *
          * Removes the given MODBUSQuad. This method does nothing if it
          * can't find a MODBUSQuad with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The MODBUSQuad to remove.
          */
        
        void removeQuad( unsigned long entityKey );

        /** 
          * addBVSStage
          *
          * Adds the given MODBUSBVSStage.
          *
          * @param      MODBUSBVSStage& quad
          *             The new MODBUSBVSStage.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSBVSStage with the given entity key already exists.
          */
        
     //   void addBVSStage( MODBUSBVSStage& bvsStage );

        /** 
          * removeBVSStage
          *
          * Removes the given MODBUSBVSStage. This method does nothing if it
          * can't find a MODBUSBVSStage with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The MODBUSBVSStage to remove.
          */
        
   //     void removeBVSStage( unsigned long entityKey );

        /** 
          * addSequence
          *
          * Adds the given MODBUSSequence.
          *
          * @param      MODBUSSequence& sequence
          *             The new MODBUSSequence.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSSequence with the given entity key already exists.
          */
        
        void addSequence( MODBUSSequence& sequence );

         /** 
          * removeSequence
          *
          * Removes the given MODBUSSequence. This method does nothing if it
          * can't find a MODBUSSequence with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The MODBUSSequence to remove.
          */
        
        void removeSequence( unsigned long entityKey );      

        /** 
          * addVideoOutput
          *
          * Adds the given MODBUSVideoOutput. Use this for generic MODBUSVideoOutputs like
          * ProjectionUnits that don't have their own specialied add method.
          *
          * @param      MODBUSVideoOutput& videoOutput
          *             The new MODBUSVideoOutput.
          *
          * @exception  ObjectAlreadyExistsException
          *             Thrown if a MODBUSVideoOutput with the given entity key already exists.
          */
        
        void addVideoOutput( MODBUSVideoOutput& videoOutput );

        /** 
          * removeVideoOutput
          *
          * Removes the given MODBUSVideoOutput. This method does nothing if it
          * can't find a VideoOutput with the given entity key. Use this for generic 
          * VideoOutputs like ProjectionUnits that don't have their own specialied 
          * remove method. 
          *
          * @param      unsigned long entityKey
          *             The MODBUSVideoOutput to remove.
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
          * @return     MODBUSCommunicationsHandler*
          *             The requested MODBUSVideoInput.
          *
          * @exception  VideoSwitchAgentInternalException
          *             If the CommunicationsHandler has not been added yet.
	      */

        MODBUSCommunicationsHandler* getCommunicationsHandler();

        /**
          * addCommunicationsHander
          * 
          * Adds the CommunicationsHandler.
          *
          * @param      MODBUSCommunicationsHandler& commsHandler
          *             The handler to add.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the CommunicationsHandler already exists.
          */

        void addCommunicationsHander( MODBUSCommunicationsHandler& commsHandler );

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
        typedef std::map< unsigned long, EMODBUSInputType >       InputTypeMap;
        typedef std::map< unsigned short, MODBUSVideoOutput* >    IdToVideoOutput;
        typedef std::map< unsigned short, MODBUSCamera* >         IdToCamera;
        typedef std::map< unsigned short, IdToCamera >         LocationAndIdToCamera;
        typedef std::map< unsigned short, MODBUSQuad* >           IdToQuad;
    //    typedef std::map< unsigned short, MODBUSBVSStage* >       IdToBVSStage;
        typedef std::map< unsigned short, MODBUSSequence* >       IdToSequence;
        typedef std::map< unsigned short, MODBUSRecordingUnit* >  IdToRecordingUnit;
        typedef std::map< unsigned short, TA_Base_Bus::DataPoint* > IdToDataPoint;
        typedef std::map< unsigned long, MODBUSVideoOutput* >     VideoOutputMap;
        typedef std::map< unsigned long, MODBUSVideoInput* >      VideoInputMap;
        typedef std::map< unsigned long, MODBUSCamera* >          CameraMap;
        typedef std::map< unsigned long, MODBUSRecordingUnit* >   RecordingUnitMap;
        typedef std::map< unsigned long, MODBUSQuad* >            QuadMap;
    //    typedef std::map< unsigned long, MODBUSBVSStage* >        BVSStageMap;
        typedef std::map< unsigned long, MODBUSSequence* >        SequenceMap;

		typedef std::map<int, MODBUSVideoOutput*>				  DisplayOrderToOutput;
		typedef std::map<unsigned long, DisplayOrderToOutput>     ConsoleToDisplayOrderToOutPut;

        /**
          * MODBUSSwitchManager
          *
          * Standard constructor. This is private because this class is a singleton.
          * Use getInstance() to get the one and only instance of this class.
          */

        MODBUSSwitchManager();

        //
        // Disable copy constructor and assignment operator.
        //

        MODBUSSwitchManager( const MODBUSSwitchManager& theMODBUSSwitchManager );
        MODBUSSwitchManager& operator=( const MODBUSSwitchManager& theMODBUSSwitchManager );

		//
		// The one and only instance of this class.
		//

		static MODBUSSwitchManager* m_theClassInstance;
		
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        //
        // The objects within these maps. 
        //
        InputTypeMap        m_inputTypes;
	    VideoOutputMap      m_videoOutputs;
		ConsoleToDisplayOrderToOutPut m_consoleToDisplayOrderToOutput;
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
        IdToDataPoint          m_idToDataPoint;

        //
        // The CommunicationsHandler for the MODBUS Video Switch Agent.
        // Note: This is only stored to provide access to the object.  It is not
        // maintained (created, cleaned up, etc) by the MODBUSSwitchManager.
        //
        MODBUSCommunicationsHandler* m_commsHandler;

        //StateValidationCycle* m_stateValidationCycle;

        TA_Base_Bus::DataPointFactory* m_dataPointFactory;

        static const std::string SUPPORTED_DATAPOINT_TYPE;
    };

} // TA_IRS_App

#endif // MODBUS_SWITCH_MANAGER_H
