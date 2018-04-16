/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/IRadioBtnCfg.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Used to persist the settings of the profile resource buttons 
  * in the radio status panel of the RadioManager
  */

#if !defined(IRadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define IRadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_


namespace TA_IRS_Core
{
    class IRadioBtnCfg 
    {
    public:

	    /**
	      * ~IRadioBtnCfg
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~IRadioBtnCfg(){};

        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey() =0;

	    /**
	      * getOperatorProfile
	      * 
	      * @return the key of operator / profile
	      */
	    virtual unsigned long getOperatorProfile() =0;

        virtual bool isProfile() = 0;

	    /**
	      * getButtonRef
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getButtonRef() =0;

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
#endif // !defined(IRadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
