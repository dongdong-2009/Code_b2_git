/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfg.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Used to persist the settings of the profile resource buttons 
  * in the radio status panel of the RadioManager
  */

#if !defined(RadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define RadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include "core/data_access_interface/radio/src/IRadioBtnCfg.h"
#include "core/data_access_interface/radio/src/RadioBtnCfgHelper.h"


namespace TA_IRS_Core
{
    class RadioBtnCfg : public virtual IRadioBtnCfg
    {
    public:

	    /**
	      * RadioBtnCfg
          *
          * Constructor
	      * 
	      * @return 
	      * @param : const unsigned long key
	      */
        RadioBtnCfg(unsigned long key);

	    /**
	      * RadioBtnCfg
	      * 
	      * 
	      * @return 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      */
        RadioBtnCfg(unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * ~RadioBtnCfg
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~RadioBtnCfg();

        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey();

	    /**
	      * getProfile
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperatorProfile();

        virtual bool isProfile();
        
	    /**
	      * getButtonRef
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getButtonRef();

	    /**
	      * getResourceType
	      * 
	      * @return char
	      */
	    virtual char getResourceType();

	    /**
	      * getResourceId
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getResourceId();


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        //Disabled Methods
        RadioBtnCfg(const RadioBtnCfg&);
        RadioBtnCfg& operator=(const RadioBtnCfg&);

    private:
        RadioBtnCfgHelper m_helper;

    };
}
#endif // !defined(RadioBtnCfg_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
