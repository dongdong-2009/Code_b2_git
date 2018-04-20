/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/tis_agent_access/src/ILibraryVersionListener.h $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
     * The structure that will have the library version information for a
     * particular station specified by the entity key.
     */
	typedef struct
	{
        /** The datapoint storing the value. */
		unsigned long entityKey;
        /** The library version number. */
		unsigned short value;
	} LibraryVersionInfo;

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
        virtual void libraryVersionUpdate( LibraryVersionInfo libraryVersion ) = 0;
    };


} // namespace TA_IRS_Bus

#endif // LIBRARYVERSIONLISTENER_H