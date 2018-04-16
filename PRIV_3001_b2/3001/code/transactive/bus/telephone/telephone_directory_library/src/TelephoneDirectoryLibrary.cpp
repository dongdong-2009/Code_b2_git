/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Implementation of the Class TelephoneDirectoryLibrary
  */

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TelephoneManagerDirectoryBroadcastMessage_MessageTypes.h"
#include "bus/telephone/telephone_manager/IDL/src/ITelephoneManagerCommonCorbaDef.h"

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryItemStatus.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneSubDirectory.h"

using std::vector;
using std::string;

using TA_Base_Core::TransactiveException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::CommsMessageSender;
using TA_Base_Core::MessagePublicationManager;
using TA_Base_Core::DataConfigurationException;

using namespace TA_IRS_Core;


TelephoneDirectoryLibrary::TelephoneDirectoryLibrary()
{
    FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}

TelephoneDirectoryLibrary::~TelephoneDirectoryLibrary()
{
    FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

vector<TelephoneDirectoryEntryData> TelephoneDirectoryLibrary::getAllEntries()
{
    FUNCTION_ENTRY("getAllEntries");

    vector<TelephoneDirectoryEntryData> retList;
    ITelephoneDirectories activeEntryPtrList;

    TelephoneSubDirectory* directory = NULL;
	std::map<long,std::string>  MapAssociedData;
    unsigned int i = 0;

    try
    {
        activeEntryPtrList = TelephoneDirectoryAccessFactory::getInstance().getAllTelephoneDirectories();
		//libo++ 
        MapAssociedData = TelephoneSubDirectoryAccessFactory::getInstance().getAllAssociatedSubdirectoryMap();
		//++libo
        for (i = 0; i < activeEntryPtrList.size(); i++)
        {
            TelephoneDirectoryEntryData data;
            data.ID = activeEntryPtrList[i]->getPrimaryKey();
            data.number = activeEntryPtrList[i]->getTelephoneNum();
            data.fullName = activeEntryPtrList[i]->getFullName();
            data.location = activeEntryPtrList[i]->getLocation();
            //data.directoryDataName = directory->getSubDirName();
			//libo++
			std::map<long, std::string>::iterator it =MapAssociedData.find(activeEntryPtrList[i]->getSubDirKey());
			if(it != MapAssociedData.end())
			{
				data.directoryDataName = it->second;
			}
			//++libo
            retList.push_back(data);
       }

        cleanUpDirectories(activeEntryPtrList);
    }
    catch(DatabaseException& e)
    {
        cleanUpDirectories(activeEntryPtrList);
        retList.clear();
 
        if (directory != NULL)
        {
            delete directory;
        }

        FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
	//TD 18313
    catch(DataException& e)
    {
        if(e.getFailType() != DataException::NO_VALUE)
        {
            cleanUpDirectories(activeEntryPtrList);
            retList.clear();
			
			if (directory != NULL)
			{
				delete directory;
			}

            FUNCTION_EXIT;      
            TA_THROW(TelephoneDirectoryLibraryException(e.what()));
        }
    }
	//TD 18313

    FUNCTION_EXIT;
    return retList;
}

vector<TelephoneDirectoryData> TelephoneDirectoryLibrary::getAllDirectories()
{
    FUNCTION_ENTRY("getAllDirectories");

    vector<TelephoneDirectoryData> dirList;
    ITelephoneSubDirectories activeSubPtrList;

    unsigned int i = 0;

    try
    {
        activeSubPtrList = TelephoneSubDirectoryAccessFactory::getInstance().getAllTelephoneSubDirectories();

        for(i = 0; i < activeSubPtrList.size(); i++)
        {
            
            TelephoneDirectoryData data;
            data.ID  = activeSubPtrList[i]->getPrimaryKey();
            data.name = activeSubPtrList[i]->getSubDirName();
            dirList.push_back(data);
        }

        cleanUpSubDirectories(activeSubPtrList);
    }
    catch(DatabaseException& e)
    {
        cleanUpSubDirectories(activeSubPtrList);
        dirList.clear();

        FUNCTION_EXIT;      
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException& e)
    {
        if(e.getFailType() != DataException::NO_VALUE)
        {
            cleanUpSubDirectories(activeSubPtrList);
            dirList.clear();

            FUNCTION_EXIT;      
            TA_THROW(TelephoneDirectoryLibraryException(e.what()));
        }
    }

    FUNCTION_EXIT;
    return dirList;
}

vector<TelephoneDirectoryEntryData> TelephoneDirectoryLibrary::getAllDirectoryEntries(const std::string& directoryName)
{
    FUNCTION_ENTRY("getAllDirectoryEntries");
	FUNCTION_EXIT;
    return getTelephoneDirectoriesWhere(false, "", "", directoryName);
}

vector<TelephoneDirectoryEntryData> TelephoneDirectoryLibrary::getEntriesByName(const std::string& name)
{

    FUNCTION_ENTRY("getEntriesByName");
    FUNCTION_EXIT;
    return getTelephoneDirectoriesWhere(false, "", name, "");
}

//libo ++
TelephoneDirectoryEntryDataMap TelephoneDirectoryLibrary::getEntryByNumberScope(const std::string& numberScope)
{
    FUNCTION_ENTRY("getEntryByNumberScope");

    ITelephoneDirectories activeEntryPtrList;
    TelephoneDirectoryEntryData* retData = NULL;
    unsigned int i = 0;
	TelephoneDirectoryEntryDataMap retDataMap;

    try
    {
        activeEntryPtrList = TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectoriesScope(false, numberScope);

        if(activeEntryPtrList.size() == 0)
        {
            cleanUpDirectories(activeEntryPtrList);
            FUNCTION_EXIT;
            return retDataMap;
        }
		//libo++
		std::map<long,std::string>  MapAssociedData;
		MapAssociedData = TelephoneSubDirectoryAccessFactory::getInstance().getAllAssociatedSubdirectoryMap();
		//++libo

		for(int n =0 ;n< activeEntryPtrList.size(); n++)
		{
            retData = new TelephoneDirectoryEntryData();          
            retData->ID                 = activeEntryPtrList[n]->getPrimaryKey();
            retData->number             = activeEntryPtrList[n]->getTelephoneNum();
            retData->fullName           = activeEntryPtrList[n]->getFullName();
            retData->location           = activeEntryPtrList[n]->getLocation();
			//libo++
			std::map<long, std::string>::iterator it =MapAssociedData.find(activeEntryPtrList[n]->getSubDirKey());
			if(it != MapAssociedData.end())
			{
				retData->directoryDataName = it->second;
			}
			//++libo
		}
        cleanUpDirectories(activeEntryPtrList);
    }
    catch(DatabaseException& e)
    {
        cleanUpDirectories(activeEntryPtrList);
        FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException&)
    {
        FUNCTION_EXIT;
        return retDataMap;
    }

    FUNCTION_EXIT;
    return retDataMap;
}

//++libo

TelephoneDirectoryEntryData* TelephoneDirectoryLibrary::getEntryByNumber(const std::string& number)
{
    FUNCTION_ENTRY("getEntryByNumber");

    ITelephoneDirectories activeEntryPtrList;
    TelephoneSubDirectory* directory = NULL;
    TelephoneDirectoryEntryData* retData = NULL;
    unsigned int i = 0;

    try
    {
        activeEntryPtrList = TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectoriesWhere(false, number);

        if(activeEntryPtrList.size() > 1 || activeEntryPtrList.size() == 0)
        {
            cleanUpDirectories(activeEntryPtrList);
            FUNCTION_EXIT;
            return NULL;
        }

        directory = dynamic_cast<TelephoneSubDirectory*>(activeEntryPtrList[0]->getAssociatedSubdirectory());
        TA_ASSERT(directory != NULL, "ITelephoneDirectory object could not be converted into a TelephoneDirectory");

        if (directory != NULL)
        {
            retData = new TelephoneDirectoryEntryData();
            
            retData->ID                 = activeEntryPtrList[0]->getPrimaryKey();
            retData->number             = activeEntryPtrList[0]->getTelephoneNum();
            retData->fullName           = activeEntryPtrList[0]->getFullName();
            retData->location           = activeEntryPtrList[0]->getLocation();
            retData->directoryDataName  = directory->getSubDirName();
            
            delete directory;
            directory = NULL;
        }

        cleanUpDirectories(activeEntryPtrList);
    }
    catch(DatabaseException& e)
    {
        cleanUpDirectories(activeEntryPtrList);
        
        if(directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

        FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException&)
    {
        FUNCTION_EXIT;
        return NULL;
    }

    FUNCTION_EXIT;
    return retData;
}


bool TelephoneDirectoryLibrary::addEntry(const TelephoneDirectoryEntryData& entry)
{
    FUNCTION_ENTRY("addEntry");

    IConfigTelephoneDirectory* configEntryItem = NULL;
    ITelephoneSubDirectory* directory = NULL;

    unsigned long itemKey = 0;
    try
    {
        directory = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectoryWhere(true, entry.directoryDataName);
        if(directory == NULL)
        {
        	FUNCTION_EXIT;
            return false;
        }

        configEntryItem = TelephoneDirectoryAccessFactory::getInstance().createTelephoneDirectory();
        TA_ASSERT(configEntryItem != NULL, "IConfigTelephoneSubDirectory returns a NULL IConfigTelephoneSubDirectory item");

        ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(configEntryItem);
        TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");

        cast->setSubDirKey(directory->getPrimaryKey());
        cast->setTelephoneNum(entry.number);
        cast->setFullName(entry.fullName);
        cast->setLocation(entry.location);
        cast->setStatus(TA_IRS_Core::TELDIR_STATUS_ACTIVE);
        cast->applyChanges();
        
        itemKey = cast->getPrimaryKey();
        delete configEntryItem;
        configEntryItem = NULL;

        if(directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

    }
    catch(DataConfigurationException& configExp)
    {
        // Not all data has been specified when applyChanges are called
        TA_ASSERT(false, configExp.what());
    }
    catch(DatabaseException& e)
    {
        if(directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

        if (configEntryItem != NULL)
        {
            delete configEntryItem;
            configEntryItem = NULL;
        }

    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException&)
    {
        if (directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

        if (configEntryItem != NULL)
        {
            delete configEntryItem;
            configEntryItem = NULL;
        }
    	FUNCTION_EXIT;
        return false;
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    TA_IRS_Bus::DirectoryEntryEvent directoryEntry;
    directoryEntry.type = TA_IRS_Bus::NEW_ITEM;
    directoryEntry.ID = itemKey;
    directoryEntry.number = entry.number.c_str();
    directoryEntry.fullName = entry.fullName.c_str();
    directoryEntry.location = entry.location.c_str();
    directoryEntry.directoryDataName = entry.directoryDataName.c_str();

    CORBA::Any commsData;
    commsData <<= directoryEntry;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEntryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------
    
	FUNCTION_EXIT;
    return true;
}


bool TelephoneDirectoryLibrary::modifyEntry(const TelephoneDirectoryEntryData& newEntry, const TelephoneDirectoryEntryData& oldEntry)
{

    FUNCTION_ENTRY("modifyEntry");

    // Primary Key should not be modified at application level.
    // As such, in this function we expect both newEntry and oldEntry to have the same ID
    if(newEntry.ID != oldEntry.ID)
    {
        FUNCTION_EXIT;
        return false;
    }

    ITelephoneSubDirectory* directory = NULL;
    ITelephoneDirectory* entry = NULL;

    try
    {
        directory = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectoryWhere(false, oldEntry.directoryDataName);
        if(directory == NULL)
        {
        	FUNCTION_EXIT;
            return false;
        }

        entry = TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectory(oldEntry.ID, true);
        if(entry == NULL)
        {
            if(directory != NULL)
            {
                delete directory;
                directory = NULL;
            }

            FUNCTION_EXIT;
            return false;
        }

        ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(entry);
        TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");

        cast->setSubDirKey(directory->getPrimaryKey());
        cast->setTelephoneNum(newEntry.number);
        cast->setFullName(newEntry.fullName);
        cast->setLocation(newEntry.location);
        cast->setStatus(TA_IRS_Core::TELDIR_STATUS_ACTIVE);
        cast->applyChanges();

        delete cast;
        cast = NULL;

        if (directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

    }
    catch(DatabaseException& e)
    {
        if (directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

        if (entry != NULL)
        {
            delete entry;
            entry = NULL;
        }
        
    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException&)
    {
    	FUNCTION_EXIT;
        return false;
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    TA_IRS_Bus::DirectoryEntryEvent directoryEntry;
    directoryEntry.type = TA_IRS_Bus::UPDATE_ITEM;
    directoryEntry.ID = newEntry.ID;
    directoryEntry.number = newEntry.number.c_str();
    directoryEntry.fullName = newEntry.fullName.c_str();
    directoryEntry.location = newEntry.location.c_str();
    directoryEntry.directoryDataName = newEntry.directoryDataName.c_str();

    CORBA::Any commsData;
    commsData <<= directoryEntry;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEntryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------
    
	FUNCTION_EXIT;
    return true;
}

bool TelephoneDirectoryLibrary::deleteEntry(const TelephoneDirectoryEntryData& entry)
{
    FUNCTION_ENTRY("deleteEntry");
    ITelephoneDirectory* delEntry = NULL;
    try
    {
        delEntry = TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectory(entry.ID, true);
        if(delEntry == NULL)
        {
        	FUNCTION_EXIT;
            return false;
        }

        ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(delEntry);
        TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");

        cast->deleteThisObject();
        delete cast;
        cast = NULL;

    }
    catch(DataException&)
    {
        if (delEntry != NULL)
        {
            delete delEntry;
            delEntry = NULL;
        }
        return false;
    }
    catch(DatabaseException& e)
    {
    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));

    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    CORBA::Any commsData;
    TA_IRS_Bus::DirectoryEntryEvent directoryEntry;
    directoryEntry.type = TA_IRS_Bus::DELETE_ITEM;
    directoryEntry.ID = entry.ID;
    directoryEntry.number = entry.number.c_str();
    directoryEntry.fullName = entry.fullName.c_str();
    directoryEntry.location = entry.location.c_str();
    directoryEntry.directoryDataName = entry.directoryDataName.c_str();
    commsData <<= directoryEntry;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEntryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------

	FUNCTION_EXIT;
    return true;
}


bool TelephoneDirectoryLibrary::addDirectory(const TelephoneDirectoryData& directory)
{
    FUNCTION_ENTRY("addDirectory");

    unsigned long newDirKey = 0;
    IConfigTelephoneSubDirectory* newDir = NULL;
    try
    {
        newDir = TelephoneSubDirectoryAccessFactory::getInstance().createTelephoneSubDirectory();
        TA_ASSERT(newDir != NULL, "TelephoneSubDirectoryAccessFactory did not create ConfigTelephoneSubDirectory object");

        ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(newDir);
        TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");

        cast->setSubDirName(directory.name);
        cast->setStatus(TA_IRS_Core::TELDIR_STATUS_ACTIVE);
        cast->applyChanges();

        // Retrieve the primary key that should be set by ConfigTelephoneSubDirectory/Helper  
        // function when adding a new record to the database
        newDirKey = cast->getPrimaryKey();

        delete cast;
        cast = NULL;
    }
    catch(DataConfigurationException& configExp)
    {
        // Not all data has been specified when applyChanges are called
        TA_ASSERT(false, configExp.what());
    }
    catch(DatabaseException& e)
    {
        if (newDir != NULL)
        {
            delete newDir;
            newDir = NULL;
        }

    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException&)
    {
        if (newDir != NULL)
        {
            delete newDir;
            newDir = NULL;
        }

    	FUNCTION_EXIT;
        return false;
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    TA_IRS_Bus::DirectoryEvent corbaDir;
    corbaDir.type = TA_IRS_Bus::NEW_ITEM;
    corbaDir.ID = newDirKey;
    corbaDir.name = directory.name.c_str();
    
    CORBA::Any commsData;
    commsData <<= corbaDir;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------
    
    FUNCTION_EXIT;
    return true;
}


bool TelephoneDirectoryLibrary::modifyDirectory(const TelephoneDirectoryData& newDirectory, const TelephoneDirectoryData& oldDirectory)
{
    FUNCTION_ENTRY("modifyDirectory");
    ITelephoneSubDirectory* modDir = NULL;

    // Primary Key should not be modified at application level.
    // As such, in this function we expect both newDirectory and oldDirectory to have the same ID
    if(newDirectory.ID != oldDirectory.ID)
    {
        FUNCTION_EXIT;
        return false;
    }

    try
    {
        modDir = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectory(oldDirectory.ID, true);
        if(modDir == NULL)
        {
            return false;
        }

        ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(modDir);
        TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");

        cast->setSubDirName(newDirectory.name);
        cast->applyChanges();

        delete cast;
        cast = NULL;
    }
    catch(DataConfigurationException& e)
    {
        // Not all data has been specified when applyChanges are called
        if (modDir != NULL)
        {
            delete modDir;
            modDir = NULL;
        }
        
        FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DatabaseException& e)
    {
        if (modDir != NULL)
        {
            delete modDir;
            modDir = NULL;
        }
        
        FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    
    catch(DataException&)
    {
        if (modDir != NULL)
        {
            delete modDir;
            modDir = NULL;
        }

        FUNCTION_EXIT;
        return false;
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    CORBA::Any commsData;
    TA_IRS_Bus::DirectoryEvent corbaDir;
    corbaDir.type = TA_IRS_Bus::UPDATE_ITEM;
    corbaDir.ID = newDirectory.ID;
    corbaDir.name = newDirectory.name.c_str();
    commsData <<= corbaDir;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------
    FUNCTION_EXIT;
    return true;
}


bool TelephoneDirectoryLibrary::deleteDirectory(const TelephoneDirectoryData& directory)
{

    FUNCTION_ENTRY("deleteDirectory");
    ITelephoneSubDirectory* delDir = NULL;
    try
    {
        delDir = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectory(directory.ID, true);
        if(delDir == NULL)
        {
        	FUNCTION_EXIT;
            return false;
        }

        ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(delDir);
        TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");

        cast->setSubDirName(directory.name);
        try
        {
            cast->deleteThisObject();
        }
        catch(DataException& e)
        {
            delete cast;
            cast = NULL;
            if(e.getFailType() == DataException::CANNOT_DELETE)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
            	FUNCTION_EXIT;
                return false;
            }
            else
            {
            	FUNCTION_EXIT;
                TA_THROW(TelephoneDirectoryLibraryException(e.what()));
            }
        }

        delete cast;
        cast = NULL;
    }
    catch(DatabaseException& dbE)
    {
        if (delDir != NULL)
        {
            delete delDir;
            delDir = NULL;
        }

    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(dbE.what()));
    }
    catch(DataException&)
    {
        if (delDir != NULL)
        {
            delete delDir;
            delDir = NULL;
        }

    	FUNCTION_EXIT;
        return false;
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    CORBA::Any commsData;
    TA_IRS_Bus::DirectoryEvent corbaDir;
    corbaDir.type = TA_IRS_Bus::DELETE_ITEM;
    corbaDir.ID = directory.ID;
    corbaDir.name = directory.name.c_str();
    commsData <<= corbaDir;

    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------

	FUNCTION_EXIT;
    return true;
}


vector<TelephoneDirectoryEntryData> TelephoneDirectoryLibrary::searchDirectory(const std::string& directoryName, const std::string& searchstr)
{
    FUNCTION_ENTRY("searchDirectory");

    unsigned int i = 0;
    vector<TelephoneDirectoryEntryData> retList;
    ITelephoneSubDirectory* directory;
    ITelephoneDirectories entryList;

    try
    {
        entryList = TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectoriesWhere(false, "", searchstr, directoryName);
        for(i = 0; i < entryList.size(); i++)
        {
            directory = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectory(entryList[i]->getSubDirKey(), false);
            if(directory != NULL)
            {
                TelephoneDirectoryEntryData data;
                data.ID = entryList[i]->getPrimaryKey();
                data.number = entryList[i]->getTelephoneNum();
                data.fullName = entryList[i]->getFullName();
                data.location = entryList[i]->getLocation();
                data.directoryDataName = directory->getSubDirName();
                retList.push_back(data);

                delete directory;
            }
            else
            {
                //Should not reach this point
                TA_ASSERT(false, "Contact exists with a non-existance subdirectory");
            }
        }

        cleanUpDirectories(entryList);
    }
    catch(DatabaseException& e)
    {
        cleanUpDirectories(entryList);
        if(directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

    	FUNCTION_EXIT;
        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }
    catch(DataException& e)
    {
        cleanUpDirectories(entryList);
        if(directory != NULL)
        {
            delete directory;
            directory = NULL;
        }

    	FUNCTION_EXIT;
		TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }

	FUNCTION_EXIT;
    return retList;
}


bool TelephoneDirectoryLibrary::uploadDirectory(const std::vector<TelephoneDirectoryData*>& directories, 
                                                const std::vector<TelephoneDirectoryEntryData*>& entries)
{
    FUNCTION_ENTRY("uploadDirectory");

    unsigned int i = 0;

    // ---------------------------------------------------------------------------
    // add data into TE_SUBDIRECTORY and set status to new 
    // ---------------------------------------------------------------------------
    for( i = 0; i < directories.size(); i++)
    {
        ConfigTelephoneSubDirectory* pConfigSubDir = dynamic_cast<ConfigTelephoneSubDirectory*>(TelephoneSubDirectoryAccessFactory::getInstance().createTelephoneSubDirectory());
        TA_ASSERT(pConfigSubDir != NULL, "Could not cast IConfigTelephoneSubDirectory to ConfigTelephoneSubDirectory");

        // Primary key will be auto-retrieved and stored by the ConfigTelephoneSubDirectory/Helper class
        pConfigSubDir->setSubDirName(directories[i]->name);
        pConfigSubDir->setStatus(TA_IRS_Core::TELDIR_STATUS_NEW);

        try
        {
            pConfigSubDir->applyChanges();
        }
        catch(TransactiveException& e)
        {
            if (pConfigSubDir != NULL)
            {
                delete pConfigSubDir;
                pConfigSubDir = NULL;
            }

            removeNewlyAddedDirectories();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
            FUNCTION_EXIT;
            
            TA_THROW(TelephoneDirectoryLibraryException("Error uploading directory information"));
        }

    }
    
    // ---------------------------------------------------------------------------
    // Write all new directory entry information into the TE_DIRECTORY Table
    // all contacts into entries and set status to new 
    //  if no dir exist, throw exception 
    // ---------------------------------------------------------------------------
    ITelephoneSubDirectory* directory = NULL;
    for(i = 0; i < entries.size(); i++)
    {
        try
        {
            directory = TelephoneSubDirectoryAccessFactory::getInstance().getTelephoneSubDirectoryWhere(false, entries[i]->directoryDataName, TA_IRS_Core::TELDIR_STATUS_NEW);
            TA_ASSERT(directory != NULL, "No valid directory is available");
        }
        catch(TransactiveException& e) //This should not happen, directory should already exist
        {
            if (directory != NULL)
            {
                delete directory;
                directory = NULL;
            }
            
            removeNewlyAddedEntries();
            removeNewlyAddedDirectories();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
            FUNCTION_EXIT;
            TA_THROW(TelephoneDirectoryLibraryException("Error uploading contacts"));
        }

        ConfigTelephoneDirectory* pConfigEntryItem = dynamic_cast<ConfigTelephoneDirectory*>(TelephoneDirectoryAccessFactory::getInstance().createTelephoneDirectory());
        TA_ASSERT(pConfigEntryItem != NULL, "Could not cast IConfigTelephoneDirectory to ConfigTelephoneDirectory");

        pConfigEntryItem->setSubDirKey(directory->getPrimaryKey());
        pConfigEntryItem->setTelephoneNum(entries[i]->number);
        pConfigEntryItem->setFullName(entries[i]->fullName);
        pConfigEntryItem->setLocation(entries[i]->location);
        pConfigEntryItem->setStatus(TA_IRS_Core::TELDIR_STATUS_NEW);

        try
        {
            pConfigEntryItem->applyChanges();
        }
        catch(TransactiveException& e)
        {
            if (pConfigEntryItem != NULL)
            {
                delete pConfigEntryItem;
                pConfigEntryItem = NULL;
            }

            removeNewlyAddedEntries();
            removeNewlyAddedDirectories();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
            FUNCTION_EXIT;
            
            TA_THROW(TelephoneDirectoryLibraryException("Error uploading directory information"));
        }

        if (pConfigEntryItem != NULL)
        {
            delete pConfigEntryItem;
            pConfigEntryItem = NULL;
        }

    }

    // ---------------------------------------------------------------------------
    // All addition are successful, okay to delete old records now
    // delete all inactive directory and entries from table
    // ---------------------------------------------------------------------------
    try
    {
        ITelephoneDirectories inactiveEntryPtrList = 
                TelephoneDirectoryAccessFactory::getInstance().getAllTelephoneDirectories(true, TA_IRS_Core::TELDIR_STATUS_INACTIVE);

        for (i = 0; i < inactiveEntryPtrList.size(); i++)
        {
            ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(inactiveEntryPtrList[i]);
            TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");
            cast->deleteThisObject();
            delete cast;

        }
        inactiveEntryPtrList.clear();
    }
    catch(TransactiveException&)
    {
    }
   
    try
    {
        ITelephoneSubDirectories inactiveSubDirPtrList = 
                TelephoneSubDirectoryAccessFactory::getInstance().getAllTelephoneSubDirectories(true, TA_IRS_Core::TELDIR_STATUS_INACTIVE);

        for (i = 0; i < inactiveSubDirPtrList.size(); i++)
        {
            ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(inactiveSubDirPtrList[i]);
            TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");
            cast->deleteThisObject();
            delete cast;

        }
        inactiveSubDirPtrList.clear();
    }
    catch(TransactiveException&)
    {
    }

    // ---------------------------------------------------------------------------
    // Change all active state to inactive state 
    // ---------------------------------------------------------------------------
    ITelephoneDirectories newEntryPtrList;
    try
    {
        newEntryPtrList = TelephoneDirectoryAccessFactory::getInstance().getAllTelephoneDirectories(true,TA_IRS_Core::TELDIR_STATUS_ACTIVE);

        for (i = 0; i < newEntryPtrList.size(); i++)
        {
            ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(newEntryPtrList[i]);
            TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");
            cast->setStatus(TA_IRS_Core::TELDIR_STATUS_INACTIVE);
            cast->applyChanges();
            delete cast;

        }
        newEntryPtrList.clear();
    }
    catch(TransactiveException&)
    {
    }

    ITelephoneSubDirectories newSubDirPtrList;
    try
    {
         
        newSubDirPtrList = TelephoneSubDirectoryAccessFactory::getInstance().getAllTelephoneSubDirectories(true,TA_IRS_Core::TELDIR_STATUS_ACTIVE);

        for (i = 0; i < newSubDirPtrList.size(); i++)
        {
            ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(newSubDirPtrList[i]);
            TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");
            cast->setStatus(TA_IRS_Core::TELDIR_STATUS_INACTIVE);
            cast->applyChanges();
            delete cast;

        }
        newSubDirPtrList.clear();
    }
    catch(TransactiveException&)
    {
    }

    // ---------------------------------------------------------------------------
    // Change all new state to active state
    // ---------------------------------------------------------------------------
    // There should not be NO_VALUE / NON_UNIQUE DataException being thrown
    newEntryPtrList = 
            TelephoneDirectoryAccessFactory::getInstance().getAllTelephoneDirectories(true, TA_IRS_Core::TELDIR_STATUS_NEW);

    for (i = 0; i < newEntryPtrList.size(); i++)
    {
        ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(newEntryPtrList[i]);
        TA_ASSERT(cast != NULL, "IConfigTelephoneDirectory object could not be converted into a ConfigTelephoneDirectory");
        cast->setStatus(TA_IRS_Core::TELDIR_STATUS_ACTIVE);
        cast->applyChanges();
        delete cast;

    }
    newEntryPtrList.clear();

    // There should not be NO_VALUE / NON_UNIQUE DataException being thrown
    newSubDirPtrList = 
            TelephoneSubDirectoryAccessFactory::getInstance().getAllTelephoneSubDirectories(true, TA_IRS_Core::TELDIR_STATUS_NEW);

    for (i = 0; i < newSubDirPtrList.size(); i++)
    {
        ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(newSubDirPtrList[i]);
        TA_ASSERT(cast != NULL, "IConfigTelephoneSubDirectory object could not be converted into a ConfigTelephoneSubDirectory");
        cast->setStatus(TA_IRS_Core::TELDIR_STATUS_ACTIVE);
        cast->applyChanges();
        delete cast;

    }
    newSubDirPtrList.clear();
    

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    CommsMessageSender* sender = 
        MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context);

    CORBA::Any commsData;
    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryUploadEvent, commsData);

    delete sender;
    // ---------------------------------------------------------------------------

	FUNCTION_EXIT;
    return true;
}

void TelephoneDirectoryLibrary::removeNewlyAddedDirectories()
{
    ITelephoneSubDirectories newSubDirPtrList = 
            TelephoneSubDirectoryAccessFactory::getInstance().getAllTelephoneSubDirectories(true, TA_IRS_Core::TELDIR_STATUS_NEW);

    for(unsigned int i = 0; i < newSubDirPtrList.size(); i++)
    {
        ConfigTelephoneSubDirectory* cast = dynamic_cast<ConfigTelephoneSubDirectory*>(newSubDirPtrList[i]);
        TA_ASSERT(cast != NULL, "ITelephoneSubDirectory object could not be converted into ConfigTelephoneSubDirectory");
        cast->deleteThisObject();
        delete cast;
    }
    newSubDirPtrList.clear();
}

void TelephoneDirectoryLibrary::removeNewlyAddedEntries()
{
    ITelephoneDirectories newEntryPtrList = 
                TelephoneDirectoryAccessFactory::getInstance().getAllTelephoneDirectories(true, TA_IRS_Core::TELDIR_STATUS_NEW);

    for(unsigned int i = 0; i < newEntryPtrList.size(); i++)
    {
        ConfigTelephoneDirectory* cast = dynamic_cast<ConfigTelephoneDirectory*>(newEntryPtrList[i]);
        TA_ASSERT(cast != NULL, "ITelephoneSubDirectory object could not be converted into ConfigTelephoneSubDirectory");
        cast->deleteThisObject();
        delete cast;
    }
    newEntryPtrList.clear();
}

void TelephoneDirectoryLibrary::restoreAmmendedDirectories(std::vector<TA_IRS_Core::ConfigTelephoneSubDirectory*>& backupDir)
{
    for(unsigned int i = 0; i < backupDir.size(); i++)
    {
        try
        {
            backupDir[i]->applyChanges(); 
            delete backupDir[i];
        }
        catch(...)
        {
            TA_THROW(TelephoneDirectoryLibraryException("Error restoring old contacts") );
        }
    }
    backupDir.clear();
}

void TelephoneDirectoryLibrary::restoreAmmendedDirectoryEntries(std::vector<TA_IRS_Core::ConfigTelephoneDirectory*>& backupDirEntryList)
{
    for(unsigned int i = 0; i < backupDirEntryList.size(); i++)
    {
        try
        {
            backupDirEntryList[i]->applyChanges(); 
            delete backupDirEntryList[i];
        }
        catch(...)
        {
            TA_THROW(TelephoneDirectoryLibraryException("Error restoring old contacts") );
        }
    }
    backupDirEntryList.clear();
}

void TelephoneDirectoryLibrary::cleanUpDirectories(TA_IRS_Core::ITelephoneDirectories& dirList)
{
    for(unsigned int i = 0; i < dirList.size(); i++)
    {
        TelephoneDirectory* cast = dynamic_cast<TelephoneDirectory*>(dirList[i]);
        TA_ASSERT(cast != NULL, "ITelephoneDirectory object could not be converted into a TelephoneDirectory");
        delete cast;
    }
    dirList.clear();
}

void TelephoneDirectoryLibrary::cleanUpSubDirectories(TA_IRS_Core::ITelephoneSubDirectories& subDirList)
{
    for(unsigned int i = 0; i < subDirList.size(); i++)
    {
        TelephoneSubDirectory* cast = dynamic_cast<TelephoneSubDirectory*>(subDirList[i]);
        TA_ASSERT(cast != NULL, "ITelephoneSubDirectory object could not be converted into a TelephoneSubDirectory");
        delete cast;
    }
    subDirList.clear();
}

vector<TelephoneDirectoryEntryData> TelephoneDirectoryLibrary::getTelephoneDirectoriesWhere(  
                                                                                const bool readWrite,
                                                                                const std::string& telephoneNum,
                                                                                const std::string& fullName,
                                                                                const std::string& dirname,
                                                                                const TA_IRS_Core::TelephoneDirectoryItemStatus stateOfRecords)
{
    FUNCTION_ENTRY("getTelephoneDirectoriesWhere");

    vector<TelephoneDirectoryEntryData> retList;
    ITelephoneDirectories activeEntryPtrList;

    TelephoneSubDirectory* directory = NULL;
    unsigned int i = 0;

    try
    {
        activeEntryPtrList = 
            TelephoneDirectoryAccessFactory::getInstance().getTelephoneDirectoriesWhere(false, telephoneNum, fullName, dirname, stateOfRecords);

        for(i = 0; i < activeEntryPtrList.size(); i++)
        {
            directory = dynamic_cast<TelephoneSubDirectory*>(activeEntryPtrList[i]->getAssociatedSubdirectory());

            if (directory != NULL)
            {
                TelephoneDirectoryEntryData data;
                data.ID = activeEntryPtrList[i]->getPrimaryKey();
                data.number = activeEntryPtrList[i]->getTelephoneNum();
                data.fullName = activeEntryPtrList[i]->getFullName();
                data.location = activeEntryPtrList[i]->getLocation();
                data.directoryDataName = directory->getSubDirName();
                retList.push_back(data);

                delete directory;
            }
            else
            {
                // Should not happen
                TA_ASSERT(false, "ITelephoneDirectory object could not be converted into a TelephoneDirectory");
            }
        }

        cleanUpDirectories(activeEntryPtrList);
    }
    catch(DatabaseException& e)
    {
        cleanUpDirectories(activeEntryPtrList);
        if(directory != NULL)
        {
            delete directory;
        }

        TA_THROW(TelephoneDirectoryLibraryException(e.what()));
    }

	FUNCTION_EXIT;
    return retList;

}
