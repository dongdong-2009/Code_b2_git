/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/DvaMsgLib.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include <sstream>
#include <iomanip>
#include "ace/OS.h"

#include "core/data_access_interface/pa_4669/src/PaSysParamAccessFactory.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/modbus_comms/src/DataBlock.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/DvaMsgLibDownload.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/DvaMsgLib.h"

namespace TA_IRS_App
{

DvaMsgLib::DvaMsgLib() : IPasPollingObserver(), IPasSerialDataProcessor(), ITransactionWorkerUser(),
m_strLibVersionBuffer( EMPTY_STRING ),
m_bProcessingLibUpdate( false ),
m_bForceUpdateDvaLib( false ),
m_usDvaMsgLibCounter( 0u ),
m_mapDvaMsgsBuffer(),
m_lstProcessingCounters()
{
    PasTransaction* pNewReading = 
        new DvaMsgLibDownload( CachedConfig::getInstance()->getIsMaster(), *this );

    PasTransactionManager::getInstance().bindTransactionWorker( *this, pNewReading, false );
}

DvaMsgLib::~DvaMsgLib()
{
    PasTransactionManager::getInstance().unbindTransactionWorker( *this );
}

void DvaMsgLib::processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp )
{
    if ( !dbData.isInRange( PAS_POLLING_DVA_LIB_COUNTER ))
    {
        // Polling data is not cared, return.
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "DataBlock[%d, %d] doesn't contain address %d, return", dbData.start(), dbData.end(), PAS_POLLING_DVA_LIB_COUNTER );
        return;
    }

    unsigned short usTempCounter = dbData[PAS_POLLING_DVA_LIB_COUNTER];

    if (( usTempCounter != m_usDvaMsgLibCounter ) || m_bForceUpdateDvaLib )
    {
        m_bForceUpdateDvaLib = false;
        m_usDvaMsgLibCounter = usTempCounter;

        // Refresh DVA library
        readDvaLibData();
    }
}

bool DvaMsgLib::processNewData( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    m_mapDvaMsgsBuffer.clear();
    return parseDvaMsgHwContent( dbData );
}

bool DvaMsgLib::processRemainData( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    return parseDvaMsgHwContent( dbData );
}

void DvaMsgLib::serialProcessFailed( int nReason )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Process DVA library download failed, error code %d", nReason );
}

void DvaMsgLib::processFinalData( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    int nStartAddress = 0;
    int nEndAddress = 0;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        nStartAddress = PAS_ADDRESS_OCC_DVA_LIB_VERSION_START;
        nEndAddress = PAS_ADDRESS_OCC_DVA_LIB_VERSION_END;
    }
    else
    {
        nStartAddress = PAS_ADDRESS_STN_DVA_LIB_VERSION_START;
        nEndAddress = PAS_ADDRESS_STN_DVA_LIB_VERSION_END;
    }

    if ( !dbData.isInRange( nStartAddress ) || !dbData.isInRange( nEndAddress ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "DataBlock[%d, %d] doesn't contain address [%d, %d], return", 
            dbData.start(), dbData.end(), nStartAddress, nEndAddress );
        return;
    }

    unsigned short usCurNameChar = 0;

    std::stringstream stmTemp;
    stmTemp << std::hex << std::setiosflags( std::ios::uppercase );

    // Decode the DVA library version, no need to check this CRC according to SI's update
    for ( int nMicroOffset = nStartAddress; nMicroOffset <= nEndAddress; ++nMicroOffset )
    {
        usCurNameChar = dbData[nMicroOffset];
        stmTemp << std::setw( 2 ) << std::setfill( '0' ) << ((usCurNameChar & 0xFF00) >> BIT_COUNT_IN_BYTE);
        stmTemp << std::setw( 2 ) << std::setfill( '0' ) << (usCurNameChar & 0x00FF);
    }

    m_strLibVersionBuffer = stmTemp.str().c_str();

    // All DVA update successfully, update them into database
    updateDvaMsgToDatabase();

    updateDvaLibVersionToDatabase();

    updatePasParamsToDatabase();
}

void DvaMsgLib::updateDvaMsgToDatabase()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to update DVA message into database" );

    // We check if there is any inconsistency between the new data and the data in the CachedMaps
    TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData seqDisposableUpdateData;

    unsigned short usMinDvaMsgId = 0;
    unsigned short usMaxDvaMsgId = 0;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        seqDisposableUpdateData.length( TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_DVA_NUM );
        usMinDvaMsgId = static_cast<unsigned short>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_DVA_NUM);
        usMaxDvaMsgId = static_cast<unsigned short>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_DVA_NUM);
    }
    else
    {
        seqDisposableUpdateData.length( TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_DVA_NUM );
        usMinDvaMsgId = static_cast<unsigned short>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_DVA_NUM);
        usMaxDvaMsgId = static_cast<unsigned short>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_DVA_NUM);
    }

    int nCountOfChanges = 0;

    unsigned long ulLocationKey = CachedConfig::getInstance()->getAgentLocationKey();

    for ( unsigned short usIdLoop = usMinDvaMsgId; usIdLoop <= usMaxDvaMsgId; ++usIdLoop )
    {
        TA_IRS_Bus::StationDvaMessageRecord dvaRecord;

        try
        {
            dvaRecord = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromIdAndLocation( usIdLoop, ulLocationKey );
        }
        // Current implement is not allow creating new record
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
            continue;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "Caught unknown exception when calling functions of CachedMaps" );
            continue;
        }

		if(PasHelpers::getInstance()->isAdHocDvaMsg( dvaRecord.usMsgType ))
		{
			continue;
		}

        MsgIdToDvaMsgHwMapIt itNewDvaFound = m_mapDvaMsgsBuffer.find( usIdLoop );

        bool bDvaMsgUpdated = false;
        unsigned short usOldMsgType = dvaRecord.usMsgType;
        std::string strOldMsgName = dvaRecord.strMsgName.c_str();

        if ( m_mapDvaMsgsBuffer.end() == itNewDvaFound )
        {
            if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == dvaRecord.usMsgType ) || 
                ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == dvaRecord.usMsgType ))
            {
                dvaRecord.usMsgType = TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK;
                bDvaMsgUpdated = true;
            }
            else if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED != dvaRecord.usMsgType ) && 
                ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK != dvaRecord.usMsgType ))
            {
                dvaRecord.usMsgType = TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED;
                bDvaMsgUpdated = true;
            }
        }
        else if (( itNewDvaFound->second.usMsgType  != dvaRecord.usMsgType ) || 
                    ( 0 != itNewDvaFound->second.strMsgName.compare( dvaRecord.strMsgName )))
        {
            // If old message is Ad-Hoc, new message should be Ad-Hoc, otherwise report an error
            if (( PasHelpers::getInstance()->isAdHocDvaMsg( dvaRecord.usMsgType )) && 
                ( PasHelpers::getInstance()->isAdHocDvaMsg( itNewDvaFound->second.usMsgType )))
            {
                // Don't change the name for the ad-hoc message when processing PAS DVA message update
                if (  dvaRecord.usMsgType != itNewDvaFound->second.usMsgType )
                {
                    dvaRecord.usMsgType  = itNewDvaFound->second.usMsgType;
                    bDvaMsgUpdated = true;
                }
            }
            // If old message is Pre-record, new message should be Pre-record, otherwise report an error
            else if (( !PasHelpers::getInstance()->isAdHocDvaMsg( dvaRecord.usMsgType )) && 
                ( !PasHelpers::getInstance()->isAdHocDvaMsg( itNewDvaFound->second.usMsgType )))
            {
                dvaRecord.usMsgType = itNewDvaFound->second.usMsgType;
                dvaRecord.strMsgName = itNewDvaFound->second.strMsgName.c_str();
                bDvaMsgUpdated = true;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "DVA message could not be changed from %d to %d",
                    dvaRecord.usMsgType, itNewDvaFound->second.usMsgType );
            }
        }

        if ( bDvaMsgUpdated )
        {
            try
            {
                TA_IRS_Bus::CachedMaps::getInstance()->setStationDvaMessageRecord( dvaRecord, true );
            }
            catch ( std::exception& expWhat )
            {
                // Failed to write the changes
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
                continue;
            }
            catch (...)
            {
                // Failed to write the changes
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                    "Caught unknown exception when calling CachedMaps::setStationDvaMessageRecord" );
                continue;
            }

            // submit an audit message
            // No such audit message in C955
            //PasHelpers::getInstance()->submitPaAgentDVAMessageUpdateAuditMessage( 
            //    usOldMsgType, dvaRecord.usMsgType, strOldMsgName, dvaRecord.strMsgName, dvaRecord.ulKey );

            // If failed to update into database and cache map, ignore this change
            seqDisposableUpdateData[nCountOfChanges].m_pKey = dvaRecord.ulKey;
            seqDisposableUpdateData[nCountOfChanges].m_id = dvaRecord.usMsgId;
            seqDisposableUpdateData[nCountOfChanges].m_locationKey = dvaRecord.ulLocationKey;
            seqDisposableUpdateData[nCountOfChanges].m_label = dvaRecord.strMsgName.c_str();
            seqDisposableUpdateData[nCountOfChanges].m_type = dvaRecord.usMsgType;

            // Detected at least one change
            ++nCountOfChanges;
        }
    } // for loop

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Update %d DVA messages into database done", nCountOfChanges );

    seqDisposableUpdateData.length( nCountOfChanges );

    if ( 0 < nCountOfChanges )
    {
        // Detected at least one change, so we broadcast it to all open PA Managers
        PasHelpers::getInstance()->sendDvaMessagesUpdate( seqDisposableUpdateData );
    }
}

void DvaMsgLib::readDvaLibData()
{
    m_lstProcessingCounters.push_back( m_usDvaMsgLibCounter );
    PasTransactionManager::getInstance().signalTransaction( *this );
}

bool DvaMsgLib::parseDvaMsgHwContent( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to parse DVA H/W Content" );

    int nStartAddress = 0;
    int nEndAddress = 0;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        nStartAddress = PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START;
        nEndAddress = PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END;
    }
    else
    {
        nStartAddress = PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START;
        nEndAddress = PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END;
    }

    if ( !dbData.isInRange( nStartAddress ) || !dbData.isInRange( nEndAddress ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "DataBlock[%d, %d] doesn't contain address [%d, %d], return", 
            dbData.start(), dbData.end(), nStartAddress, nEndAddress );
        return false;
    }

    const int DVA_MSG_CONTENT_LENGTH = 20;
    const int nLastMsgStartAddr = nEndAddress - DVA_MSG_CONTENT_LENGTH + 1;

    int nCurMsgStartAddr = nStartAddress;
    int nMicroOffset = 0;

    while ( nCurMsgStartAddr  <= nLastMsgStartAddr )
    {
        DvaMsgHwContent stNewDvaMsg;
        nMicroOffset = 0;

        // First WORD [0] is PA h/w index for DVA message
        unsigned short usIndex = dbData[nCurMsgStartAddr + nMicroOffset];
        ++nMicroOffset;

        // First WORD [1] is reserved
        unsigned short usReservedByte = dbData[nCurMsgStartAddr + nMicroOffset];
        ++nMicroOffset;

        // Second WORD [2] is message id
        stNewDvaMsg.usMsgId = dbData[nCurMsgStartAddr + nMicroOffset];
        ++nMicroOffset;

        if ( PAS_DVA_MSG_LIB_END_ID == stNewDvaMsg.usMsgId )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Parse DVA H/W Content end" );
            return false;
        }

        // Third WORD [3] is message bank id (type)
        stNewDvaMsg.usMsgType = dbData[nCurMsgStartAddr + nMicroOffset];
        ++nMicroOffset;

        unsigned short usCurNameChar = 0;
        char cMsbChar = END_OF_STRING;
        char cLsbChar = END_OF_STRING;
        // WORDs [4 - 19] is message name
        for ( ; nMicroOffset < DVA_MSG_CONTENT_LENGTH; ++nMicroOffset )
        {
            usCurNameChar = dbData[nCurMsgStartAddr + nMicroOffset];
            cLsbChar = static_cast<char>(usCurNameChar & MASK_LOW_BYTE);
            cMsbChar = static_cast<char>((usCurNameChar & MASK_HIGH_BYTE) >> BIT_COUNT_IN_BYTE);

            if ( END_OF_STRING == cMsbChar )
            {
                break;
            }
            stNewDvaMsg.strMsgName += cMsbChar;

            if ( END_OF_STRING == cLsbChar )
            {
                break;
            }
            stNewDvaMsg.strMsgName += cLsbChar;
        }

        m_mapDvaMsgsBuffer.insert( MsgIdToDvaMsgHwMap::value_type(stNewDvaMsg.usMsgId, stNewDvaMsg) );

        nCurMsgStartAddr += DVA_MSG_CONTENT_LENGTH;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Parse DVA H/W Content done with continue" );
    return true;
}

void DvaMsgLib::setDvaMsgLibCounter( const unsigned short usCounter )
{
    m_usDvaMsgLibCounter = usCounter;
}

void DvaMsgLib::setDvaMsgLibCounter( const std::string& strCounter )
{
    if ( strCounter.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Counter string is empty" );
        return;
    }

    std::stringstream stmConvertor;
    stmConvertor << strCounter;
    stmConvertor >> m_usDvaMsgLibCounter;
}

void DvaMsgLib::updatePasParamsToDatabase()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to update PAS parameters into database" );

    if ( m_lstProcessingCounters.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update PAS parameters" );
        return;
    }

    unsigned short usCurCounter = m_lstProcessingCounters.front();
    m_lstProcessingCounters.pop_front();

    unsigned long ulLocKey = CachedConfig::getInstance()->getAgentLocationKey();

    std::stringstream stmConvertor;
    stmConvertor << usCurCounter;

    try
    {
        TA_Base_Core::PaSysParamAccessFactory::getInstance().setPaSysParamValue( 
            ulLocKey, TA_Base_Core::PAS_PARAM_NAME::DVA_LIB_COUNTER_VALUE, stmConvertor.str() );
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when PasTransactionManager running" );
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Update PAS parameters into database done" );

    TA_Base_Bus::IPAAgentCorbaDef::PaSysParameter stSysParam;
    stSysParam.strName = CORBA::string_dup( TA_Base_Core::PAS_PARAM_NAME::DVA_LIB_COUNTER_VALUE );
    stSysParam.strValue = CORBA::string_dup( stmConvertor.str().c_str() );
    PasHelpers::getInstance()->sendPAAgentPaSysParamUpdate( stSysParam );
}

void DvaMsgLib::updateDvaLibVersionToDatabase()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to update DVA lib version into database" );

    unsigned long ulLocKey = CachedConfig::getInstance()->getAgentLocationKey();

    TA_IRS_Bus::DvaVersionRecord stVersionUpdated;
    try
    {
        stVersionUpdated = TA_IRS_Bus::CachedMaps::getInstance()->getDvaVersionRecordFromLocationKey( ulLocKey );
    }
    catch ( TA_Base_Core::CachedMappingNotFoundException& )
    {
        // New record
        stVersionUpdated.ulLocationKey = ulLocKey;
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when PasTransactionManager running" );
    }

    // Check whether the library version is changed
    if ( 0 != m_strLibVersionBuffer.compare( stVersionUpdated.strVersionNumber ))
    {
        stVersionUpdated.strVersionNumber = m_strLibVersionBuffer.c_str();
        try
        {
            TA_IRS_Bus::CachedMaps::getInstance()->setDvaMessageVersionRecord( stVersionUpdated, true );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Update DVA lib version into database done" );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "Caught unknown exception when PasTransactionManager running" );
        }
    }
}

}
