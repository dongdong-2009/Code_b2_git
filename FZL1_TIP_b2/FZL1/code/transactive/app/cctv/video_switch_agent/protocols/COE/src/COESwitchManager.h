#if !defined COE_SWITCH_MANAGER_H
#define COE_SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Rob Young
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Manages access to the protocol dependent entities (Cameras, VideoMonitors,
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * This class is a singleton.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <boost/noncopyable.hpp>
#include <map>

// Forward declarations.

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

namespace TA_IRS_App
{
    class COECommunicationsHandler;
    class COESwitchManager : boost::noncopyable
    {
            friend class ACE_Singleton<COESwitchManager, ACE_Recursive_Thread_Mutex>;
        public:

            enum ECOEInputType
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
              * @return     COESwitchManager&
              *             The only COESwitchManager.
              */

            static COESwitchManager& getInstance();

            ///////////////////////////////////////////////////////////////////
            //
            // Accessor Methods
            //
            ///////////////////////////////////////////////////////////////////


            ECOEInputType getInputType(unsigned long entityKey);

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

            COECamera* getCamera(unsigned long entityKey);

            COESequence* getSequence(unsigned long entityKey);

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

            COEVideoOutput* getVideoOutput(unsigned long entityKey);

            std::vector<COEVideoOutput*>  getAllVideoOutputsWithAssignment(unsigned long entityKey);

            ///////////////////////////////////////////////////////////////////
            //
            // Add/Remove Entity Methods
            //
            ///////////////////////////////////////////////////////////////////


            void addCamera(COECamera& camera);

            /**
              * removeCamera
              *
              * Removes the given COECamera. This method does nothing if it
              * can't find a COECamera with the given entity key.
              *
              * @param      unsigned long entityKey
              *             The COECamera to remove.
              */

            void removeCamera(unsigned long entityKey);


            void addSequence(COESequence& sequence);

            /**
             * removeSequence
             *
             * Removes the given COESequence. This method does nothing if it
             * can't find a COESequence with the given entity key.
             *
             * @param      unsigned long entityKey
             *             The COESequence to remove.
             */

            void removeSequence(unsigned long entityKey);

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

            void addVideoOutput(COEVideoOutput& videoOutput);

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

            void removeVideoOutput(unsigned long entityKey);


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

            void addCommunicationsHander(COECommunicationsHandler& commsHandler);

            /**
              * removeCommunicationsHander
              *
              * Removes the CommunicationsHandler.  NOTE: This WILL NOT clean up
              * the CommunicationsHandler.  It is expected that the object that
              * added the handler will clean it up.
              */

            void removeCommunicationsHander();


        private:
            typedef std::map< unsigned long, ECOEInputType >       InputTypeMap;
            typedef std::map< unsigned long, COEVideoOutput* >     VideoOutputMap;
            typedef std::map< unsigned long, COEVideoInput* >      VideoInputMap;
            typedef std::map< unsigned long, COECamera* >          CameraMap;
            typedef std::map< unsigned long, COESequence* >        SequenceMap;

            /**
              * COESwitchManager
              *
              * Standard constructor. This is private because this class is a singleton.
              * Use getInstance() to get the one and only instance of this class.
              */

            COESwitchManager();
            virtual ~COESwitchManager();

            //
            // The objects within these maps.
            //
            InputTypeMap        m_inputTypes;
            VideoOutputMap      m_videoOutputs;
            VideoInputMap       m_videoInputs;
            CameraMap           m_cameras;
            SequenceMap         m_sequences;
            //
            // The CommunicationsHandler for the COE Video Switch Agent.
            // Note: This is only stored to provide access to the object.  It is not
            // maintained (created, cleaned up, etc) by the COESwitchManager.
            //
            COECommunicationsHandler* m_commsHandler;
    };

} // TA_IRS_App

#endif // COE_SWITCH_MANAGER_H
