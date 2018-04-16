/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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