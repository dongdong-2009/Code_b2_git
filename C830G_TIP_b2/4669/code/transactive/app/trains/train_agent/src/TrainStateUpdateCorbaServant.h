/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainStateUpdateCorbaServant_H)
#define TrainStateUpdateCorbaServant_H


#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class StateUpdateManager;
    class FullGlobalState;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainStateUpdateCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:37 PM
     */
    class TrainStateUpdateCorbaServant : public TA_Base_Core::ServantBase,
                                         public POA_TA_Base_Bus::ITrainStateUpdateCorba
    {

    public:

        /**
         * Constructor
         *
         * @param stateUpdateManager    The object to delegate calls to
         */
        TrainStateUpdateCorbaServant( StateUpdateManager& stateUpdateManager );


        /**
         * Destructor
         */
        virtual ~TrainStateUpdateCorbaServant();


        /**
         * This queries this agent and returns all local state held by this agent.
         * The information it returns is relevant only to the peer of this agent (ie this is only used between control/monitor pairs).
         *
         * @return the full local state of this agent
         */
        virtual TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState* getLocalState();


        /**
         * This queries this agent and returns all global state held by this agent.
         * The information it returns is relevant to all train agents throughout the system.
         *
         * @return the full global state of this agent
         */
        virtual TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState* getGlobalState();


    private:


        /**
         * Constructor
         */
        TrainStateUpdateCorbaServant();


        /**
         * This is the actual implementation of the functionality - calls are delegated to this object
         */
        StateUpdateManager& m_stateUpdateManager;

    };

} // TA_IRS_App

#endif // !defined(TrainStateUpdateCorbaServant_H)
