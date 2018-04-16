/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMember.h $
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

#if !defined(AFX_RADIO_DIRECTORY_MEMBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define AFX_RADIO_DIRECTORY_MEMBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioDirectoryMember.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h"


namespace TA_IRS_Core
{
    class RadioDirectoryMember : public IRadioDirectoryMember  
    {
    public:

	    /**
	      * RadioDirectoryMember
	      * 
	      * Constructs a train object based on the primary key
	      * 
	      * @param : const unsigned long key
	      */
	    RadioDirectoryMember(const unsigned long key);
        
	    /**
	      * RadioDirectoryMember
	      * 
	      * Constructs a new RadioDirectoryMember object using the information provided in IData interface
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      * 
	      * @see RadioDirectoryMemberHelper::RadioDirectoryMemberHelper(const unsigned long, TA_Base_Core::IData&);
	      */
        RadioDirectoryMember(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * ~RadioDirectoryMember
	      * 
	      * Destructor 
	      */
	    virtual ~RadioDirectoryMember();

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

    private:

	    /**
	      * RadioDirectoryMember
	      * 
	      * Copy constructor (disabled).
          * If a copy constructor is required care should be taken. The copy constructor to 
          * the RadioDirectoryMemberHelper has only been written for ConfigRadioDirectoryMember objects and will not copy 
          * the RadioDirectoryMember helper for a read-only object (ie this one).
	      * 
	      * @param : const RadioDirectoryMember&
	      */
        RadioDirectoryMember(const RadioDirectoryMember&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return RadioDirectoryMember& 
	      * @param : const RadioDirectoryMember &
	      */
		RadioDirectoryMember& operator=(const RadioDirectoryMember&);

    private:

        RadioDirectoryMemberHelper m_radioDirectoryHelper;
    };
}
#endif // !defined(AFX_RADIO_DIRECTORY_MEMBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
