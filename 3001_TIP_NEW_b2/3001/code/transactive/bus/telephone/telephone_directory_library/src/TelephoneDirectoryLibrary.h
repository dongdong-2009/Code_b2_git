/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Responsible for managing the retrieval and updates of the Telephone Directory
  * entries. It will communicate with the database through TelephoneDirectoryAcessFactory. 
  * The factory will create either TelephoneDirectory/TelephoneSubDirectory (read-only) or 
  * ConfigDirectory/ConfigSubDirectory(read-write) accordingly. 
  */

#if !defined(TelephoneDirectoryLibrary_807B6E42_E669_4fdb_A224_9DE93297DFF7__INCLUDED_)
#define TelephoneDirectoryLibrary_807B6E42_E669_4fdb_A224_9DE93297DFF7__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)


#include <string>
#include <vector>
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryItemStatus.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneSubDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneSubDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.h"

class TelephoneDirectoryLibrary
{

public:
	TelephoneDirectoryLibrary();

	virtual ~TelephoneDirectoryLibrary();

public:
	/**
	 * getAllEntries
	 * 
	 * Returns all Telephone Directory entries with "ACTIVE" status. 
     * It will converts the data return by the factory into TelephoneDirectoryEntryData
	 * 
	 * @return static std::vector<TelephoneDirectoryEntryData> 
     *
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static std::vector<TelephoneDirectoryEntryData> getAllEntries();


	/**
	 * getAllDirectories
	 * 
	 * Retrieves all "ACTIVE" sub directory records from the database
	 * 
	 * @return static std::vector<TelephoneDirectoryData> 
     *
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
    static std::vector<TelephoneDirectoryData> getAllDirectories();

	/**
	 * getAllDirectoryEntries
	 * 
     * Returns all "ACTIVE" TelephoneDirectoryEntryData items who's
	 * "directoryName" attribute matches the directoryName parameter.	 
	 * 
	 * @return std::vector<TelephoneDirectoryData> 
     *
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static std::vector<TelephoneDirectoryEntryData> getAllDirectoryEntries(const std::string& directoryName);

	/**
	 * getEntriesByName  
     *
     * Returns all "ACTIVE" TelephoneDirectoryEntryData entries with names
	 * containing the string specified by name.	 
	 * 
	 * @return std::vector<TelephoneDirectoryEntryData> 
     *
	 * @param : const std::string& name    
     *          Used as the search parameter to find entries with names containing the string
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static std::vector<TelephoneDirectoryEntryData> getEntriesByName(const std::string& name);

	/**
	 * getEntryByNumber  
     * Find TelephoneDirectoryEntryData with number matching the supplied value. 
     *
     * ASSERTION will be raised if the directory associated with the record cannot be found
     *
	 * @return TelephoneDirectoryEntryData*
     *         N.B. It is the responsibility of the *client* to delete the object return
     *
	 * @param : const std::string& number    
     *          Used as the search parameter to find an entry with matching number value
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static TelephoneDirectoryEntryData* getEntryByNumber(const std::string& number);

	/**
	 * addEntry  
     *
     * Add the TelephoneDirectoryEntryData entry to database. This will also
	 * utilize the CommsMessageSender class to send a sendBroadcastCommsMessage with a
	 * message type of TelephoneDirectoryEntryAddNotification. 
     *
     * ASSERTION will be raised on receiving DataConfigurationException.  This would imply that not all field data
     * has been set before the applyChanges() is called.
     *
	 * @return bool
     *         Return the status of the addition of the new entry to the database.
     *         If addition failed, status returned will be false, otherwise, it will be a true
     *
	 * @param : const TelephoneDirectoryEntryData& entry    
     *          TelephoneDirectoryDataEntry to to be added to the database
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static bool addEntry(const TelephoneDirectoryEntryData& entry);

	/**
	 * modifyEntry  
     *
     * Modify the data of the specified oldEntry to the values supplied
	 * by newEntry. Returns true if successful and false if unsuccessful. This will
	 * also utilize the CommsMessageSender class to send a sendBroadcastCommsMessage
	 * with a message type of TelephoneDirectoryEntryModifyNotification. 
	 * 
     * ASSERTION will be raised on receiving DataConfigurationException.  This would imply that not all field data
     * has been set before the applyChanges() is called.
     *
	 * @return bool
     *         Return the status of the modification of the entry to the database.
     *         If modification failed, status returned will be false, otherwise, it will be a true
     *
	 * @param : const TelephoneDirectoryEntryData& newEntry    
     *          TelephoneDirectoryDataEntry containing updated values (new data)
	 * @param : const TelephoneDirectoryEntryData& oldEntry    
     *          TelephoneDirectoryDataEntry to modify (original data)
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static bool modifyEntry(const TelephoneDirectoryEntryData& newEntry, const TelephoneDirectoryEntryData& oldEntry);

    /**
	 * deleteEntry  
     *
     * Delete the specified TelephoneDirectoryEntryData entry in the
	 * database.  Returns true if successfull and false if unsuccesful. This will also
	 * utilise the CommsMessageSender class to send a sendBroadcastCommsMessage with a
	 * message type of TelephoneDirectoryEntryDeleteNotification. 
     *
	 * @return bool
     *         Return the status of the deletion of the entry from the database.
     *         If deletion failed, status returned will be false, otherwise, it will be a true
     *
	 * @param : const TelephoneDirectoryEntryData& entry    
     *          TelephoneDirectoryDataEntry to delete
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */
	static bool deleteEntry(const TelephoneDirectoryEntryData& entry);

	/**
	 * addDirectory  
     *
     * Add the TelephoneDirectoryData entry to database. Returns true if
	 * successfull and false if unsuccesful. This will also utilise the
	 * CommsMessageSender class to send a sendBroadcastCommsMessage with a message
	 * type of TelephoneDirectoryAddNotification. Throws
	 * TelephoneDirectoryLibraryException if there is a failure to contact the
	 * database.
     *
     * ASSERTION will be raised on receiving DataConfigurationException.  This would imply that not all field data
     * has been set before the applyChanges() is called.
     *
	 * @return bool
     *         Return the status of the addition of the entry to the database.
     *
	 * @param : const TelephoneDirectoryData& directory    
     *          TelephoneDirectoryData to to be added 
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - Could not connect to the database or error executing the SQL query
	 */

	static bool addDirectory(const TelephoneDirectoryData& directory);

	/**
	 * modifyDirectory  
     * 
     * Modify the data of the specified oldDirectory to the value(s)
	 * supplied by newDirectory. Returns true if successful and false if unsuccessful.
	 * This will also utilise the CommsMessageSender class to send a
	 * sendBroadcastCommsMessage with a message type of
	 * TelephoneDirectoryModifyNotification. 
     *
	 * @return bool
     *         Return the status of the modification of the directory to the database.
     *
	 * @param : const TelephoneDirectoryData& newDirectory    
     *          TelephoneDirectoryData containing updated values (new data)
	 * @param : const TelephoneDirectoryData& oldDirectory    
     *          TelephoneDirectoryData to modify (old data)
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - could not connect to the database or error executing the SQL query
     *         - not all fields are for the record is set before calling applyChanges.
     *         - if sub-directory name is set to NULL, 
	 */
	static bool modifyDirectory(const TelephoneDirectoryData& newDirectory, const TelephoneDirectoryData& oldDirectory);

	/**
	 * deleteDirectory  
     * 
     * Delete the specified TelephoneDirectoryData entry in the
	 * database. The subdirectory can not be deleted if it contains any Contacts, i.e.
	 * any TelephoneDirectoryEntryData items with the directoryDataName attribute
	 * equal to the TelephoneDirectoryData entry's name attribute. Returns true if
	 * successfull and false if unsuccesful. This will also utilise the
	 * CommsMessageSender class to send a sendBroadcastCommsMessage with a message
	 * type of TelephoneDirectoryDeleteNotification. 
     *
	 * @return bool
     *         Return the status of the deletion of the directory to the database.
     *         If one or more directory entry data have a reference to the record, and 
     *         deleteThisObject is being called, it will return false as well
     *
	 * @param : const TelephoneDirectoryData& directory    
     *          TelephoneDirectoryData to be deleted
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - could not connect to the database or error executing the SQL query
	 */
	static bool deleteDirectory(const TelephoneDirectoryData& directory);

	/**
	 * searchDirectory  
     *
     * Returns all TelephoneDirectoryEntryData items who's "name"
	 * attribute contain a matching case insensitive sub-string to the provided search
	 * string. 
     *
	 * @return std::vector<TelephoneDirectoryEntryData>
     *         Return the status of the modification of the directory to the database.
     *         If one or more directory entry data have a reference to the record, and 
     *         deleteThisObject is being called, it will return false as well
     *
	 * @param : const std::string& directoryName    
     *          Name of the subdirectory to conduct the search on
	 * @param : const std::string& searchstr   
     *          String used to find matching substrings in TelephoneDirectoryDataEntry "name"'s
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - could not connect to the database or error executing the SQL query
	 */
	static std::vector<TelephoneDirectoryEntryData> searchDirectory(const std::string& directoryName, const std::string& searchstr);

	/**
	 * uploadDirectory
     *
     * Uploads the data from the data into the database and replace
	 * existing data. The data upload will follow the following process:
     * 1) Add all subdirectory information in directories to the database.
     *    If a existing directory in the database has the same directory name as the to-be-added directory, 
     *    this existing directory will be reused regardless of its current STATUS. This directory record's
     *    status will be updated to TELDIR_STATUS_NEW. All other newly added directories will have a 
     *    TELDIR_STATUS_NEW status as well
     * 2) Add all contact infomation in entries to the database. Status of the newly added contacts will
     *    be set to TELDIR_STATUS_NEW
     * => TelephoneDirectoryLibraryException will be thrown if there are any errors in Step (1) or (2).
     * => All subdirectory and entry record with TELDIR_STATUS_NEW status will be deleted
     * 3) Delete all TELDIR_STATUS_INACTIVE sub directory and directory entries records
     * 4) Change all TELDIR_STATUS_ACTIVE sub directory and directory entries records to TELDIR_STATUS_INACTIVE
     * 5) Change all TELDIR_STATUS_NEW sub directory and directory entries records to TELDIR_STATUS_ACTIVE
     * 6) Utilise the CommsMessageSender class to send a sendBroadcastCommsMessage with a message type of
	 *    TelephoneDirectoryUploadNotification.
     * 7) If this point is reached, the process was successful.
     *
	 * @param uploadFile	The path to the tab seperated volume file containing the data
	 * to be uploaded into the database
	 * 
	 * @return bool
     *         Return the status of the uploading of the sub directories and directory 
     *         entries to the database
     *
	 * @param : std::vector<TelephoneDirectoryData*>& directories    
     *          The vector containing all Subdirectories (TelephoneDirectoryData) to be uploaded
	 * @param : std::vector<TelephoneDirectoryEntryData*>& entries   
     *          The vector containing all Contacts (TelephoneDirectoryEntryData) to be uploaded
	 * 
     * @throws TelephoneDirectoryLibraryException 
     *         - if there is a problem retrieving data from the database
     *         - could not connect to the database or error executing the SQL query
     *         
	 */
	static bool uploadDirectory(const std::vector<TelephoneDirectoryData*>& directories, 
                                const std::vector<TelephoneDirectoryEntryData*>& entries);


	//libo++
	static TelephoneDirectoryEntryDataMap getEntryByNumberScope(const std::string& numberScope);
	//++libo

private:

	/**
	  * removeNewlyAddedDirectories
	  * 
	  * Internal helper function. 
      * Remove all sub-directories that have the status = TELDIR_STATUS_NEW
	  * 
	  * @return static void 
	  * 
	  * @exception <exceptions> Optional
	  */
    static void removeNewlyAddedDirectories();


	/**
	  * removeNewlyAddedEntries
	  * 
	  * Internal helper function. 
      * Remove all directory entries that have the status = TELDIR_STATUS_NEW
	  * 
	  * @return static void 
	  * 
	  * @exception <exceptions> Optional
	  */
    static void removeNewlyAddedEntries();


	/**
	 * restoreAmmendedDirectories
	 * 
	 * Internal helper function. 
     * Call applyChange() on all object in the vector list.
     * Used by uploadDirectory() to restore any ammended data if there are
     * any errors/exceptions during the uploading process
	 * 
	 * @return static void 
	 * @param : std::vector<TA_IRS_Core::ConfigTelephoneSubDirectory*>& backupDir
     *          
	 * 
     * @throws  TelephoneDirectoryLibraryException
     *          - if there is a problem establishing a connection with the database
     *          - if an error is encountered while retrieving data
     *          - if the data cannot be converted to the required format 
     *            (e.g. TEDENT_ID should be an unsigned long)
     *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
     *            constructed with an TelephoneDirectory.
     *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
     *          - if the ENTITYKEY references a non-existent ENTITY record. 
     *          - if the data contained in the TelephoneDirectory object is not sufficent to create 
     *            an entry in the database. 
	 */
    static void restoreAmmendedDirectories(std::vector<TA_IRS_Core::ConfigTelephoneSubDirectory*>& backupDir);

	/**
	 * restoreAmmendedDirectoryEntries
	 * 
	 * Internal helper function. 
     * Call applyChange() on all object in the vector list.
     * Used by uploadDirectory() to restore any ammended data if there are
     * any errors/exceptions during the uploading process
	 * 
	 * @return static void 
	 * @param : std::vector<TA_IRS_Core::ConfigTelephoneDirectory*>& backupDir
     *          
	 * 
     * @throws  TelephoneDirectoryLibraryException
	 */
    static void restoreAmmendedDirectoryEntries(std::vector<TA_IRS_Core::ConfigTelephoneDirectory*>& backupDirEntryList);

	/**
	  * cleanUpDirectories
	  * 
	  * Internal helper function.
      * Delete all pointers' content in the input list and subsequently clear the list
	  * 
	  * @return static void 
	  * @param : TA_IRS_Core::ITelephoneDirectories& dirList
      *          List to be clear
	  * 
	  * @exception <exceptions> Optional
	  */
    static void cleanUpDirectories(TA_IRS_Core::ITelephoneDirectories& dirList);


	/**
	  * cleanUpSubDirectories
	  * 
	  * Internal helper function.
      * Delete all pointers' content in the input list and subsequently clear the list
	  * 
	  * @return static void 
	  * @param : TA_IRS_Core::ITelephoneSubDirectories& subDirList
      *          List to be clear
	  * 
	  * @exception <exceptions> Optional
	  */
    static void cleanUpSubDirectories(TA_IRS_Core::ITelephoneSubDirectories& subDirList);


	/**
	 * getTelephoneDirectoriesWhere
	 * 
	 * Calls the TelephoneDirectoryAccessFactory getTelephoneDirectoriesWhere(..) method 
     * to retrieve a vector of directoy entry data and convert them into a vector of TelephoneDirectoryEntryData
     *
     * If a particular clause is not required, specify with a null for strings.  
     *
	 * @return static std::vector<TelephoneDirectoryEntryData> 
     *
	 * @param : const bool readWrite = false
	 * @param : const std::string& telephoneNum = ""
	 * @param : const std::string& fullName = ""
	 * @param : const std::string& dirname = ""
	 * @param : const TA_IRS_Core::TelephoneDirectoryItemStatus stateOfRecords = TA_IRS_Core::TELDIR_STATUS_ACTIVE
	 * 
     * @throws  TelephoneDirectoryLibraryException 
     *          - if there are any problems in communicating with the database.
     *            The possilble reasons are:
     *            1) Invalid connection string (as stored in RunParams)
     *            2) Database is not/cannot be opened
     *            3) Error while attempting to execute a database query
	 */
    static std::vector<TelephoneDirectoryEntryData> getTelephoneDirectoriesWhere(const bool readWrite = false,
                                    const std::string& telephoneNum = "",
                                    const std::string& fullName = "",
                                    const std::string& dirname = "",
                                    const TA_IRS_Core::TelephoneDirectoryItemStatus stateOfRecords = TA_IRS_Core::TELDIR_STATUS_ACTIVE);

    
};
#endif // !defined(TelephoneDirectoryLibrary_807B6E42_E669_4fdb_A224_9DE93297DFF7__INCLUDED_)
