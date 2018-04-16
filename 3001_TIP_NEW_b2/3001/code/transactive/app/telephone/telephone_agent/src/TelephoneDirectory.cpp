/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_agent/src/TelephoneDirectory.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the implementation for the TelephoneDirectory class
  */

#include "TelephoneDirectory.h"

namespace TA_App
{
	/**
	  * TelephoneDirectory
	  */
	TelephoneDirectory::TelephoneDirectory(std::string name, TA_App::IDirectoryCorbaDef::EDirectoryType type)
	{
		//set the data
		m_data.name = CORBA::string_dup(name.c_str());
		m_data.type = type;
		//activate this directory as a CORBA servant
		activateServant();
	}

	/**
	  * ~TelephoneDirectory
	  */
	TelephoneDirectory::~TelephoneDirectory()
	{
		//clean up
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//delete each entry
			delete m_entries[i];
		}
		//deactivate this directory as a CORBA servant
		deactivateServant();
	}

	/**
	  * getDirectoryData
	  */
	TA_App::IDirectoryCorbaDef::DirectoryData* TelephoneDirectory::getDirectoryData(const char* sessionId)
	{
		//prepare the return data
		TA_App::IDirectoryCorbaDef::DirectoryData* data = new TA_App::IDirectoryCorbaDef::DirectoryData;
		//copy the values
		data->name = CORBA::string_dup(m_data.name);
		data->type = m_data.type;
		//return the data
		return data;
	}

	/**
	  * getAllEntries
	  */
	TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* TelephoneDirectory::getAllEntries(const char* sessionId)
	{
		//prepare the return sequence
		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* sequence = new TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence;
		//for each entry
		for (unsigned int i(0); i < m_entries.size(); i ++)
		{
			//create a new entry
			TA_App::IDirectoryCorbaDef::DirectoryEntryData* entry = new TA_App::IDirectoryCorbaDef::DirectoryEntryData;
			//copy all entry data
			entry->name		= CORBA::string_dup(m_entries[i]->name);
			entry->number	= CORBA::string_dup(m_entries[i]->number);
			entry->type		= m_entries[i]->type;
			entry->location = m_entries[i]->location;
			//add it to the sequence
			(*sequence)[i] = *entry;
		}
		//return the sequence
		return sequence;
	}

	/**
	  * getEntriesByName
	  */
	TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* TelephoneDirectory::getEntriesByName(const char* sessionId, const char* name)
	{
		//prepare a return sequence
		TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence* sequence = new TA_App::IDirectoryCorbaDef::DirectoryEntryDataSequence;
		//initiate the count, this will be used to keep track of the entry that we're adding to the return sequence
		unsigned int count(0);
		//for each entry
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//check the name for a substring
			if (strstr(m_entries[i]->name,name) != NULL)
			{
				//create a new entry
				TA_App::IDirectoryCorbaDef::DirectoryEntryData* entry = new TA_App::IDirectoryCorbaDef::DirectoryEntryData;
				//copy all the data
				entry->name		= CORBA::string_dup(m_entries[i]->name);
				entry->number	= CORBA::string_dup(m_entries[i]->number);
				entry->type		= m_entries[i]->type;
				entry->location = m_entries[i]->location;
				//increment the length of the return sequence
				sequence->length(count+1);
				//copy the entry into the return sequence
				(*sequence)[count] = *entry;
				//increment the count
				count++;
			}
		}
		//return the sequence
		return sequence;
	}

	/**
	  * getEntryByNumber
	  */
	TA_App::IDirectoryCorbaDef::DirectoryEntryData* TelephoneDirectory::getEntryByNumber(const char* sessionId, const char* number)
	{
		//for each entry
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//check the number
			if (!strcmp(m_entries[i]->number,number))
			{
				//we've found it
				//create a new entry
				TA_App::IDirectoryCorbaDef::DirectoryEntryData* entry = new TA_App::IDirectoryCorbaDef::DirectoryEntryData;
				//copy the data
				entry->name		= CORBA::string_dup(m_entries[i]->name);
				entry->number	= CORBA::string_dup(m_entries[i]->number);
				entry->type		= m_entries[i]->type;
				entry->location = m_entries[i]->location;
				//return the entry
				return entry;
			}
		}
		//we didn't find an entry with this number
		return NULL;
	}

	/**
	  * addEntry
	  */
	void TelephoneDirectory::addEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& newEntry)
	{
		//check if the entry doesn't already exist in this directory
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//check the number
			if (!strcmp(m_entries[i]->number,newEntry.number))
			{
				//already exists in this directory
				//throw an exception
				//TODO
				return;
			}
		}
		//if we get here we can safely add it to our directory
		//create a new entry
		TA_App::IDirectoryCorbaDef::DirectoryEntryData* entry = new TA_App::IDirectoryCorbaDef::DirectoryEntryData;
		//copy the data
		entry->name		= CORBA::string_dup(newEntry.name);
		entry->number	= CORBA::string_dup(newEntry.number);
		entry->type		= newEntry.type;
		entry->location = newEntry.location;
		//add the new entry to the vector of entries
		m_entries.push_back(entry);
	}

	/**
	  * modifyEntry
	  */
	void TelephoneDirectory::modifyEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& modifiedEntry)
	{
		//Search for the entry
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//check the number
			if (!strcmp(m_entries[i]->number,modifiedEntry.number))
			{
				//it exists, modify it
				m_entries[i]->name		= CORBA::string_dup(modifiedEntry.name);
				m_entries[i]->type		= modifiedEntry.type;
				m_entries[i]->location	= modifiedEntry.location;
				//now exit
				return;
			}
		}
		//if we get here, it means it doesn't exist, we will have to add it
		addEntry(sessionId,modifiedEntry);
		return;
	}

	/**
	  * deleteEntry
	  */
	void TelephoneDirectory::deleteEntry(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& deletedEntry)
	{
		//find the entry
		for (unsigned int i(0); i < m_entries.size(); i++)
		{
			//check the number
			if (!strcmp(m_entries[i]->number,deletedEntry.number))
			{
				//it exists, delete it
				//copy the pointer
				TA_App::IDirectoryCorbaDef::DirectoryEntryData* deletedEntryPointer= m_entries[i];
				//create an iterator
				std::vector<TA_App::IDirectoryCorbaDef::DirectoryEntryData*>::iterator iter;
				//go through the vector again
				for (iter = m_entries.begin(); iter != m_entries.end(); iter++)
				{
					//check the number, again
					if (!strcmp((*iter)->number,deletedEntry.number))
					{
						//remove the entry from the vector
						m_entries.erase(iter);
						//delete the pointer
						delete deletedEntryPointer;
						break;
					}
				}
				//now exit
				return;
			}
		}
		//it didn't exist, throw an exception
		//TODO
	}
};
