#ifndef IPID_GROUP_H_INCLUDED
#define IPID_GROUP_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/IPidGroup.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
