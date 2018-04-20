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
#if !defined(CctvAgentInterface_H)
#define CctvAgentInterface_H


#include "app/trains/train_agent/src/ICctvAgentInterface.h"

#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"

#include "core/naming/src/NamedObject.h"


namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
}


namespace TA_IRS_App
{

    /**
     * This is an interface to the CCTV Agent functions needed by the train agent
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:22 PM
     */
    class CctvAgentInterface : public ICctvAgentInterface
    {
    public:

        /**
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:22 PM
         */
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::SwitchManagerCorbaDef,
                                           TA_Base_Bus::SwitchManagerCorbaDef_ptr,
                                           TA_Base_Bus::SwitchManagerCorbaDef_var > SwitchManagerObject;


        /**
         * Constructor with required information
         *
         * @param configuration    Agent configuration object
         */
        CctvAgentInterface( ITrainAgentConfiguration& configuration );


        /**
         * Destructor
         */
        virtual ~CctvAgentInterface();


        /**
         * This will allow or disallow automatic stage switching between 2 stages.
         *
         * @exception AgentCommunicationException if there is an error communicating with the agent
         * @exception InvalidParameterException for unknown stage IDs
         *
         * @param originatingStage    The stage ID the switch will be made from
         * @param destinationStage    The stage ID the switch will be made to
         * @param allowSwitch    Whether the video switch can automatically make the switch.
         */
        void setStageSwitchMode( unsigned long originatingStage,
                                 unsigned long destinationStage,
                                 bool allowSwitch );


    private:


        /**
         * Private default constructor
         */
        CctvAgentInterface();


        /**
         * This will load the local switch agent entity from the database, and populate the named object with the required details.
         */
        void loadAgentObject();


        /**
         * Used to get configuration items
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * This is used to perform operations on the local CCTV agent
         */
        SwitchManagerObject m_switchManager;

    };

} // TA_IRS_App

#endif // !defined(CctvAgentInterface_H)
