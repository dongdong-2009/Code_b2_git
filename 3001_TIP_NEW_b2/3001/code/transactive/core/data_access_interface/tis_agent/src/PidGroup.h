#ifndef PID_GROUP_H_INCLUDED
#define PID_GROUP_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/tis_agent/src/PidGroup.h $
  * @author christiandw
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * This immutable class represents a named group of PID's. It impliments the the IPidGroup interface.
  * Note that this class is little more than a struct, because the database interaction takes
  * place in the PidGroupsAccessFactory class
  */

#include "core/data_access_interface/tis_agent/src/IPidGroup.h"


namespace TA_IRS_Core
{
    class PidGroup : public IPidGroup
    {
    public:

        /**
         * PidGroup
         *
         * Standard constructor.
         *
         * @param name: the name of the group
		 * @param pidNames: 
         */
        PidGroup( const std::string& name,
				  std::vector<std::string> pidNames);


        /**
         * ~PidGroup
         *
         * Standard destructor.
         */
        virtual ~PidGroup();

        /**
         * getName
         *
         * @return the name of the group
		 *
         */
        virtual std::string getName() const;


        /**
         * getPidNames
         *
         * @return  a vector of the full entity names of the PIDs in this group
		 *
         */
        virtual const std::vector<std::string> getPidNames() const;

    private: //data

		const std::string m_name;
		const std::vector<std::string> m_pidNames;

    private: //methods
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        PidGroup& operator=( const PidGroup& );
        PidGroup( const PidGroup& );


    };

} //end namespace TA_IRS_Core

#endif 
