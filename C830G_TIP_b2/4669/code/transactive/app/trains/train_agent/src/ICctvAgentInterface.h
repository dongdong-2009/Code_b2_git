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
#if !defined(ICctvAgentInterface_H)
#define ICctvAgentInterface_H

namespace TA_IRS_App
{

    /**
     * This is the interface used to communicate with the CCTV Agent. It is only really applicable at the OCC.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:40 PM
     */
    class ICctvAgentInterface
    {

    public:

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
        virtual void setStageSwitchMode( unsigned long originatingStage,
                                         unsigned long destinationStage,
                                         bool allowSwitch ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ICctvAgentInterface_H)
