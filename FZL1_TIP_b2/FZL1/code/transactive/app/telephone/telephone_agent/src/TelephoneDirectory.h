#if !defined(TELEPHONEDIRECTORY_H)
#define TELEPHONESDIRCTORY_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_agent/src/TelephoneDirectory.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 
  */
#include "app/telephone/telephone_agent/IDL/src/IDirectoryCorbaDef.h"
#include "core/corba/src/ServantBase.h"

namespace TA_App
{
	class TelephoneDirectory : public virtual POA_TA_App::IDirectoryCorbaDef, public virtual TA_Core::ServantBase
	{
	public:
		/**
		  * TelephoneDirectory
		  * 
		  * This is the default constructor for the telephone directories
		  * 
		  * @param		The name for this directory
		  * @param		The type of directory
		  * 
		  * @return		nothing
		  */
		TelephoneDirectory(std::string name, TA_App::IDirectoryCorbaDef::EDirectoryType type);
		/**
		  * ~TelephoneDirectory
		  * 
		  * This is the default destructor 
		  * 
		  * @return		nothing
		  */
		~TelephoneDirectory();

		/**
		  * getDirectoryData
		  * 
		  * This method returns the directory data for this directory
		  * 
		  * @return		TA_App::IDirectoryCorbaDef::DirectoryData*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		TA_App::IDirectoryCorbaDef::DirectoryData* getDirectoryData(const char* sessionId);
		/**
		  * getAllEntries
		  * 
		  * This method returns all entries in this directory
		  * 
		  * @return		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* getAllEntries(const char* sessionId);
		/**
		  * getEntriesByName
		  * 
		  * This method returns all entries in the directory that have the given name as a substring
		  * 
		  * @return		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence*
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The name to look for
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* getEntriesByName(const char* sessionId, const char* name);
		/**
		  * getEntryByNumber
		  * 
		  * This method returns the entry for the spcified number
		  * 
		  * @return		TA_App::IDirectoryCorbaDef::DirectoryEntryData*
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The number for which to return the entry
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		TA_App::IDirectoryCorbaDef::DirectoryEntryData* getEntryByNumber(const char* sessionId, const char* number);
		/**
		  * addEntry
		  * 
		  * This method adds a new entry to this directory
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The new entry to add
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		void addEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& newEntry);
		/**
		  * modifyEntry
		  * 
		  * This method modifies an entry in this directory
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The new entry to modify
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		void modifyEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& modifiedEntry);
		/**
		  * deleteEntry
		  * 
		  * This method deletes an entry from this directory
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The new entry to delete
		  *
		  * @exception	TA_App::IDirectoryCorbaDef::DirectoryException
		  */
		void deleteEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& deletedEntry);
    
	private:
		std::vector<TA_App::IDirectoryCorbaDef::DirectoryEntryData*> m_entries; // the vector of entries within this directory
		TA_App::IDirectoryCorbaDef::DirectoryData m_data; // the data for this directory
	};
}


#endif // !defined(TELEPHONESDIRCTORY_H)
