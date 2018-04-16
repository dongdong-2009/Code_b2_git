/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntry.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Class RadioDirectoryEntry is an implementation of the interface IRadioDirectoryEntry.
  * It holds the data specific to a RadioDirectoryEntry entry in the database, 
  * and allows read-only access to that data.
  */

#if !defined(AFX_RADIO_DIRECTORY_ENTRY_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define AFX_RADIO_DIRECTORY_ENTRY_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioDirectoryEntry.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntryHelper.h"


namespace TA_IRS_Core
{
    class RadioDirectoryEntry : public IRadioDirectoryEntry  
    {
    public:

	    /**
	      * RadioDirectoryEntry
	      * 
	      * Constructs a train object based on the primary key
	      * 
	      * @param : const unsigned long key
	      */
	    RadioDirectoryEntry(const unsigned long key);
        
	    /**
	      * RadioDirectoryEntry
	      * 
	      * Constructs a new RadioDirectoryEntry object using the information provided in IData interface
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      * 
	      * @see RadioDirectoryEntryHelper::RadioDirectoryEntryHelper(const unsigned long, TA_Base_Core::IData&);
	      */
        RadioDirectoryEntry(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * ~RadioDirectoryEntry
	      * 
	      * Destructor 
	      */
	    virtual ~RadioDirectoryEntry();

        //
        // IRadioDirectoryEntry methods
        //
	    /**
	      * getOwner
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOwner();

	    /**
	      * getCategory
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getCategory();


	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      */
        virtual unsigned long getKey();

        virtual std::string getName();

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

	    // disabled methods
        RadioDirectoryEntry(const RadioDirectoryEntry&);
    	RadioDirectoryEntry& operator=(const RadioDirectoryEntry&);

    private:

        RadioDirectoryEntryHelper m_radioDirectoryHelper;
    };
}
#endif // !defined(AFX_RADIO_DIRECTORY_ENTRY_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
