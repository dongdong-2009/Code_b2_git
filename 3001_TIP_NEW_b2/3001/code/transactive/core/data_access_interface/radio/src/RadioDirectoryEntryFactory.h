/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntryFactory.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is the access factory used to load/create radio textmessage data.
  */

#if !defined(RadioDirectoryEntryFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define RadioDirectoryEntryFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include <string>

// forward declarations
namespace TA_IRS_Core
{
    class IRadioDirectoryEntry;
}


namespace TA_IRS_Core
{
    class RadioDirectoryEntryFactory
    {
    public:

	    /**
	      * ~RadioDirectoryEntryFactory
	      * 
	      * Destructor
	      */
	    virtual ~RadioDirectoryEntryFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioDirectoryEntryFactory& 
	      */
	    static RadioDirectoryEntryFactory& getInstance();

	    /**
	      * removeInstance
	      * 
	      * Removes the instance of the class (if already created) and cleans up the members.  
          * Primarily used upon program termination (e.g. from main()) so that Purify does not 
          * consider this class and its members to be memory leaks.
	      * 
	      * @return void 
	      */
        static void removeInstance();

	    /**
	      * getRadioDirectoryEntries
	      * 
	      * get the collection of directory entries for the current operator for the specified category
	      */
        std::vector<IRadioDirectoryEntry*> getRadioDirectoryEntries(unsigned long category);
        
	    /**
	      * getRadioDirectoryEntry
	      * 
	      * get the specific directory entry
	      */
        IRadioDirectoryEntry* getRadioDirectoryEntry(unsigned long category, const std::string& name);

		IRadioDirectoryEntry* modifyDirectory(unsigned long category, std::string name);
		
		void deleteDirectory(unsigned long category, std::string name);
        
    private:

        // disabled methods
        RadioDirectoryEntryFactory();
		RadioDirectoryEntryFactory(const RadioDirectoryEntryFactory&);
		RadioDirectoryEntryFactory& operator=(const RadioDirectoryEntryFactory&);

        unsigned long getOperatorId();

    private:
        
	    static RadioDirectoryEntryFactory* m_instance;
    };
}
#endif
