/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMemberFactory.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  */

#if !defined(RadioDirectoryMemberFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define RadioDirectoryMemberFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include <string>

// forward declarations
namespace TA_IRS_Core
{
    class IRadioDirectoryMember;
}


namespace TA_IRS_Core
{
    class RadioDirectoryMemberFactory
    {

    public:

	    /**
	      * ~RadioDirectoryMemberFactory
	      * 
	      * Destructor
	      */
	    virtual ~RadioDirectoryMemberFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioDirectoryMemberFactory& 
	      */
	    static RadioDirectoryMemberFactory& getInstance();

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


        std::vector<IRadioDirectoryMember*> getRadioDirectoryMembers(unsigned long parentKey);
        
		IRadioDirectoryMember* getRadioDirectoryMember(unsigned long key, bool readWrite = false);


        IRadioDirectoryMember* createNew(unsigned long parent, char resource_type, unsigned long resource_id);

		void deleteMembers(char resource_type, unsigned long resource_id);

		void updateMemberIds(char resource_type, unsigned long old_resource_id, unsigned long new_resource_id);
        
    private:

        // disabled methods
        RadioDirectoryMemberFactory();
		RadioDirectoryMemberFactory(const RadioDirectoryMemberFactory&);
		RadioDirectoryMemberFactory& operator=(const RadioDirectoryMemberFactory&);
		
		std::string getLocalDatabaseName();

    private:
        
	    static RadioDirectoryMemberFactory* m_instance;
        
    };
}
#endif

