/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/configuration_updates/src/IOnlineUpdatable.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class defines the interface that must be implemented
  * by GUIs that want to receive online updates
  */


#if !defined(IOnlineUpdatable_2FFD81A9_DA59_47d2_83B2_32B9F63FCF73__INCLUDED_)
#define IOnlineUpdatable_2FFD81A9_DA59_47d2_83B2_32B9F63FCF73__INCLUDED_

#include "core/configuration_updates/src/ConfigUpdateDetails.h"

namespace TA_Base_Core
{
    class IOnlineUpdatable
    {

    public:

        virtual ~IOnlineUpdatable() { };

        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        virtual void processUpdate(const ConfigUpdateDetails& updateEvent) =0;
    };
}

#endif // !defined(IOnlineUpdatable_2FFD81A9_DA59_47d2_83B2_32B9F63FCF73__INCLUDED_)
