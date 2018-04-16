/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Simply a placeholder structure for a Telehpone Directory
  */


#if !defined(TelephoneDirectoryData_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_)
#define TelephoneDirectoryData_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_

#include <string>
#include <vector>

struct TelephoneDirectoryData
{
	
    
public:
    /**
     * ID of the entry. This will be used as the primary key lookup. If this a
     * new entry, this field must be left blank, and the new ID will be received
     * via the comms message updated once it has successfully been added to
     * the database.
     */
    unsigned long ID;

	/**
	 * Name of the Directory. Must be unique.
	 */
	std::string name;

    /**
	 * Copy contructor
	 * @param directory
	 */
    TelephoneDirectoryData();
    TelephoneDirectoryData (const TelephoneDirectoryData &Directory);
    TelephoneDirectoryData& operator=(const TelephoneDirectoryData &Directory);

};
#endif // !defined(TelephoneDirectoryData_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_)