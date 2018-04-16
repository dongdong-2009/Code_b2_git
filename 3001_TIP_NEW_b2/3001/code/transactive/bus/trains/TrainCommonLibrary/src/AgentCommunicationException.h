/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(AgentCommunicationException_H)
#define AgentCommunicationException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * This is thrown when there is a problem communicating with the train agent. It
     * should wrap Operation Mode Exceptions, CORBA Exceptions, Object resolution
     * exceptions.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:26 PM
     */
    class AgentCommunicationException : public TrainException
    {
    public:

        /**
         * constructor
         */
        AgentCommunicationException() throw ();
        
        /**
         * desconstructor
         */
        virtual ~AgentCommunicationException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        AgentCommunicationException(const std::string& msg) throw ();

    };

}

#endif // !defined(AgentCommunicationException_H)
