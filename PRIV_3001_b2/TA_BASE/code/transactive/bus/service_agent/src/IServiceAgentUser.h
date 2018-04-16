/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/bus/service_agent/src/IServiceAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#if !defined(ISERVICEAGENTUSER)
#define ISERVICEAGENTUSER

#include "bus/generic_agent/src/IGenericAgentUser.h"

namespace TA_Base_Bus
{
	class IServiceAgentUser : public virtual IGenericAgentUser
	{
    public:

        /**
          * Constructor
          */
        IServiceAgentUser(){}

        /**
          * Destructor
          */
        virtual ~IServiceAgentUser(){}

        /**
          * start
          *
          * This pure virtual method is used to start any processing
		  * after the service agent is set up
          *
          */
        virtual void start() = 0;

    private:
        // Disable copy constructor and assignment operator
        IServiceAgentUser( const IServiceAgentUser& theServiceAgentUser);
        IServiceAgentUser& operator=(const IServiceAgentUser &);
    };
}

#endif // !defined(ISERVICEAGENTUSER)
