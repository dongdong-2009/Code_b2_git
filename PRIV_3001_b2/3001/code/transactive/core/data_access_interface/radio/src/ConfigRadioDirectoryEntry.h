/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioDirectoryEntry.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(AFX_CONFIGRADIODIRECTORYENTRY_H__B450BEF2_A6F9_4803_A74D_BD03BF411018__INCLUDED_)
#define AFX_CONFIGRADIODIRECTORYENTRY_H__B450BEF2_A6F9_4803_A74D_BD03BF411018__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioDirectoryEntry.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntryHelper.h"

namespace TA_IRS_Core
{
    class ConfigRadioDirectoryEntry : public IRadioDirectoryEntry
    {
    public:
	    ConfigRadioDirectoryEntry(const unsigned long key);
        ConfigRadioDirectoryEntry(const unsigned long row, TA_Base_Core::IData& data);
        ConfigRadioDirectoryEntry(unsigned long owner, unsigned long category, std::string name);

        virtual ~ConfigRadioDirectoryEntry();

        //
        // IRadioDirectoryEntry methods
        //
        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey();

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
	      * getName
	      * 
	      * @return std::string
	      */
	    virtual std::string getName();

        virtual void invalidate();
        virtual void save();


    private:
        // disabled methods
        ConfigRadioDirectoryEntry();
        ConfigRadioDirectoryEntry(const ConfigRadioDirectoryEntry&);
		ConfigRadioDirectoryEntry& operator=(const ConfigRadioDirectoryEntry&);

        RadioDirectoryEntryHelper m_radioDirectoryHelper;
    };
}

#endif // !defined(AFX_CONFIGRADIODIRECTORYENTRY_H__B450BEF2_A6F9_4803_A74D_BD03BF411018__INCLUDED_)

