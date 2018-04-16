#ifndef IPID_GROUP_H_INCLUDED
#define IPID_GROUP_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/tis_agent/src/IPidGroup.h $
  * @author Robin Ashcroft
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * This interface represents a named group of PID's. 
  */


#include <vector>
#include <string>

namespace TA_IRS_Core
{


    class IPidGroup
    {
    public:


        /**
         * ~IPidGroup
         *
         * Standard destructor.
         */
        virtual ~IPidGroup() {};


        /**
         * getName
         *
         * @return the name of the group
		 *
         */
        virtual std::string getName() const = 0;


        /**
         * getPidNames
         *
         * @return  a vector of the full entity names of the PIDs in this group
		 *
         */
        virtual const std::vector<std::string> getPidNames() const = 0;


    };

} //end namespace TA_IRS_Core

#endif 
