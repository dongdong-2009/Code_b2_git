/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtility.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class will perform the upload of Telephone Directory data from an external
  * source (typically a file) into the database. This class will utilise the
  * TelephoneDirectoryLibrary class to perform the database manipulation functions.
  */

#if !defined(TelephoneDirectoryUploadUtility_0AA39060_97F0_45e4_956D_7995AA8A7FCF__INCLUDED_)
#define TelephoneDirectoryUploadUtility_0AA39060_97F0_45e4_956D_7995AA8A7FCF__INCLUDED_

#include <string>
#include <vector>

struct TelephoneDirectoryData;
struct TelephoneDirectoryEntryData;

class TelephoneDirectoryUploadUtility
{

public:
    TelephoneDirectoryUploadUtility(const std::string& sessionId);
	virtual ~TelephoneDirectoryUploadUtility();

public:
	/**
	 * @param uploadFileName    The pathname of the Tab Separated Volume to use for
	 * the upload
	 * 
	 */

	/**
	  * TelephoneDirectoryUploadUtility
	  * 
	  * Constructor accepting a filename to open. The filename will be stored in
	  * m_uploadFileName , and a valid file handle will be obtained. If the file can
	  * not be opened an exception must be thrown.
	  * 
	  * @return 
	  * @param : std::string& uploadFileName
      *          The pathname of the Tab Separated Volume to use for the upload
	  * @param : const std::string& sessionId
	  * 
	  * @throws TelephoneDirectoryUploadUtilityException when there is an error opening file
	  */
    TelephoneDirectoryUploadUtility(std::string& uploadFileName, const std::string& sessionId);

	/**
	  * setUploadFile
	  * 
	  * Open the upload file
	  * 
	  * @return void 
	  * @param : std::string& uploadFileName
      *          The pathname of the Tab Separated Volume to use for the upload
	  * 
	  * @throws TelephoneDirectoryUploadUtilityException when there is an error opening file
	  */
	void setUploadFile(std::string& uploadFileName);

	/**
	  * processUploadFromFile
	  * 
	  * Public interface to trigger the upload process.
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
	bool processUploadFromFile();

protected:

	/**
	 * Pathname of the Tab Separated Volume to use for the upload.
	 */
	std::string m_uploadFileName;

	/**
	 * Telephone subdirectories to be uploaded.
	 */
	std::vector<TelephoneDirectoryData*> m_directories;

	/**
	 * Telephone contacts to be uploaded.
	 */
	std::vector<TelephoneDirectoryEntryData*> m_entries;

	CFile m_FileHandle;

	/**
	  * openUploadFile
	  * 
	  * Opens the disk file m_uploadFileName and obtain a valid file handle. If the
	  * file can not be opened an exception must be thrown.
	  * 
	  * @return 
	  * 
	  * @throws TelephoneDirectoryUploadUtilityException when there is an error opening file
	  */
	void openUploadFile();

	/**
	  * parseUploadFile
	  * 
	  * Parses the upload file and populates the TelephoneDirectoryData and
	  * TelephoneDirectoryEntryData vectors with the information to be  uploaded.
	  * Exceptions raised from file handling procedures will be handled here. The file
	  * format will be a tab seperated volume each line containing the  information for
	  * one TelephoneDirectoryEntryData. PLEASE NOTE: The  TelephoneDirectoryData items
	  * are not explicitly listed in the file, thus one TelephoneDirectoryData
	  * structure will be created for each unique directory name occurring in the file.
	  * The following mapping between the data in the upload file  and the
	  * TelephoneDirectoryData/TelephoneDirectoryEntryData will be used to  populate
	  * these data structures: Upload file data		-		
            Data structure member Extension	- TelephoneDirectoryEntryData:number 
            Last Name -  TelephoneDirectoryEntryData:lastName 
            First Name - TelephoneDirectoryEntryData:firstName 
            Directory Link - TelephoneDirectoryEntryData:directoryDataName 
                             TelephoneDirectoryData:name
	        Cost Center	- TelephoneDirectoryEntryData:location
      * Will return true if successful or false if unsuccessful
      *
	  * @return	bool 
      *         Will return true if successful or false if unsuccessful.
	  * 
	  * @exception <exceptions> Optional
	  */
	bool parseUploadFile();

	/**
	  * uploadDataToDatabase
	  * 
	  * Upload the TelephoneDirectoryData and TelephoneDirectoryEntryData vectors to
	  * the database. This will utilise the uploadDirectory method of the
	  * TelephoneDirectoryLibrary to perform the database functionality.
      * Will return true if successful or false if unsuccessful
      *
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
	bool uploadDataToDatabase();


	/**
	  * processRecord
	  * 
	  * Helper function that breakdown the record into the respective fields
	  * 
	  * @return bool 
	  * @param : const std::string& input
	  * 
	  * @exception <exceptions> Optional
	  */
    bool processRecord(const std::string& input);


	/**
	  * cleanUp
	  * 
	  * Remove any memory allocated by this class
	  * 
	  * @return void 
	  * 
	  * @exception <exceptions> Optional
	  */
    void cleanUp();


    /**
	  * Stores the session ID
      */
    std::string m_SessionId;

    static const unsigned short MAX_BUFFER_SIZE;
	// TD 16047
    static const unsigned short LINES_TO_SKIP;
    static const unsigned short LENGTH_OF_LOCATIONNAME;
    static const unsigned short LENGTH_OF_DIRECTORYNAME;
    static const std::string START_PROCESS_CHAR;
	// TD 16047

private:
    TelephoneDirectoryUploadUtility();


};
#endif // !defined(TelephoneDirectoryUploadUtility_0AA39060_97F0_45e4_956D_7995AA8A7FCF__INCLUDED_)
