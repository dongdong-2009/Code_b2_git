/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class uses the Data Access Interface to retrieve MMS Scheduling information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#if !defined(MmsSchedulingDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define MmsSchedulingDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>


// Forward declarations
namespace TA_Core
{
    class IConfigMmScheduling;
}


namespace TA_IRS_App
{
    class MmsSchedulingDatabaseAccessor
    {

    public:

        // This is a map of IConfigMmScheduling* objects stored by their key (unsigned long)
        typedef std::map<unsigned long, TA_IRS_Core::IConfigMmScheduling*> LoadedMmsScheduling;

        struct ProfileDetails
        {
            std::string name;      // The name of the profile
            unsigned long typeKey; // Indicates whether the profile is a central or local type
        };


        /**
         * Constructor
         */
        MmsSchedulingDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~MmsSchedulingDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the rights from the database and store them in m_appliationMmsScheduling
         * and m_subsystemMmsScheduling.
         *
         * @exception DataException - This will be thrown if there is an error with the rights data and it
         *                             cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();
       
        
        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its key.
         * Precondition: loadItems() must be called before this method
         *
         * @param unsigned long - The identifier of the item to retrieve
         *
         * @return IConfigMmScheduling* - The database object for this item. This will be NULL if one
         *                            is not found.
         */
        virtual TA_IRS_Core::IConfigMmScheduling* getItem(unsigned long identifier);


        /**
         * areCurrentChangesPending
         *
         * This is called to determine if there are any current changes that have not been applied.
         * If this returns true then it will populate the vector of strings passed in with all items
         * not applied.
         * 
         * @param vector<string>& - This is an OUT parameter and should be passed in empty. It will
         *                          be populated with names for all data not yet applied
         * 
         * @return bool - This will return true if there are items that have had changes which have  not
         *                been applied.
         */
         bool areCurrentChangesPending(std::vector<std::string>& rightsNotApplied);

        
         /**
          * invalidate
          *
          * This will set invalidate the access factory, so
          * that next time load is called all data should
          * be refreshed from the database.
          */
         void invalidateData();


        /**
         * getAllSchedules
         *
         * This returns all duties and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the duty keys and names.
         */
        ScheduleMap getAllSchedules();


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        MmsSchedulingDatabaseAccessor( const MmsSchedulingDatabaseAccessor& theMmsSchedulingDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        MmsSchedulingDatabaseAccessor& operator=(const MmsSchedulingDatabaseAccessor& pm);

        ScheduleMap m_scheduleMap;
    };
}

#endif // !defined(MmsSchedulingDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
