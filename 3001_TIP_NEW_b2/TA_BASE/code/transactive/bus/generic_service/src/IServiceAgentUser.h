/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_service/src/IServiceAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #2 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
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
