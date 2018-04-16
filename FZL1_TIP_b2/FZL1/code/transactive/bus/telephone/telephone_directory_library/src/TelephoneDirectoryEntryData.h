/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Simply a structure to hold a Telephone Directory Entry's data
  */

#if !defined(TelephoneDirectoryEntryData_6CA33D6F_7FB8_45f7_AED7_D9AFA218CD52__INCLUDED_)
#define TelephoneDirectoryEntryData_6CA33D6F_7FB8_45f7_AED7_D9AFA218CD52__INCLUDED_

#include <string>
#include <map>

struct TelephoneDirectoryEntryData
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
	 * Number of the entry. This will be used as the primary key lookup and must be
	 * unique
	 */
	std::string number;

	/**
	 * Full name of the entry
	 */
    std::string fullName;

	/**
	 * Optional parameter indicating the location the entry is located at.
	 */
	std::string location;

	/**
	 * The name of the Directory the entry belongs to. Must correspond to a valid
	 * DirectoryData item
	 */
	std::string directoryDataName;

public:
	/**
	 * Copy constructor
	 * @param directoryEntry
	 * 
	 */
	
    TelephoneDirectoryEntryData();
    TelephoneDirectoryEntryData (const TelephoneDirectoryEntryData & directoryEntry);
    TelephoneDirectoryEntryData& operator=(const TelephoneDirectoryEntryData & directoryEntry);

};
//libo
typedef std::map<std::string, TelephoneDirectoryEntryData*> TelephoneDirectoryEntryDataMap;
#endif // !defined(TelephoneDirectoryEntryData_6CA33D6F_7FB8_45f7_AED7_D9AFA218CD52__INCLUDED_)
