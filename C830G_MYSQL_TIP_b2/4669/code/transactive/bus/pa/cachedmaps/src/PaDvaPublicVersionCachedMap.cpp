/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/PaDvaPublicVersionCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
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

#include "bus/pa/cachedmaps/src/PaDvaPublicVersionCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessagePublicVersion.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{
    PaDvaPublicVersionCachedMap::PaDvaPublicVersionCachedMap()
    :
    GenericCachedMap<DvaPublicVersionRecord, TA_Base_Core::IPaDvaMessagePublicVersion>
        (PA_DVA_VERSION, "PaDvaVersion"),
        m_configUpdateSender(0)
    {
    }

    PaDvaPublicVersionCachedMap::~PaDvaPublicVersionCachedMap()
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


    TA_Base_Core::IPaDvaMessagePublicVersion* PaDvaPublicVersionCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaDvaMessagePublicVersionAccessFactory::getInstance().
                                getPaDvaMessagePublicVersion(pKey, readWrite);
    }


    void PaDvaPublicVersionCachedMap::validateRecordData(const DvaPublicVersionRecord& dvaVersion)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(
            0 == dvaVersion.m_locationKey, dvaVersion.m_pKey);

        /*
        NOTE: Don't re-enable this code without fixing up reference to CachedMaps
        (this is now used by config plugins, which don't set the location type in Cached Maps)
        // OCC only has public messages, don't validate private ones
        if (TA_Base_Core::IPaDvaMessageVersion::OCC != CachedMaps::getInstance()->getLocationType())
        {
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc1.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc2.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc3.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc4.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc5.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privateAdhoc6.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_privatePreRecorded.length(), dvaVersion.m_pKey);
        }

        // DEPOT only has private messages, don't validate public ones
        if (TA_Base_Core::IPaDvaMessageVersion::DEPOT != CachedMaps::getInstance()->getLocationType())
        {
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc1.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc2.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc3.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc4.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc5.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicAdhoc6.length(), dvaVersion.m_pKey);
            performDatavalueValidation(
                0 == dvaVersion.m_publicPreRecorded.length(), dvaVersion.m_pKey);
        }
        */
    }
       

    DvaPublicVersionRecord PaDvaPublicVersionCachedMap::createRecordFromInterface(TA_Base_Core::IPaDvaMessagePublicVersion& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        DvaPublicVersionRecord result;
    
        result.m_pKey = item.getKey();
        result.m_locationKey = item.getLocationKey();
    
        result.m_publicAdhoc1 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC1);
        result.m_publicAdhoc2 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC2);
        result.m_publicAdhoc3 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC3);
        result.m_publicAdhoc4 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC4);
        result.m_publicAdhoc5 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC5);
        result.m_publicAdhoc6 = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC6);
        result.m_publicPreRecorded = item.getVersionData(TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_PRERECORDED);
 
        return result;
    }


    void PaDvaPublicVersionCachedMap::refreshKeyToRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToRecordMap");

        TA_Base_Core::IPaDvaMessagePublicVersions dvaVersions;

        if (locationKey == -1)
        {
            // Obtain read only array of *all* DVA Message Versions
            dvaVersions = TA_Base_Core::PaDvaMessagePublicVersionAccessFactory::getInstance().
                                            getAllPaDvaMessagePublicVersions(false);
        }
        else
        {
            // Obtain read only array of location specific DVA Message Versions
            dvaVersions.push_back(TA_Base_Core::PaDvaMessagePublicVersionAccessFactory::getInstance().
                                            getPaDvaMessagePublicVersionByLocationKey(locationKey, false));
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

        FUNCTION_EXIT;
    }

    DvaPublicVersionRecord PaDvaPublicVersionCachedMap::getRecordFromLocation(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getRecordFromLocation");
    
        FilterByLocationKey filter(locationKey);
        DvaPublicVersionRecords matchingRecords = getFirstMatchingRecord(filter);
    
        if (matchingRecords.size() == 0)
        {
            // Record not found
            std::ostringstream msg;

            msg << "Failed to locate DvaPublicVersionRecord with locationKey=" 
                << locationKey << " in KeyToPaDvaVersionRecordMap";
	        TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

            FUNCTION_EXIT;
        }
    
        // Matching item found
        FUNCTION_EXIT;
        return matchingRecords.front();
    }




    unsigned long PaDvaPublicVersionCachedMap::getKeyFromLocation(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getKeyFromLocation");
    
        FilterByLocationKey filter(locationKey);
        DvaPublicVersionRecords matchingRecords = getFirstMatchingRecord(filter);
    
        if (matchingRecords.size() == 0)
        {
            // Record not found
            std::ostringstream msg;

            msg << "Failed to locate DvaPublicVersionRecord with locationKey=" 
                << locationKey << " in KeyToPaDvaVersionRecordMap";
	        TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

            FUNCTION_EXIT;
        }
    
        // Matching item found
        FUNCTION_EXIT;
        return matchingRecords.front().m_pKey;
    }

    void PaDvaPublicVersionCachedMap::setRecord( DvaPublicVersionRecord& record, 
                                           bool writeChangesToDataBase )
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        // Only nonzero primary key present in database
        bool isExistingRecord = (-1 != record.m_pKey && 0 != record.m_pKey);

        TA_ASSERT( isExistingRecord || writeChangesToDataBase, "setRecord called with a new record without writeChangesToDataBase");

        if (writeChangesToDataBase)
        {
			TA_Base_Core::IConfigPaDvaMessagePublicVersion* configPaDvaMessageVersion = NULL;

            if (isExistingRecord)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing modified PaDvaVersionRecord for pKey=%d", 
                   record.m_pKey);

                TA_Base_Core::IPaDvaMessagePublicVersion* paDvaMessageVersion = 
                    TA_Base_Core::PaDvaMessagePublicVersionAccessFactory::getInstance().getPaDvaMessagePublicVersion(record.m_pKey, true);
                configPaDvaMessageVersion = dynamic_cast<TA_Base_Core::IConfigPaDvaMessagePublicVersion*>(paDvaMessageVersion);

                TA_ASSERT(configPaDvaMessageVersion, "dynamic_cast failed on a configPaDvaMessageVersion");
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing new PaDvaVersionRecord");

                configPaDvaMessageVersion = TA_Base_Core::PaDvaMessagePublicVersionAccessFactory::getInstance().createPaDvaMessagePublicVersion();
            }

            try
            {

                configPaDvaMessageVersion->setLocationKey(record.m_locationKey);

                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc1, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC1);
                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc2, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC2);
                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc3, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC3);
                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc4, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC4);
                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc5, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC5);
                configPaDvaMessageVersion->setVersionData(record.m_publicAdhoc6, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_ADHOC6);
                configPaDvaMessageVersion->setVersionData(record.m_publicPreRecorded, TA_Base_Core::IPaDvaMessagePublicVersion::PUBLIC_PRERECORDED);

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
                    submitConfigUpdate(record.m_pKey, TA_Base_Core::Update, columnChanges);
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
        updateRecordMapWithItem(record.m_pKey, record);
    }

    // ExceptionChecked
    void PaDvaPublicVersionCachedMap::submitConfigUpdate( unsigned long recordKey, 
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

        TA_ASSERT(m_configUpdateSender, "m_configUpdateSender is NULL");

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

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PA_DVA_MESSAGE_PUBLIC_VERSION changed");
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