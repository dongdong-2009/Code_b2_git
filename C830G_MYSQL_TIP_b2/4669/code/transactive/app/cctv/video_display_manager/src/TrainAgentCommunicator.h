#ifndef TRAINAGENTCOMMUNICATOR_H
#define TRAINAGENTCOMMUNICATOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/TrainAgentCommunicator.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Singleton to provide access to Agent data and functions
  *
  */

#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainInformationCorbaDef.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainCorbaDef.h"

#include "core/naming/src/NamedObject.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}


namespace TA_IRS_App
{
    class TrainAgentCommunicator
    {
    public:
        bool sendTrainCarrier( CORBA::Octet trainID, bool enable );

        bool sendTrainSingleScreen( CORBA::Octet trainID, CORBA::Octet cameraX );

        bool sendTrainQuadScreen( CORBA::Octet trainID, CORBA::Octet cameraW, CORBA::Octet cameraX, CORBA::Octet cameraY, CORBA::Octet cameraZ );

        bool sendTrainSequence( CORBA::Octet trainID, CORBA::Octet sequenceNo );

        bool clearTrainAlarmImage( CORBA::Octet trainID, CORBA::Octet cameraId );

        virtual ~TrainAgentCommunicator() {};

        /**
          * getInstance
          *
          * Return reference to this singleton.
          *
          * @return reference to this singleton.
          */
        static TrainAgentCommunicator& getInstance();


       /**
         * removeInstance
         *
         * Removes the instance of the class (if already created) and cleanup the members.  Primarily
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         */
        static void removeInstance();

        std::string getLastErrorMessage() { return m_lastErrorMessage; }

    private:
        /**
          * Default Constructor
          *
          * Constructor is private so can control number of instances
          */
        TrainAgentCommunicator();


        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        TrainAgentCommunicator( const TrainAgentCommunicator& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        TrainAgentCommunicator& operator=( const TrainAgentCommunicator& );


        /**
          * initialiseTrainAgentNamedObj
          *
          * description
          *
          */
        void initialiseTrainAgentNamedObj();

    private:
        static TrainAgentCommunicator* m_theInstance; // pointer to the single instance of this class.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock; // Semaphore to thread guard this singleton.

        std::string m_lastErrorMessage;

         // 
        // Pointer to the SwitchManager.
        // 

        TA_Base_Core::FlexibleTimeoutNamedObject< TA_Base_Bus::ITrainCorbaDef,
            TA_Base_Bus::ITrainCorbaDef_ptr,
            TA_Base_Bus::ITrainCorbaDef_var > m_cctvTrainAgentCommands;

        bool m_isTrainAgentNamedObjIntialised;
    };
} // namespace TA_IRS_App

#endif // TRAINAGENTCOMMUNICATOR_H
