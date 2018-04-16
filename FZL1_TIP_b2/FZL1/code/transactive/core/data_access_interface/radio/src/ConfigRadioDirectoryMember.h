/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioDirectoryMember.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Class RadioDirectoryMember is an implementation of the interface IRadioDirectoryMember.
  * It holds the data specific to a RadioDirectoryMember entry in the database, 
  * and allows read-only access to that data.
  */

#if !defined(AFX_CFGRADIO_DIRECTORY_MEMBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define AFX_CFGRADIO_DIRECTORY_MEMBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioDirectoryMember.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h"


namespace TA_IRS_Core
{
    class ConfigRadioDirectoryMember : public IRadioDirectoryMember  
    {
    public:

	    ConfigRadioDirectoryMember(const unsigned long key);
        
        ConfigRadioDirectoryMember(const unsigned long row, TA_Base_Core::IData& data);
        ConfigRadioDirectoryMember(unsigned long parent, char resource_type, unsigned long resource_id);
        
	    virtual ~ConfigRadioDirectoryMember();

        //
        // IRadioDirectoryMember methods
        //
	    /**
	      * getParent
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getParent();

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
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        virtual unsigned long getKey();


	    /**
	      * invalidate
	      * 
	      * Make the data contained by this item as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
	      * 
	      * @return void 
	      */
        virtual void invalidate();

        virtual void save();

    private:

        // disabled methods
        ConfigRadioDirectoryMember();
        ConfigRadioDirectoryMember(const ConfigRadioDirectoryMember&);
		ConfigRadioDirectoryMember& operator=(const ConfigRadioDirectoryMember&);

    private:

        RadioDirectoryMemberHelper m_radioDirectoryHelper;
    };
}
#endif 
