/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioDirectoryEntry.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Used to store trans Active managed(i.e. user-configurable) radio group
  * related resources for the Radio Manager
  */

#if !defined(IRadioDirectoryEntry_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioDirectoryEntry_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include <string>

namespace TA_IRS_Core
{
    class IRadioDirectoryEntry
    {
    public:

	    /**
	      * ~IRadioDirectoryEntry
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioDirectoryEntry(){};
        
	    /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey() =0;

	    /**
	      * getOwner
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOwner() =0;

	    /**
	      * getCategory
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getCategory() =0;

        virtual std::string getName() = 0;

    };
}
#endif // !defined(IRadioDirectoryEntry_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
