/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/ILibraryVersionListener.h $
  * @author:  Kent Yip
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/04/19 20:02:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is used to keep track of the TTIS predefined message library
  * version at each location
  *
  */

#ifndef LIBRARYVERSIONLISTENER_H
#define LIBRARYVERSIONLISTENER_H

#include "bus/signs/tis_agent_access/src/LibraryVersionMonitor.h"

namespace TA_IRS_Bus
{
    /**
     * Listener for an updates to library versions.
     */
    class ILibraryVersionListener
    {
    public:
        /**
         * Destructor to ensure that any deleted listeners are deregistered
         * correctly.
         */

        /**
         * Observer method to be implemented by derived classes.  Will be
         * invoked a library version has been updated.
         * @param libraryVersion The library version data.
         */
        virtual void libraryVersionUpdate( LibraryVersionUpdateInfo libraryVersion ) = 0;
    };


} // namespace TA_IRS_Bus

#endif // LIBRARYVERSIONLISTENER_H