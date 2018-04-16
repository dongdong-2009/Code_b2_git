/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioSubscriber.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * 
  */

#if !defined(IRadioSubscriber_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioSubscriber_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include <string>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IRadioSubscriber : public virtual TA_Base_Core::IItem
    {
    public:

	    /**
	      * ~IRadioSubscriber
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioSubscriber(){};

	    /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey() =0;

	    /**
	      * getResourceType
	      * 
	      * @return char
	      */
	    virtual char getResourceType() =0;

	    /**
	      * getResourceId
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getResourceId() =0;

	    /**
	      * getSSI
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getSSI() =0;

	    /**
	      * getAlias
	      * 
	      * @return std::string
	      */
	    virtual std::string getAlias() =0;


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };
}
#endif // !defined(IRadioSubscriber_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
