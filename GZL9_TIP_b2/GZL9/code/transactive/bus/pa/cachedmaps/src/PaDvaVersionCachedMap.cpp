/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/PaDvaVersionCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of DVA Version mappings, also listens
  * out for changes to database regarding the PA_DVA_VERSION table
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>

#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/pa/src/IConfigPaDvaMessageVersion.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{
    PaDvaVersionCachedMap::PaDvaVersionCachedMap()
    :
    GenericCachedMap<DvaVersionRecord, TA_Base_Core::IPaDvaMessageVersion>
        (PA_DVA_VERSION, "PaDvaVersion"),
        m_configUpdateSender(0)
    {
    }

    PaDvaVersionCachedMap::~PaDvaVersionCachedMap()
    {
        try
        {
            delete m_configUpdateSender;
            m_configUpdateSender = 0;
        }
        catch (...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    TA_Base_Core::IPaDvaMessageVersion* PaDvaVersionCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaDvaMessageVersionAccessFactory::getInstance().
                                getPaDvaMessageVersion(pKey, readWrite);
    }


    void PaDvaVersionCachedMap::validateRecordData(const DvaVersionRecord& dvaVersion)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(
            0 == dvaVersion.ulLocationKey, dvaVersion.ulKey);

        /*
        NOTE: Don't re-enable this code without fixing up reference to CachedMaps
        (this is now used by config plugins, which don't set the location type in Cached Maps)
        // OCC only has public messages, don't validate private ones
        if (TA_Base_Core::IPaDvaMessageVersion::OCC != CachedMaps::getInstance()->getLocationType())
        {
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc1.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc2.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc3.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc4.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc5.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc6.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privatePreRecorded.length(), dvaVersion.ulKey);
        }

        // DEPOT only has private messages, don't validate public ones
        if (TA_Base_Core::IPaDvaMessageVersion::DEPOT != CachedMaps::getInstance()->getLocationType())
        {
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc1.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc2.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc3.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc4.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc5.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc6.length(), dvaVersion.ulKey);
            performDatavalueValidation(
                0 == dvaVersion.strVersionNumber.length(), dvaVersion.ulKey);
        }
        */
    }
       

    DvaVersionRecord PaDvaVersionCachedMap::createRecordFromInterface(TA_Base_Core::IPaDvaMessageVersion& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        DvaVersionRecord result;
    
        result.ulKey = item.getKey();
        result.ulLocationKey = item.getLocationKey();
    
        result.strVersionNumber = item.getVersionData();
    

        return result;
    }


    void PaDvaVersionCachedMap::refreshKeyToRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToRecordMap");

        TA_Base_Core::IPaDvaMessageVersions dvaVersions;

		try
		{
			if (locationKey == -1)
			{
				// Obtain read only array of *all* DVA Message Versions
				dvaVersions = TA_Base_Core::PaDvaMessageVersionAccessFactory::getInstance().
												getAllPaDvaMessageVersions(false);
			}
			else
			{
				// Obtain read only array of location specific DVA Message Versions
				dvaVersions.push_back(TA_Base_Core::PaDvaMessageVersionAccessFactory::getInstance().
												getPaDvaMessageVersionByLocationKey(locationKey, false));
			}
		}
		catch (std::exception& expWhat)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "" );
		}

        try
        {
            refreshKeyToRecordMapUsing(dvaVersions);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(dvaVersions);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(dvaVersions);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PaDvaVersionCachedMap refreshed and loaded %d items of location %d", m_keyToRecordMap.size(), locationKey );
        FUNCTION_EXIT;
    }

    DvaVersionRecord PaDvaVersionCachedMap::getRecordFromLocation(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getRecordFromLocation");
    
        FilterByLocationKey filter(locationKey);
        DvaVersionRecords matchingRecords = getFirstMatchingRecord(filter);
    
        if (matchingRecords.size() == 0)
        {
            // Record not found
            std::ostringstream msg;

            msg << "Failed to locate DvaVersionRecord with locationKey=" 
                << locationKey << " in KeyToPaDvaVersionRecordMap";
	        TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

            FUNCTION_EXIT;
        }
    
        // Matching item found
        FUNCTION_EXIT;
        return matchingRecords.front();
    }




    unsigned long PaDvaVersionCachedMap::getKeyFromLocation(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getKeyFromLocation");
    
        FilterByLocationKey filter(locationKey);
        DvaVersionRecords matchingRecords = getFirstMatchingRecord(filter);
    
        if (matchingRecords.size() == 0)
        {
            // Record not found
            std::ostringstream msg;

            msg << "Failed to locate DvaVersionRecord with locationKey=" 
                << locationKey << " in KeyToPaDvaVersionRecordMap";
	        TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

            FUNCTION_EXIT;
        }
    
        // Matching item found
        FUNCTION_EXIT;
        return matchingRecords.front().ulKey;
    }

    void PaDvaVersionCachedMap::setRecord( DvaVersionRecord& record, 
                                           bool writeChangesToDataBase )
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        // Only nonzero primary key present in database
        bool isExistingRecord = (-1 != record.ulKey && 0 != record.ulKey);

		if (!(isExistingRecord || writeChangesToDataBase))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "setRecord called with a new record without writeChangesToDataBase" );
			TA_THROW(TA_Base_Core::TransactiveException("setRecord called with a new record without writeChangesToDataBase"));
		}

        if (writeChangesToDataBase)
        {
			TA_Base_Core::IConfigPaDvaMessageVersion* configPaDvaMessageVersion = NULL;

            if (isExistingRecord)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing modified PaDvaVersionRecord for pKey=%d", 
                   record.ulKey);

                TA_Base_Core::IPaDvaMessageVersion* paDvaMessageVersion = 
                    TA_Base_Core::PaDvaMessageVersionAccessFactory::getInstance().getPaDvaMessageVersion(record.ulKey, true);
                configPaDvaMessageVersion = dynamic_cast<TA_Base_Core::IConfigPaDvaMessageVersion*>(paDvaMessageVersion);

				if (NULL == configPaDvaMessageVersion)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "dynamic_cast failed on a configPaDvaMessageVersion" );
					TA_THROW(TA_Base_Core::TransactiveException("dynamic_cast failed on a configPaDvaMessageVersion"));
				}
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing new PaDvaVersionRecord");

                configPaDvaMessageVersion = TA_Base_Core::PaDvaMessageVersionAccessFactory::getInstance().createPaDvaMessageVersion();
            }

            try
            {

                configPaDvaMessageVersion->setLocationKey(record.ulLocationKey);
                configPaDvaMessageVersion->setVersionData(record.strVersionNumber);

                configPaDvaMessageVersion->applyChanges();

                std::vector<std::string> columnChanges;
                TA_Base_Core::ItemChanges changeMap = configPaDvaMessageVersion->getAllItemChanges();
                for (TA_Base_Core::ItemChanges::iterator it = changeMap.begin();
                            it != changeMap.end();
                            it ++)
                {
                    columnChanges.push_back(it->first);
                }

                if (isExistingRecord)
                {
                    // Modification of existing
                    submitConfigUpdate(record.ulKey, TA_Base_Core::Update, columnChanges);
                }
                else
                {
                    // New group added
                    submitConfigUpdate(configPaDvaMessageVersion->getKey(), TA_Base_Core::Create, columnChanges);
                }

                delete configPaDvaMessageVersion;
                configPaDvaMessageVersion = NULL;

            }
            catch(...)
            {
                delete configPaDvaMessageVersion;
                configPaDvaMessageVersion = NULL;

                throw;
            }

        } // if writeChanges

    
        // Update the local cached data - assume record may or may not exist
        updateRecordMapWithItem(record.ulKey, record);
    }

    // ExceptionChecked
    void PaDvaVersionCachedMap::submitConfigUpdate( unsigned long recordKey, 
                                                    TA_Base_Core::EModificationType modificationType, 
                                                    const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY("submitConfigUpdate");   
    
        if (0 == m_configUpdateSender)
        {
            // 4669FAT TD #782: Postponed object creation (out of constructor), so only
            // created on an as-needed basis (config editor doesn't require this)
            m_configUpdateSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().
                getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        }

		if (NULL == m_configUpdateSender)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_configUpdateSender is NULL" );
			return;
		}

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Sending ConfigPaDvaVersion ConfigUpdate message to subscribers");


        try
        {
            m_configUpdateSender->sendConfigUpdateMessage(
                TA_Base_Core::ConfigUpdate::ConfigPaDvaVersion,          // Message Type
                recordKey,                                          // Key of changed item
                modificationType,                                   // EModificationType (upd/del)
                changes,                                            // Desc of changes (col names)
                0);                                                 // FilterableData

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PA_DVA_MESSAGE_VERSION changed");
        } // try block
        catch(...)
        {
            std::ostringstream msg;
            msg << "Failed to submit the PaDvaVersionCachedMap Config Update Message";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
        }    

        FUNCTION_EXIT;
    }

} // end namespace TA_IRS_Bus