/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioDirectoryMember.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * 
  */

#if !defined(IRadioDirectoryMember_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioDirectoryMember_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_


namespace TA_IRS_Core
{
    class IRadioDirectoryMember
    {
    public:

	    /**
	      * ~IRadioDirectoryMember
          * 
          * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioDirectoryMember(){};

	    /**
	      * getParent
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getParent() =0;

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
    };
}
#endif // !defined(IRadioDirectoryMember_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
